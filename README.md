# uc-online2

Custom modified Steam API .dll for Steam games to spoof your game as Spacewar. Drop-in replacement for `steam_api.dll` / `steam_api64.dll`.

## Usage

1. Run `build.bat` or open `uc_online2.vcxproj` in Visual Studio.
2. Copy the output .dll to your game folder:
   - **32-bit:** `build\x86\steam_api.dll`
   - **64-bit:** `build\x64\steam_api64.dll`
3. Replace your `steam_api(64).dll` with one from here. Back it up if necessary by renaming it to `steam_api(64)_o.dll`.

## Configuration

Create `union-crax.ini` next to the game executable to change your AppId as needed. If this file is missing, AppId defaults to `480` and plugins are not loaded. `PluginsFolder` is relative to the game executable or wherever it's set in the .ini. Or should be. I haven't tested it yet. Check the `steam_appid.txt` file that gets created upon running the game to check if your set AppId was accepted.

```ini
[Settings]
AppId=480
PluginsFolder=plugins
```

## Plugin Loader / Injector

If `PluginsFolder` is set in the .ini file, all `.dll` files in that folder are loaded at startup in alphabetical order. Use prefixes to control load order:

```
plugins/
  01_first_plugin.dll
  02_second_plugin.dll
  03_another_one_(dj_khaled!!).dll
```

## Building

**Quick way (true Chad way - quick, simple, and easy):**
1. Run `build.bat`.
2. ???
3. Profit.

**With Visual Studio (the bum way - requires too much effort):**
1. Open `uc_online2.vcxproj`.
2. Select Release | Win32 or Release | x64.
3. Build.

Requires Visual Studio 2022 (v143 toolset). If MSBuild is not found, `build.bat` will tell you where to get it.

