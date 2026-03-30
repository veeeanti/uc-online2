# uc-online2

Custom modified Steam API library for Steam games to spoof your game as Spacewar. Drop-in replacement for `steam_api.dll` / `steam_api64.dll` (Windows), `libsteam_api.so` / `libsteam_api64.so` (Linux), or `libsteam_api.dylib` / `libsteam_api64.dylib` (macOS).

## Usage

__**If using downloaded libraries from [Releases](https://github.com/veeeanti/uc-online2/releases):**__
- 1. Extract the archive downloaded from __**LATEST**__ release.
- 2. Copy the corresponding library to replace your original.
   - **Windows:** Rename the original `.dll` before copying (e.g., `steam_api_o.dll` or `steam_api64_o.dll`).
   - **Linux:** Rename the original `.so` (e.g., `libsteam_api_o.so`).
   - **macOS:** Rename the original `.dylib` (e.g., `libsteam_api_o.dylib`).
- 3. Make sure Steam is running first. Then try running the game as you normally would.

__**If using self-built libraries:**__
- 1. Build using CMake (see below).
- 2. Copy the output library to your game folder:
   - **Windows:** `build/Release/steam_api.dll` or `build/Release/steam_api64.dll`
   - **Linux:** `build/x64/libsteam_api64.so`
   - **macOS:** `build/x64/libsteam_api64.dylib`

## Configuration

Create `union-crax.ini` next to the game executable to change your AppId as needed. If this file is missing, AppId defaults to `480` and plugins are not loaded.

```ini
[Settings]
AppId=480
PluginsFolder=plugins
```

## Plugin Loader / Injector

If `PluginsFolder` is set in the .ini file, all `.dll` (Windows), `.so` (Linux), or `.dylib` (macOS) files in that folder are loaded at startup in alphabetical order.

## Building

### With CMake (cross-platform)

```bash
# Linux / macOS
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Windows (Visual Studio)
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release

# Windows (alternative: MSBuild, still supported)
build.bat
```

### Requirements
- **CMake 3.16+**
- **C++17 compiler**
  - Windows: Visual Studio 2022 (v143 toolset) or MSVC Build Tools
  - Linux: GCC 9+ or Clang 10+
  - macOS: Xcode 12+ (Apple Clang)

### Output files

| Platform  | 32-bit               | 64-bit                  |
|-----------|----------------------|-------------------------|
| Windows   | `steam_api.dll`      | `steam_api64.dll`       |
| Linux     | `libsteam_api.so`    | `libsteam_api64.so`     |
| macOS     | `libsteam_api.dylib` | `libsteam_api64.dylib`  |

## Forking / Modifications

- Please feel free to fork this and modify it however you see fit, and if you feel it would benefit the original repo, make a PR and I'll look into it.
- You are allowed to modify this and distribute it to your liking, all I ask is that I'm made aware of this. Read the license for more info.

## Issues?

- No, this will not work with Denuvo protected games.
- As it is right now, DLC you don't own will likely not work.
- You cannot join VAC protected servers or servers hosted using the real AppId.
- For any other unexpected or unaccounted for issues, please contact me.
