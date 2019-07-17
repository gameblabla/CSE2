# Cave Story Engine 2 (Enhanced)

Cave Story Engine 2 is a decompilation of Cave Story, ported from DirectX to SDL2.

![Screenshot](screenshot.png)

This branch adds several enhancements including:
* PNG support
* Full alpha blending support
* Fixed text blending, utilising the aforementioned alpha support
* All resource files moved to the data folder
* [Booster's Lab](https://github.com/taedixon/boosters-lab) support (open 'data/mrmap.bin')
* 60FPS
* Widescreen
* Vastly-improved fullscreen
* Automatic vsync (only enabled on 60hz monitors)
* Smooth sprite movement, through the removal of the design choice that locks sprites to a 320x240 grid when drawn (can be re-enabled by disabling SMOOTH_SPRITE_MOVEMENT)
* Bugfixes enabled by default
* Pixtone sounds split to .pxt files
* Support for Ogg Vorbis, FLAC, .it, .xm, .mod, .s3m, and Pxtone audio formats for music and SFX
* Support for high-resolution sprites (see SPRITE_SCALE in CommonDefines.h)

## Dependencies

*Note: with CMake, if these are not found, they will be built locally*

* SDL2
* FreeType
* FLTK
* libxmp-lite (only if TRACKER_AUDIO is enabled)

## Building

### CMake

This project primarily uses CMake, allowing it to be built with a range of compilers.

In this folder, create another folder called 'build', then switch to the command-line (Visual Studio users should open the [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs)) and `cd` into it. After that, generate the files for your build system with:

```
cmake .. -DCMAKE_BUILD_TYPE=Release
```

You can also add the following flags:
* `-DJAPANESE=ON` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `-DFIX_BUGS=ON` - Enabled by default - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `-DNONPORTABLE=ON` - Enable bits of code that aren't portable, but are what the original game used
* `-DFORCE_LOCAL_LIBS=ON` - Compile the built-in versions of SDL2, FreeType, and FLTK instead of using the system-provided ones
* `-DRENDERER=Texture` - Use the hardware-accelerated SDL2 Texture API renderer (default)
* `-DRENDERER=Surface` - Use the software-rendered SDL2 Surface API renderer
* `-DRENDERER=Software` - Use a handwritten software renderer
* `-DOGG_AUDIO=ON` - Enable support for Ogg Vorbis music/SFX
* `-DFLAC_AUDIO=ON` - Enable support for FLAC music/SFX
* `-DTRACKER_AUDIO=ON` - Enable support for .it, .xm, .mod, .s3m music/SFX
* `-DPXTONE_AUDIO=ON` - Enable support for PxTone music/SFX (not to be confused with PixTone)
* `-DSMOOTH_SPRITE_MOVEMENT=ON` - Enabled by default - Removes the grid that all sprites are locked to, allowing them to move smoothly at higher resolutions

Then compile CSE2 with this command:

```
cmake --build . --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file instead.

Once built, the executables and assets can be found in the newly-generated `game` folder.

### Makefile (deprecated)

*Note: this requires pkg-config*

Run 'make' in this folder, preferably with some of the following settings:

* `RELEASE=1` - Compile a release build (optimised, stripped, etc.)
* `STATIC=1` - Produce a statically-linked executable (good for Windows builds, so you don't need to bundle DLL files)
* `JAPANESE=1` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `FIX_BUGS=1` - Enabled by default - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `WINDOWS=1` - Enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)
* `RASPBERRY_PI=1` - Enable tweaks to improve performance on Raspberry Pis
* `NONPORTABLE=1` - Enable bits of code that aren't portable, but are what the original game used
* `RENDERER=Texture` - Use the hardware-accelerated SDL2 Texture API renderer (default)
* `RENDERER=Surface` - Use the software-rendered SDL2 Surface API renderer
* `RENDERER=Software` - Use a hand-written software renderer
* `OGG_AUDIO=1` - Enable support for Ogg Vorbis music/SFX
* `FLAC_AUDIO=1` - Enable support for FLAC music/SFX
* `TRACKER_AUDIO=1` - Enable support for .it, .xm, .mod, .s3m music/SFX
* `PXTONE_AUDIO=1` - Enable support for PxTone music/SFX (not to be confused with PixTone)
* `SMOOTH_SPRITE_MOVEMENT=1` - Enabled by default - Removes the grid that all sprites are locked to, allowing them to move smoothly at higher resolutions

### Visual Studio .NET 2003

Project files for Visual Studio .NET 2003 are available in the 'msvc2003' folder.

Visual Studio .NET 2003 was used by Pixel to create the original `Doukutsu.exe`, so these project files allow us to check the accuracy of the decompilation by comparing the generated assembly code to that of the original executable.

This option is **strictly** for checking the decompilation's accuracy; CMake should be preferred in all other situations.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
