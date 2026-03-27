#include <Windows.h>
#include <Shlwapi.h>
#include <new.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define STEAM_API_EXPORTS

#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/sdk/steam_gameserver.h"

#include "include/callback_dispatcher.h"
#include "include/globals.h"
#include "include/dll_loader.h"

#include "include/api/api_client.h"
#include "include/api/api_interfaces.h"
#include "include/api/api_interfaces_v.h"
#include "include/api/api_gameserver.h"
#include "include/api/api_breakpad.h"
#include "include/api/api_callbacks.h"
#include "include/api/api_shutdown.h"
#include "include/api/api_factory.h"
#include "include/api/api_flat.h"

// Global variable definitions
HMODULE g_ClientModule = nullptr;
HSteamPipe g_ClientPipe = 0;
HSteamUser g_ClientUser = 0;
ISteamClient* g_pSteamClient = nullptr;
ISteamClient* g_pSteamClientSafe = nullptr;
ISteamUtils* g_pUtilsForCallbacks = nullptr;
ISteamController* g_pControllerForCallbacks = nullptr;
ISteamInput* g_pInputForCallbacks = nullptr;
CSteamAPIContext g_ClientCtx;
bool g_bClientReady = false;
SRWLOCK g_CtxLock;

HMODULE g_ServerModule = nullptr;
HSteamPipe g_ServerPipe = 0;
HSteamUser g_ServerUser = 0;
ISteamClient* g_ServerClient = nullptr;
ISteamClient* g_pServerClient = nullptr;
ISteamClient* g_pSteamClientGameServer = nullptr;
ISteamClient* g_pSteamClientGameServer_Latest = nullptr;
ISteamGameServer* g_pGameServer = nullptr;
ISteamUtils* g_pServerUtils = nullptr;
CSteamGameServerAPIContext g_ServerCtx;
bool g_bServerReady = false;
EServerMode g_ServerMode = eServerModeInvalid;

bool g_bUsingBreakpad = false;
bool g_bFullDumps = false;
void* g_BreakpadCtx = nullptr;
PFNPreMinidumpCallback g_BreakpadCallback = nullptr;
char g_BreakpadVer[64] = { 0 };
char g_BreakpadTimestamp[64] = { 0 };
uint32 g_BreakpadAppID = 0;
uint64 g_BreakpadSteamID = 0;

bool g_bTryCatch = false;
int g_DispatchMode = 0;
char g_InstallPath[MAX_PATH] = { 0 };
bool g_bHaveInstallPath = false;
SRWLOCK g_CallbackLock;
uint32 g_ForcedAppId = 480;

Fn_CreateInterface g_pfnCreateInterface = nullptr;
Fn_ReleaseThreadLocal g_pfnReleaseThreadLocal = nullptr;
Fn_IsKnownInterface g_pfnIsKnownInterface = nullptr;
Fn_NotifyMissing g_pfnNotifyMissing = nullptr;
Fn_BreakpadInit g_pfnBreakpadInit = nullptr;
Fn_BreakpadSetAppID g_pfnBreakpadSetAppID = nullptr;
Fn_BreakpadSetSteamID g_pfnBreakpadSetSteamID = nullptr;
Fn_BreakpadSetComment g_pfnBreakpadSetComment = nullptr;
Fn_BreakpadWriteDump g_pfnBreakpadWriteDump = nullptr;

uintp g_CtxCounter = 0;

S_API void* S_CALLTYPE SteamInternal_ContextInit(void* pData)
{
	UCOLOG("[UCOnline2] SteamInternal_ContextInit");
	if (!pData) return nullptr;
	void** pArr = (void**)pData;
	uintp* pCounter = (uintp*)&pArr[1];
	char* pBase = (char*)pData;
	#if defined(_M_IX86)
		if (*pCounter == g_CtxCounter) return pBase + 8;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 8); *pCounter = g_CtxCounter; }
		ReleaseSRWLockExclusive(&g_CtxLock);
		return pBase + 8;
	#elif defined(_M_AMD64)
		if (*pCounter == g_CtxCounter) return pBase + 16;
		AcquireSRWLockExclusive(&g_CtxLock);
		if (*pCounter != g_CtxCounter) { void(*pFn)(void*) = (void(*)(void*))pArr[0]; pFn(pBase + 16); *pCounter = g_CtxCounter; }
		ReleaseSRWLockExclusive(&g_CtxLock);
		return pBase + 16;
	#endif
}

#ifdef _DEBUG

static void UCOLogImpl(const char* fmt, va_list args)
{
	char msg[2048] = { 0 };
	_vsnprintf_s(msg, sizeof(msg), _TRUNCATE, fmt, args);

	char logPath[MAX_PATH] = { 0 };
	DWORD len = GetTempPathA(MAX_PATH, logPath);
	if (len == 0 || len > (MAX_PATH - 25)) return;

	if (!PathAppendA(logPath, "uc_online2.log")) return;

	FILE* f = nullptr;
	if (fopen_s(&f, logPath, "ab") != 0 || !f) return;

	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	fprintf(f, "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msg);

	size_t msgLen = strlen(msg);
	if (msgLen == 0 || msg[msgLen - 1] != '\n')
		fputs("\r\n", f);

	fclose(f);
}
#endif

void UCOLOG(const char* fmt, ...)
{
#ifdef _DEBUG
	if (!fmt) return;
	va_list args;
	va_start(args, fmt);
	UCOLogImpl(fmt, args);
	va_end(args);
#endif
}

void UCOColor(WORD color, const char* text)
{
	(void)color;
#ifdef _DEBUG
	if (text && text[0])
		UCOLOG("%s", text);
#endif
}

void* InitSteamClient(HMODULE* phMod, bool bLocal, const char* iface)
{
	g_pUtilsForCallbacks = nullptr;
	g_pControllerForCallbacks = nullptr;
	g_pInputForCallbacks = nullptr;

	if (!phMod || !iface) return nullptr;

	*phMod = nullptr;

	char dllPath[MAX_PATH] = { 0 };
	const char* installDir = SteamAPI_GetSteamInstallPath();

	if (_stricmp(installDir, "UCOnline2_InvalidPath") == 0)
	{
		if (!bLocal) return nullptr;
	}
	else
	{
		#if defined(_M_IX86)
			_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\steamclient.dll", installDir);
		#elif defined(_M_AMD64)
			_snprintf_s(dllPath, MAX_PATH, _TRUNCATE, "%s\\steamclient64.dll", installDir);
		#endif
	}

	if (SteamAPI_IsSteamRunning())
	{
		*phMod = LoadLibraryExA(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!*phMod)
			UCOColor(0, "[UCOnline2] Failed to load steamclient DLL");
	}
	else
	{
		UCOColor(0, "[UCOnline2] Steam is not running");
	}

	if (!*phMod)
	{
		if (!bLocal)
		{
			UCOColor(0, "[UCOnline2] No Steam instance and bLocal is false");
			return nullptr;
		}

		#if defined(_M_IX86)
			*phMod = LoadLibraryW(L"steamclient.dll");
		#elif defined(_M_AMD64)
			*phMod = LoadLibraryW(L"steamclient64.dll");
		#endif

		if (!*phMod)
		{
			UCOColor(0, "[UCOnline2] Cannot find steamclient DLL");
			return nullptr;
		}
	}

	g_pfnCreateInterface = (Fn_CreateInterface)GetProcAddress(*phMod, "CreateInterface");

	if (g_pfnCreateInterface)
	{
		g_pSteamClientSafe = (ISteamClient*)g_pfnCreateInterface("SteamClient021", nullptr);
		g_pfnReleaseThreadLocal = (Fn_ReleaseThreadLocal)GetProcAddress(*phMod, "Steam_ReleaseThreadLocalMemory");
		g_CtxCounter++;

		return g_pfnCreateInterface(iface, nullptr);
	}
	else
	{
		UCOColor(0, "[UCOnline2] CreateInterface not found in steamclient");
		FreeLibrary(*phMod);
		*phMod = nullptr;
	}

	return nullptr;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		UCOLOG("[UCOnline2] DllMain -> DLL_PROCESS_ATTACH");

		static CDLLLoader s_PluginLoader;
		s_PluginLoader.ReadConfig();
		g_ForcedAppId = s_PluginLoader.GetAppId();

		SetAppIDEnv();
		WriteAppIDFile();

		char dllPath[MAX_PATH] = { 0 };
		DWORD len = GetModuleFileNameA(hModule, dllPath, sizeof(dllPath));

		if (len == 0)
			return FALSE;

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			return FALSE;

		UCOLOG("[UCOnline2] DLL Path: %s", dllPath);

		#if defined(_M_IX86)
			if (!StrStrIA(dllPath, "steam_api.dll"))
				UCOLOG("[UCOnline2] Warning: not named steam_api.dll");
		#elif defined(_M_AMD64)
			if (!StrStrIA(dllPath, "steam_api64.dll"))
				UCOLOG("[UCOnline2] Warning: not named steam_api64.dll");
		#endif

		UCOLOG("[UCOnline2] PID: %lu", GetCurrentProcessId());
		UCOLOG("[UCOnline2] Thread: %lu", GetCurrentThreadId());

		InitializeSRWLock(&g_CtxLock);
		InitializeSRWLock(&g_CallbackLock);

		#ifdef _DEBUG
		g_pDumpHandler = new CDumpHandler();
		#endif

		s_PluginLoader.LoadPlugins();

		UCOLOG("[UCOnline2] %zu plugin(s) loaded", s_PluginLoader.LoadedCount());
	}

	return TRUE;
}
