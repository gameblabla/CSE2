[![Build Status](https://travis-ci.com/Clownacy/Cave-Story-Engine-2.svg?branch=accurate)](https://travis-ci.com/Clownacy/Cave-Story-Engine-2)

## Table of Contents

This repo has multiple branches:

Branch | Description
--------|--------
[accurate](https://www.github.com/Clownacy/Cave-Story-Engine-2/tree/accurate) | The main decompilation branch. The code intended to be as close to the original as possible, down to all the bugs and platform-dependencies.
[portable](https://www.github.com/Clownacy/Cave-Story-Engine-2/tree/portable) | This branch ports the engine to SDL2, and addresses numerous portability issues, allowing it to run on other platforms.
[enhanced](https://www.github.com/Clownacy/Cave-Story-Engine-2/tree/enhanced) | Based on the portable branch, this adds several enhancements to the engine, and makes it more accessible to modders.
[emscripten](https://www.github.com/Clownacy/Cave-Story-Engine-2/tree/emscripten) | Modifies the engine to build with Emscripten, [allowing it to run in web browsers](http://sonicresearch.org/clownacy/cave.html).
[wii](https://www.github.com/Clownacy/Cave-Story-Engine-2/tree/wii) | Ports the engine to the Nintendo Wii.

# Cave Story Engine 2

Cave Story Engine 2 is a decompilation of Cave Story.

![Screenshot](screenshot.png)

## Background

In 2007, a Linux port of Cave Story was made by Peter Mackay and Simon Parzer. Details about it can be found in [Peter's old blog](https://web.archive.org/web/20070911202919/http://aaiiee.wordpress.com:80/). This port received an update in 2011, including two shiny new executables. What Peter and Simon didn't realise was that they left huge amounts of debugging information in these executables, including the names of every C++ source file, and the variables and functions they contained.

This was a goldmine of information about not just the game's inner-workings, but its _source code._ This alone made a decompilation viable, but it wasn't the only help we'd get...

When Pixel made Cave Story, he compiled the original Windows EXE with no optimisations. This left the generated assembly code extremely verbose and easy to read. It also made the code very decompiler-friendly, since the assembly could be mapped directly back to the original C(++) code.

A combination of easy-to-decompile code and a near-complete symbol list made much of the decompilation process a copy/paste job, but not all of the game would need decompiling, as some of Cave Story's source code would actually see the light of day...

In early 2018, the Organya music engine was [released on GitHub](https://github.com/shbow/organya) by an old friend of Pixel's. On top of providing an insight into Pixel's coding style, this helped with figuring out one of the most complex parts of Cave Story's codebase.

It's because of these findings that a decompilation is possible: [the Mario 64 decompilation project](https://github.com/n64decomp/sm64) had a game that was built with no optimisations, [the Devilution project](https://github.com/diasurgical/devilution) was lucky enough to find a symbol list, so it's a miracle that we have both!

Many months of copypasting and tinkering later, here is the result.

## Building

### Visual Studio .NET 2003

Of course, project files for Visual Studio .NET 2003 are available, and can be found in the 'vs2003' folder.

As proven by the original `Doukutsu.exe`'s [Rich Header](http://bytepointer.com/articles/the_microsoft_rich_header.htm), Pixel used Visual Studio .NET 2003 to create Cave Story. This means these project files allow us to check the accuracy of the decompilation by comparing the generated assembly code to that of the original executable. The tool for this can be found in the 'devilution' folder.

### Visual Studio 2017 (and later)

Project files for Visual Studio 2017 can be found in the 'vs2017' folder.

### CMake (Visual Studio & MinGW-w64)

In this folder, create another folder called 'build', then switch to the command-line (Visual Studio users should open the [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs)) and `cd` into it. After that, generate the files for your build system with:

```
cmake .. -DCMAKE_BUILD_TYPE=Release
```

You can also add the following flags:

Name | Function
--------|--------
`-DLTO=ON` | Enable link-time optimisation
`-DMSVC_LINK_STATIC_RUNTIME=ON | Link the static MSVC runtime library
`-DJAPANESE=ON` | Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
`-DFIX_BUGS=ON` | Fix various bugs in the game
`-DDEBUG_SAVE=ON` | Re-enable the dummied-out 'Debug Save' option, and the ability to drag-and-drop save files onto the window

You can pass your own compiler flags with `-DCMAKE_C_FLAGS` and `-DCMAKE_CXX_FLAGS`.

You can then compile CSE2 with this command:

```
cmake --build . --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file instead.

Once built, the executable can be found in the `game_english`/`game_japanese` folder, depending on the selected language.

### Makefile (MinGW-w64)

Run 'make' in this folder, preferably with some of the following settings:

Name | Function
--------|--------
`RELEASE=1` | Compile a release build (optimised, stripped, etc.)
`STATIC=1` | Produce a statically-linked executable (so you don't need to bundle DLL files)
`JAPANESE=1` | Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
`FIX_BUGS=1` | Fix various bugs in the game
`DEBUG_SAVE=1` | Re-enable the dummied-out 'Debug Save' option, and the ability to drag-and-drop save files onto the window

You can pass your own compiler flags by defining `CXXFLAGS`.

Once built, the executable can be found in the `game_english`/`game_japanese` folder, depending on the selected language.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
