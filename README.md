# uc-online2

Custom modified Steam API .dll for Steam games to spoof your game as Spacewar. Drop-in replacement for `steam_api.dll` / `steam_api64.dll`.

## Usage

__**If using downloaded .dlls from [Releases](https://github.com/veeeanti/uc-online2/releases):**__
- 1. Extract the archive downloaded from __**LATEST**__ release (should be v1.0.1 as of currently writing this).
- 2. Copy the corresponding .dll to replace your original .dll.
   - 2a. Rename the original .dll before copying it to something else if you feel you must back it up, something like ``steam_api_o.dll`` as Goldberg Emu suggests or ``steam_api64.dll.old``. (It doesn't matter as long as it is just changed.)
- 3. Make sure Steam is running first. Then try running the game as you normally would from the .exe. If it has SteamStub, use Steamless to remove it or use the .dll made to bypass it for games that Steamless cannot unpack. (Dave the Diver is an example.)
   - 3a. If it throws an error related to auth failure, restart Steam and try again. If the error persists, contact me. I'll work with you to figure it out one way or another. 

__**If using self built .dlls:**__
- 1. Run `build.bat` or open `uc_online2.vcxproj` in Visual Studio.
- 2. Copy the output .dll to your game folder:
   - **32-bit:** `build\x86\steam_api.dll`
   - **64-bit:** `build\x64\steam_api64.dll`
- 3. Replace your `steam_api(64).dll` with one from here. Back it up if necessary by renaming it to `steam_api(64)_o.dll`.

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

__**Quick way (true Chad way - quick, simple, and easy):**__
- 1. Run `build.bat`.
- 2. ???
- 3. Profit.

__**With Visual Studio (the bum way - requires too much effort):**__
- 1. Open `uc_online2.vcxproj`.
- 2. Select Release | Win32 or Release | x64.
- 3. Build.

Requires Visual Studio 2022 with C / C++ Environment selected (v143 or higher toolset). If MSBuild is not found, `build.bat` will tell you where to get it. The `build.bat` script has a preset path that applies to my personal setup and if it doesn't match yours, you will need to modify it. Otherwise, you'll consistently error out even when you have everything installed. 

## Forking / Modifications 

Okay, so this part I did not cover as of publishing the source files, this will cover personal modifications and forks as well as modifications to this.

- Please feel free to fork this and modify it however you see fit, and if you feel it would benefit the original repo, make a PR and I'll look into it.
- You are allowed to modify this and distribute it to your liking, all I ask is that I'm made aware of this. Read the license for more info. 
   - I only want this so I can be certain that it's not being distributed as a method to spread malware. (I should make the license have something for this, or make it better...)
- Have any ideas but you're not sure how to implement it? Or don't know how to code? Contact me with your ideas. I'll work on it for you and communicate with you throughout the process. 

## Issues?

- No, this will not work with Denuvo protected games. If you think it can, modify it so that it can work like an activated game, but even then I cannot guarantee it will work. It will likely reject you and you will need to get re-activated as your token will be fucked permanently. So basically, __I say just don't even bother. It'll likely waste your time and the activators' time too.__
- As it is right now, DLC you don't own will likely not work - I'll try and add functionality for that in and if it works, then it'll likely work the same as Goldberg does.
- If you're trying this with a game that has the AppId hard coded in (like with Godot games) then you'll need to modify the game to set the AppId to what you need it to be. Though, you won't even need this at all if you do that lol. 
- You cannot join VAC protected servers or servers hosted using the real AppId in Garry's Mod or other Source games or any other games that have similar protections. (GoldSrc games seemingly do not apply, as CS1.6 let me join any servers.) Please do not message me asking why you can't join any servers in Garry's Mod. Instead, ask me how you can play with your friends if they have legitimate copies. :)
- For any other unexpected or unaccounted for issues, please contact me. I have yet to test this with every game so I will rely on the community to do so. 
