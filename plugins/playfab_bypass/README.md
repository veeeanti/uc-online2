# playfab_bypass plugin

**Bypasses PlayFab authentication for Unity games using PlayFab services.**

Many Unity games use PlayFab for multiplayer authentication, often with Steam integration. The typical flow is `PlayFabClientAPI.LoginWithSteam(steamTicket)` which validates the Steam ticket against PlayFab's backend. With UCOnline2 spoofing AppId = 480 (Spacewar), the Steam ticket is for Spacewar, not the real game, so validation fails.

This plugin intercepts `LoginWithSteam` and redirects to `LoginWithCustomID` (anonymous), allowing multiplayer to proceed without valid Steam auth.

## Confirmed working

| Game | Notes |
|---|---|
| Games using PlayFab SDK with Steam authentication | Tested with IL2CPP backend |

## Setup

1. Build and copy `playfab_bypass.dll` into the game's plugins folder:

```powershell
Copy-Item plugins\playfab_bypass\relbuild\x64\playfab_bypass.dll `
  C:\path\to\TheGame\plugins\ -Force
```

Create the `plugins\` folder if it doesn't exist. Also ensure UCOnline2's `steam_api64.dll` is in place.

2. Configure `union-crax.ini`:

```ini
[Settings]
AppId=480
ogAppId=<the game's real Steam AppId>
PluginsFolder=plugins
```

## How it works

The plugin auto-detects PlayFab usage by scanning for `PlayFabClientAPI.LoginWithSteam` in the IL2CPP metadata. When found, it hooks the method and redirects authentication to `LoginWithCustomID` with an anonymous identifier, bypassing the Steam ticket validation entirely.

## Limitations

- The hook is installed once GameAssembly.dll loads and IL2CPP is ready
- Method signatures may drift between PlayFab SDK versions; if you see "LoginWithSteam not found" in logs, the signature may have changed
- Does not handle `LoginWithPlayFabID` or other auth methods (can be extended if needed)

## Build

```powershell
msbuild plugins\playfab_bypass\playfab_bypass_plugin.vcxproj `
  -p:Configuration=Release -p:Platform=x64 -m
```

Output: `plugins\playfab_bypass/relbuild/x64/playfab_bypass.dll`. MinHook is statically linked.