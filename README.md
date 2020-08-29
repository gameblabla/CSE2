# THIS BRANCH HAS BEEN DISCONTINUED

Emscripten's lack of mainloop support and its goofy build system make it a
nightmare to support. It will never be mainlined.

The original readme is below:

# CSE2

CSE2 is a decompilation of Cave Story, ported from DirectX to SDL2.

![Screenshot](screenshot.png)

This branch allows the game to be compiled with [Emscripten](http://emscripten.org/), enabling it to be played in a browser. This requires fairly extensive (and messy) modifications to the engine, to remove its dependency on "mainloops" - the infinite loops games put themselves in so they can run indefinitely. It's because of this that Emscripten support is limited to this branch: it's just so invasive.

## Dependencies

*Note: with CMake, if these are not found, they will be built locally*

* SDL2
* FreeType
* FLTK

## Building

### CMake

This project primarily uses CMake, allowing it to be built with a range of compilers.

In this folder, create another folder called 'build', then switch to the command-line (Visual Studio users should open the [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs)) and `cd` into it. After that, generate the files for your build system with:

```
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Emscripten users should do this instead:

```
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
```

You can also add the following flags:
* `-DJAPANESE=ON` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `-DFIX_BUGS=ON` - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `-DNONPORTABLE=ON` - Enable bits of code that aren't portable, but are what the original game used
* `-DFORCE_LOCAL_LIBS=ON` - Compile the built-in versions of SDL2, FreeType, and FLTK instead of using the system-provided ones

Then compile CSE2 with this command:

```
cmake --build . --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file instead.

Once built, the executables and assets can be found in the newly-generated `game` folder.

### Makefile (deprecated)

*Note: this requires pkg-config*

Run 'make' in this folder ('make -f Makefile.emscripten' for Emscripten builds), preferably with some of the following settings:

* `RELEASE=1` - Compile a release build (optimised, stripped, etc.)
* `STATIC=1` - Produce a statically-linked executable (good for Windows builds, so you don't need to bundle DLL files)
* `JAPANESE=1` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `FIX_BUGS=1` - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `WINDOWS=1` - Enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)
* `RASPBERRY_PI=1` - Enable tweaks to improve performance on Raspberry Pis
* `NONPORTABLE=1` - Enable bits of code that aren't portable, but are what the original game used

### Visual Studio .NET 2003

Project files for Visual Studio .NET 2003 are available in the 'msvc2003' folder.

Visual Studio .NET 2003 was used by Pixel to create the original `Doukutsu.exe`, so these project files allow us to check the accuracy of the decompilation by comparing the generated assembly code to that of the original executable.

This option is **strictly** for checking the decompilation's accuracy; CMake should be preferred in all other situations.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
