// ============================================================
// UCOnline2 plugin -- PlayFab authentication bypass
//
// Many Unity games use PlayFab for multiplayer authentication,
// often with Steam integration. The typical flow is:
//
//   PlayFabClientAPI.LoginWithSteam(steamTicket)
//
// which validates the Steam ticket against PlayFab's backend.
// With UCOnline2 spoofing AppId = 480, the Steam ticket is
// for Spacewar, not the real game, so validation fails.
//
// This plugin intercepts PlayFab's LoginWithSteam and redirects
// to LoginWithCustomID (anonymous) or LoginWithPlayFabID,
// allowing multiplayer to proceed without valid Steam auth.
//
// MinHook is statically linked.
// ============================================================
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "../../include/MinHook.h"
#include "../../include/uco_plugin.h"

#include "il2cpp_runtime.h"

static UCO_LogFn g_Log              = nullptr;
static volatile LONG g_bShutdown    = 0;
static HANDLE    g_hWatcherThread   = nullptr;

#define LOG(...) do { if (g_Log) g_Log(__VA_ARGS__); } while (0)

extern "C" void IL2CPP_Log(const char* fmt, ...)
{
    if (!g_Log) return;
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    _vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
    va_end(ap);
    g_Log("%s", buf);
}

static const char* GetIniPath()
{
    static char path[MAX_PATH] = {};
    static bool computed = false;
    if (computed) return path[0] ? path : nullptr;
    computed = true;
    char exeDir[MAX_PATH] = {};
    DWORD len = GetModuleFileNameA(nullptr, exeDir, MAX_PATH);
    if (len == 0) return nullptr;
    for (int i = (int)len - 1; i >= 0; --i) {
        if (exeDir[i] == '\\' || exeDir[i] == '/') { exeDir[i] = 0; break; }
    }
    int n = _snprintf_s(path, sizeof(path), _TRUNCATE, "%s\\union-crax.ini", exeDir);
    if (n <= 0) { path[0] = 0; return nullptr; }
    return path;
}

static void* g_pfnLoginWithSteam = nullptr;
static void* g_pfnLoginWithCustomID = nullptr;
static void* g_pfnGetCompletedTask = nullptr;

typedef void* (__fastcall *Fn_LoginWithCustomID)(void* pThis, void* customId);
static Fn_LoginWithCustomID g_pfnLoginWithCustomIDFn = nullptr;

typedef void* (__fastcall *Fn_LoginWithSteam)(void* pThis, void* request);
static Fn_LoginWithSteam g_pfnLoginWithSteamFn = nullptr;

static void* __fastcall Hooked_LoginWithSteam(void* pThis, void* request)
{
    LOG("[PlayFab] LoginWithSteam intercepted -> LoginWithCustomID (anonymous)");
    if (g_pfnLoginWithCustomIDFn)
    {
        return g_pfnLoginWithCustomIDFn(pThis, nullptr);
    }
    if (g_pfnGetCompletedTask)
    {
        LOG("[PlayFab] Falling back to Task.CompletedTask");
        typedef void* (__fastcall *Fn_GetCompleted)();
        return ((Fn_GetCompleted)g_pfnGetCompletedTask)();
    }
    return nullptr;
}

static bool TryInstall()
{
    if (!IL2CPP_IsReady()) return false;

    g_pfnLoginWithSteamFn = (Fn_LoginWithSteam)IL2CPP_FindMethodPtr(
        nullptr, "PlayFab.ClientModels", "PlayFabClientAPI", "LoginWithSteam", 1);
    if (!g_pfnLoginWithSteamFn)
    {
        LOG("[PlayFab] LoginWithSteam not found -- not a PlayFab game or method signature drifted");
        return false;
    }
    LOG("[PlayFab] LoginWithSteam found at %p", g_pfnLoginWithSteamFn);

    g_pfnLoginWithCustomIDFn = (Fn_LoginWithCustomID)IL2CPP_FindMethodPtr(
        nullptr, "PlayFab.ClientModels", "PlayFabClientAPI", "LoginWithCustomID", 1);
    if (!g_pfnLoginWithCustomIDFn)
    {
        LOG("[PlayFab] LoginWithCustomID not found -- cannot bypass");
        return false;
    }
    LOG("[PlayFab] LoginWithCustomID at %p", g_pfnLoginWithCustomIDFn);

    g_pfnGetCompletedTask = IL2CPP_FindMethodPtr(
        "mscorlib", "System.Threading.Tasks", "Task", "get_CompletedTask", 0);
    if (g_pfnGetCompletedTask)
        LOG("[PlayFab] Task.get_CompletedTask at %p", g_pfnGetCompletedTask);

    if (MH_CreateHook(g_pfnLoginWithSteamFn, (void*)&Hooked_LoginWithSteam, (void**)&g_pfnLoginWithSteam) != MH_OK ||
        MH_EnableHook(g_pfnLoginWithSteamFn) != MH_OK)
    {
        LOG("[PlayFab] failed to install LoginWithSteam hook");
        return false;
    }
    LOG("[PlayFab] LoginWithSteam hook installed at %p", g_pfnLoginWithSteamFn);

    LOG("[PlayFab] module active");
    return true;
}

static DWORD WINAPI WatcherProc(LPVOID)
{
    for (int i = 0; i < 600 && InterlockedCompareExchange(&g_bShutdown, 0, 0) == 0; ++i)
    {
        if (IL2CPP_TryInit())
        {
            if (TryInstall()) break;
        }
        Sleep(200);
    }
    if (!InterlockedCompareExchange(&g_bShutdown, 0, 0))
        LOG("[PlayFab] GameAssembly.dll never resolved or hooks failed -- giving up");
    return 0;
}

extern "C" __declspec(dllexport) int __cdecl UCO_PluginInit(const UCO_PluginContext* ctx)
{
    if (!ctx) return 1;
    if (ctx->ApiVersion != UCO_PLUGIN_API_VERSION) return 2;

    g_Log = ctx->Log;
    LOG("[PlayFab] plugin init: AppId=%u ogAppId=%u",
        ctx->ForcedAppId, ctx->OriginalAppId);

    if (MH_Initialize() != MH_OK)
        LOG("[PlayFab] MH_Initialize returned non-OK (already inited?)");

    g_hWatcherThread = CreateThread(nullptr, 0, WatcherProc, nullptr, 0, nullptr);
    return 0;
}

extern "C" __declspec(dllexport) void __cdecl UCO_PluginShutdown(void)
{
    InterlockedExchange(&g_bShutdown, 1);
    if (g_hWatcherThread)
    {
        WaitForSingleObject(g_hWatcherThread, 1000);
        CloseHandle(g_hWatcherThread);
        g_hWatcherThread = nullptr;
    }
    if (g_pfnLoginWithSteam)
        MH_DisableHook(g_pfnLoginWithSteamFn);
    LOG("[PlayFab] plugin shutdown");
}

BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) { return TRUE; }