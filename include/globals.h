#pragma once

#include <Windows.h>

#define STEAM_API_EXPORTS
#include "include/sdk/steam_api.h"
#include "include/sdk/steamclientpublic.h"
#include "include/sdk/steam_gameserver.h"

extern HMODULE g_ClientModule;
extern HSteamPipe g_ClientPipe;
extern HSteamUser g_ClientUser;
extern ISteamClient* g_pSteamClient;
extern ISteamClient* g_pSteamClientSafe;
extern ISteamUtils* g_pUtilsForCallbacks;
extern ISteamController* g_pControllerForCallbacks;
extern ISteamInput* g_pInputForCallbacks;
extern CSteamAPIContext g_ClientCtx;
extern bool g_bClientReady;
extern SRWLOCK g_CtxLock;

extern HMODULE g_ServerModule;
extern HSteamPipe g_ServerPipe;
extern HSteamUser g_ServerUser;
extern ISteamClient* g_ServerClient;
extern ISteamClient* g_pServerClient;
extern ISteamClient* g_pSteamClientGameServer;
extern ISteamClient* g_pSteamClientGameServer_Latest;
extern ISteamGameServer* g_pGameServer;
extern ISteamUtils* g_pServerUtils;
extern CSteamGameServerAPIContext g_ServerCtx;
extern bool g_bServerReady;
extern EServerMode g_ServerMode;

extern bool g_bUsingBreakpad;
extern bool g_bFullDumps;
extern void* g_BreakpadCtx;
extern PFNPreMinidumpCallback g_BreakpadCallback;
extern char g_BreakpadVer[64];
extern char g_BreakpadTimestamp[64];
extern uint32 g_BreakpadAppID;
extern uint64 g_BreakpadSteamID;

extern bool g_bTryCatch;
extern int g_DispatchMode;
extern char g_InstallPath[MAX_PATH];
extern bool g_bHaveInstallPath;
extern SRWLOCK g_CallbackLock;
extern uint32 g_ForcedAppId;

typedef void* (S_CALLTYPE* Fn_CreateInterface)(const char* pName, int* pReturnCode);
extern Fn_CreateInterface g_pfnCreateInterface;

typedef void (S_CALLTYPE* Fn_ReleaseThreadLocal)(int bThreadExit);
extern Fn_ReleaseThreadLocal g_pfnReleaseThreadLocal;

typedef bool (S_CALLTYPE* Fn_IsKnownInterface)(const char* Interface);
extern Fn_IsKnownInterface g_pfnIsKnownInterface;

typedef void (S_CALLTYPE* Fn_NotifyMissing)(HSteamPipe hPipe, const char* Interface);
extern Fn_NotifyMissing g_pfnNotifyMissing;

typedef void (S_CALLTYPE* Fn_BreakpadInit)(uint32 AppID, const char* ver, const char* date, bool bFull, void* ctx, PFNPreMinidumpCallback cb);
extern Fn_BreakpadInit g_pfnBreakpadInit;

typedef void (S_CALLTYPE* Fn_BreakpadSetAppID)(uint32 AppID);
extern Fn_BreakpadSetAppID g_pfnBreakpadSetAppID;

typedef void (S_CALLTYPE* Fn_BreakpadSetSteamID)(uint64 SteamID);
extern Fn_BreakpadSetSteamID g_pfnBreakpadSetSteamID;

typedef void (S_CALLTYPE* Fn_BreakpadSetComment)(const char* Comment);
extern Fn_BreakpadSetComment g_pfnBreakpadSetComment;

typedef void (S_CALLTYPE* Fn_BreakpadWriteDump)(uint32 Code, void* Info, uint32 BuildID);
extern Fn_BreakpadWriteDump g_pfnBreakpadWriteDump;

extern uintp g_CtxCounter;

void UCOLOG(const char* fmt, ...);
void UCOColor(WORD color, const char* text);
void* InitSteamClient(HMODULE* phModule, bool bLocal, const char* iface);
void LoadBreakpadSymbols(HMODULE hMod);
void UpdateMinidumpSteamID(uint64 sid);
