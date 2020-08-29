# THIS BRANCH HAS BEEN DISCONTINUED

I don't want to work on an enhanced Cave Story anymore, or at least not one
where I have to cater to such a massive range of people with different
expectations.

You might be interested in [Cave Story: Tweaked](https://github.com/calvarado194/CSE2-tweaks)
instead.

The original readme is below:

[![Build Status](https://travis-ci.com/Clownacy/CSE2.svg?branch=enhanced)](https://travis-ci.com/Clownacy/CSE2)

## Table of Contents

This repo has multiple branches:

Branch | Description
--------|--------
[accurate](https://www.github.com/Clownacy/CSE2/tree/accurate) | The main decompilation branch. The code intended to be as close to the original as possible, down to all the bugs and platform-dependencies.
[portable](https://www.github.com/Clownacy/CSE2/tree/portable) | This branch ports the engine away from WinAPI and DirectX, and addresses numerous portability issues, allowing it to run on other platforms.
[enhanced-lite](https://www.github.com/Clownacy/CSE2/tree/enhanced-lite) | Based on the `portable` branch, this branch is **intended for modders**, and adds several enhancements and features to the engine.
[enhanced](https://www.github.com/Clownacy/CSE2/tree/enhanced) | Based on the `enhanced-lite` branch, this branch is **intended for players**, and adds further enhancements and features to the engine.
[emscripten](https://www.github.com/Clownacy/CSE2/tree/emscripten) | Modifies the engine to build with Emscripten, [allowing it to run in web browsers](http://sonicresearch.org/clownacy/cave.html) (no longer maintained).
[wii](https://www.github.com/Clownacy/CSE2/tree/wii) | Ports the engine to the Nintendo Wii (no longer maintained).

# CSE2 (Enhanced)

CSE2 is a decompilation of Cave Story.

This branch builds upon the `enhanced-lite` branch, adding extra enhancements including:
* Support for alternate soundtracks

This is in addition to the enhancements inherited from the `enhanced-lite` branch:
* Widescreen
* Options menu:
  * Control remapping (keyboard and gamepad)
  * Soundtrack selection
  * V-sync toggle
  * 50FPS/60FPS toggle
  * Option to disable the design choice that locks sprites to a 320x240 grid when drawn, making them move smoother
* Bugfixes

Supported platforms include...
* Windows
* Linux
* macOS
* Wii U

![Screenshot](screenshot.png)

## Background

When Pixel made Cave Story, he compiled the original Windows EXE with no optimisations. This left the generated assembly code extremely verbose and easy to read. It also made the code very decompiler-friendly, since the assembly could be mapped directly back to the original C(++) code.

Technically, this alone made a decompilation feasible, as was the case for [the Super Mario 64 decompilation project](https://github.com/n64decomp/sm64) - however, there was more to be found...

In 2007, a Linux port of Cave Story was made by Peter Mackay and Simon Parzer. Details about it can be found on [Peter's old blog](https://web.archive.org/web/20070911202919/http://aaiiee.wordpress.com:80/). This port received an update in 2011, including two shiny new executables. What Peter and Simon didn't realise was that they left huge amounts of debugging information in these executables, including the names of every C++ source file, as well as the variables, functions, and structs they contained.

This was a goldmine of information about not just the game's inner-workings, but its _source code._ This is the same lucky-break [the Diablo decompilation project](https://github.com/diasurgical/devilution) had. With it, much of the game's code was pre-documented and explained _for_ us, saving us the effort of doing it ourselves. In fact, the combination of easy-to-decompile code, and a near-full set of function/variable names, reduced much of the decompilation process to mere copy-paste.

To top it all off, some of Cave Story's original source code would eventually see the light of day...

In early 2018, the Organya music engine was [released on GitHub](https://github.com/shbow/organya) by an old friend of Pixel's. On top of providing an insight into Pixel's coding style, this helped with figuring out one of the most complex parts of Cave Story's codebase.

And... that's it! It's not often that a game this decompilable comes along, so I'm glad that Cave Story was one of them. [Patching a dusty old executable from 2004 has its downsides](https://github.com/Clownacy/Cave-Story-Mod-Loader/blob/master/src/mods/graphics_enhancement/widescreen/patch_camera.c).

## Dependencies

* SDL2 (if `BACKEND_AUDIO` or `BACKEND_PLATFORM` are set to `SDL2`)
* GLFW3 (if `BACKEND_PLATFORM` is set to `GLFW3`)
* FreeType

If these are not found, they will be built locally.

In addition, `pkg-config` is required for builds that require static-linkage.

A list of dependencies for specific platforms can be found [on the wiki](https://github.com/Clownacy/Cave-Story-Engine-2/wiki/Dependency-lists).

## Building

This project uses CMake, allowing it to be built with a range of compilers.

Switch to the terminal (Visual Studio users should open the [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs)) and `cd` into this folder. After that, generate the files for your build system with:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

MSYS2 users should append `-G"MSYS Makefiles" -DPKG_CONFIG_STATIC_LIBS=ON` to this command, also.

You can also add the following flags:

Name | Function
--------|--------
`-DJAPANESE=ON` | Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
`-DFIX_BUGS=ON` | Enabled by default - Fix various bugs in the game
`-DDEBUG_SAVE=ON` | Re-enable the ability to drag-and-drop save files onto the window
`-DBACKEND_RENDERER=OpenGL3` | Render with OpenGL 3.2 (hardware-accelerated)
`-DBACKEND_RENDERER=OpenGLES2` | Render with OpenGL ES 2.0 (hardware-accelerated)
`-DBACKEND_RENDERER=SDLTexture` | (Default) Render with SDL2's Texture API (hardware-accelerated) (note: requires `-DBACKEND_PLATFORM=SDL2`)
`-DBACKEND_RENDERER=WiiU` | Render with the Wii U's GX2 API (hardware-accelerated)
`-DBACKEND_RENDERER=Software` | Render with a handwritten software-renderer
`-DBACKEND_AUDIO=SDL2` | (Default) Deliver audio with SDL2 (software-mixer)
`-DBACKEND_AUDIO=miniaudio` | Deliver audio with miniaudio (software-mixer)
`-DBACKEND_AUDIO=WiiU-Hardware` | Deliver audio with Wii U's AXVoice API (hardware-accelerated) (WARNING - currently broken: voices randomly disappear for unknown reasons)
`-DBACKEND_AUDIO=WiiU-Software` | Deliver audio with Wii U's AXVoice API (software-mixer)
`-DBACKEND_AUDIO=Null` | Don't deliver audio at all (WARNING - game will have no audio)
`-DBACKEND_PLATFORM=SDL2` | (Default) Use SDL2 for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=GLFW3` | Use GLFW3 for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=WiiU` | Use the Wii U's native APIs for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=Null` | Don't do platform-dependant operations at all (WARNING - game will have no video or input)
`-DLTO=ON` | Enable link-time optimisation
`-DPKG_CONFIG_STATIC_LIBS=ON` | On platforms with pkg-config, static-link the dependencies (good for Windows builds, so you don't need to bundle DLL files)
`-DMSVC_LINK_STATIC_RUNTIME=ON` | Link the static MSVC runtime library, to reduce the number of required DLL files (Visual Studio only)
`-DFORCE_LOCAL_LIBS=ON` | Compile the built-in versions of SDL2, GLFW3, and FreeType instead of using the system-provided ones
`-DEXTRA_SOUND_FORMATS=ON` | Enable support for alternate music/SFX formats, include Ogg Vorbis, FLAC, and PxTone (not to be confused with PixTone)
`-DCLOWNAUDIO_STB_VORBIS=ON` | Enable support for Ogg Vorbis music/SFX
`-DCLOWNAUDIO_DR_FLAC=ON` | Enable support for FLAC music/SFX
`-DCLOWNAUDIO_LIBXMPLITE=ON` | Enable support for .it, .xm, .mod, .s3m music/SFX
`-DCLOWNAUDIO_PXTONE=ON` | Enable support for PxTone music/SFX (not to be confused with PixTone)

You can pass your own compiler flags with `-DCMAKE_C_FLAGS` and `-DCMAKE_CXX_FLAGS`.

You can then compile CSE2 with this command:

```
cmake --build build --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file instead, which can be found in the `build` folder.

Once built, the executables can be found in the `game_english`/`game_japanese` folder, depending on the selected language.

### Building for the Wii U

To target the Wii U, you'll need devkitPro, and WUT.

First, add the devkitPPC tools directory to your PATH (because WUT's CMake support is broken, as of writing):

```
PATH=$PATH:$DEVKITPPC/bin
```

Then, generate the build files with this command:

```
cmake -B buildwiiu -DCMAKE_BUILD_TYPE=Release -DFORCE_LOCAL_LIBS=ON -DBACKEND_PLATFORM=WiiU -DBACKEND_RENDERER=WiiU -DBACKEND_AUDIO=WiiU-Software -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/wut/share/wut.toolchain.cmake
```

Finally, build the game with this command:

```
cmake --build buildwiiu
```

This will build a binary, but you still need to convert it to an `.rpx` file that can be ran on your Wii U.

First, we need to strip the binary:

```
powerpc-eabi-strip -g game_english/CSE2
```

Then, we convert it to an `.rpx`:
```
elf2rpl game_english/CSE2 game_english/CSE2.rpx
```

`game_english/CSE2.rpx` is now ready to be ran on your Wii U. This port expects the data folder to be in a folder called `CSE2-enhanced-en`/`CSE2-enhanced-jp` on the root of your SD card.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a licence for our own code.
