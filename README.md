# Cave Story Engine 2 (Enhanced)

Cave Story Engine 2 is a decompilation of Cave Story, ported from DirectX to SDL2.

![Screenshot](screenshot.png)

This branch adds several enhancements including:
* PNG support
* Full alpha blending support
* Fixed text blending, utilising the aforementioned alpha support
* Externalised resource files
* [Booster's Lab](https://github.com/taedixon/boosters-lab) support
* 60FPS
* Widescreen
* Vastly-improved fullscreen
* Removal of the design choice that locks sprites to a 320x240 grid when drawn (can easily be [re-enabled](src/Draw.cpp#L540) for stylistic purposes)
* Bugfixes enabled by default
* Support for Ogg Vorbis, FLAC, and Pxtone music formats

## Dependencies

* SDL2
* FreeType
* pkg-config
* FLTK

## Building

### CMake

This project primarily uses CMake, allowing it to be built with a range of compilers.

In this directory, create a directory called 'build', then switch to the command-line (Visual Studio users should open the Developer Command Prompt) and `cd` into it. After that, generate the files for your build system with:

```
cmake .. -DCMAKE_BUILD_TYPE=Release
```

You can also add the following flags:
* `-DJAPANESE=On` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `-DFIX_BUGS=On` - Enabled by default - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `-DEXTRA_SOUND_FORMATS=On` - Enabled by default - Add support for the Ogg Vorbis, FLAC, and Pxtone music/SFX formats

Then compile CSE2 with this command:

```
cmake --build . --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file instead.

Once built, the executables can be found in `build_en` or `build_jp`, depending on the selected language.

### Makefile (deprecated)

Run 'make' in the base directory, preferably with some of the following settings:

* `RELEASE=1` - Compile a release build (optimised, stripped, etc.)
* `STATIC=1` - Produce a statically-linked executable (good for Windows builds, so you don't need to bundle DLL files)
* `JAPANESE=1` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `FIX_BUGS=1` - Enabled by default - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `WINDOWS=1` - Enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)
* `RASPBERRY_PI=1` - Enable tweaks to improve performance on Raspberry Pis
* `NONPORTABLE=1` - Enable bits of code that aren't portable, but are what the original game used
* `EXTRA_SOUND_FORMATS=1` - Enabled by default - Add support for the Ogg Vorbis, FLAC, and Pxtone music/SFX formats

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
