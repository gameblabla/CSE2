# Cave Story Engine 2

Cave Story Engine 2 is a decompilation of Cave Story, ported from DirectX to SDL2.

![Screenshot](screenshot.png)

## Disclaimer

Cave Story Engine 2 is based off of the *original freeware release* by Studio Pixel, and is not based off of Nicalis' ports, and contains no extra features included in said ports (graphics, audio, and other changes).

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
* `-DFIX_BUGS=On` - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))

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
* `FIX_BUGS=1` - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `WINDOWS=1` - Enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)
* `RASPBERRY_PI=1` - Enable tweaks to improve performance on Raspberry Pis
* `NONPORTABLE=1` - Enable bits of code that aren't portable, but are what the original game used

### Visual Studio .NET 2003

Project files for Visual Studio .NET 2003 are available in the 'msvc/msvc2003' directory.

Visual Studio .NET 2003 was used by Pixel to create the original `Doukutsu.exe`, so these project files allow us to check the accuracy of the decompilation by comparing the generated assembly code to that of the original executable.

This option is **strictly** for checking the decompilation's accuracy; CMake should be preferred in all other situations.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
