[![Build Status](https://travis-ci.com/Clownacy/CSE2.svg?branch=portable)](https://travis-ci.com/Clownacy/CSE2)

## Table of Contents

This repo has two main branches:

Branch | Description
--------|--------
[accurate](https://www.github.com/Clownacy/CSE2/tree/accurate) | The main decompilation branch. The code is intended to be as close to the original as possible, down to all the bugs and platform-dependencies.
[portable](https://www.github.com/Clownacy/CSE2/tree/portable) | This branch ports the engine away from WinAPI and DirectX, and addresses numerous portability issues, allowing it to run on other platforms.

# CSE2 (Portable)

CSE2 is a decompilation of Cave Story.

This branch migrates the engine away from WinAPI and DirectX, and addresses
numerous portability issues, allowing it to run on other platforms.

Supported platforms include...
* Windows
* Linux
* macOS
* Wii U
* 3DS
* RISC OS

![Screenshot](screenshot.png)

## Background

When Pixel made Cave Story, he compiled the original Windows EXE with no
optimisations. This left the generated assembly code extremely verbose and easy
to read. It also made the code very decompiler-friendly, since the assembly
could be mapped directly back to the original C++ code.

Technically, this alone made a decompilation feasible, as was the case for [the
Super Mario 64 decompilation project](https://github.com/n64decomp/sm64) -
however, there was more to be found...

In 2007, a Linux port of Cave Story was made by Simon Parzer and Peter Mackay.
Details about it can be found on [Peter's old blog](https://web.archive.org/web/20070911202919/http://aaiiee.wordpress.com:80/).
This port received an update in 2011, including two shiny new executables. What
they didn't realise was that they left huge amounts of debugging information in
these executables, including the names of every C++ source file, as well as the
variables, functions, and structs they contained.

This was a goldmine of information about not just the game's inner-workings, but
its _source code._ This is the same lucky-break [the Diablo decompilation project](https://github.com/diasurgical/devilution)
had. With it, much of the game's code was pre-documented and explained, saving
us the effort of doing it ourselves. In fact, the combination of
easy-to-decompile code, and a near-full set of function/variable names, reduced
much of the decompilation process to mere copy-paste.

To top it all off, some of Cave Story's original source code would eventually
see the light of day:

In early 2018, the Organya music engine was [released on GitHub](https://github.com/shbow/organya)
by an old friend of Pixel's. On top of providing an insight into Pixel's coding
style, this helped with figuring out one of the most complex parts of Cave
Story's codebase.

And... that's it! It's not often that a game this decompilable comes along, so
I'm glad that Cave Story was one of them. [Patching a dusty old executable from 2005 sucks](https://github.com/Clownacy/Cave-Story-Mod-Loader/blob/master/src/mods/graphics_enhancement/widescreen/patch_camera.c).

## Dependencies

* SDL2 (if `BACKEND_AUDIO` or `BACKEND_PLATFORM` are set to `SDL2`)
* GLFW3 (if `BACKEND_PLATFORM` is set to `GLFW3`)
* FreeType (if `FREETYPE_FONTS` is enabled)

If these are not found, they will be built locally.

In addition, `pkg-config` is required for builds that require static-linkage.

A list of dependencies for specific platforms can be found [on the wiki](https://github.com/Clownacy/CSE2/wiki/Dependency-lists).

## Building

This project uses CMake, allowing it to be built with a range of compilers.

Switch to the terminal (Visual Studio users should open the [Developer Command Prompt](https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs))
and `cd` into this folder. After that, generate the files for your build system
with:

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

MSYS2 users should append `-G"MSYS Makefiles" -DPKG_CONFIG_STATIC_LIBS=ON` to
this command, also.

You can also add the following flags:

Name | Function
--------|--------
`-DJAPANESE=ON` | Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
`-DFIX_BUGS=ON` | Fix various bugs in the game
`-DDEBUG_SAVE=ON` | Re-enable the ability to drag-and-drop save files onto the window
`-DDOCONFIG=OFF` | Disable compiling the DoConfig tool (it is not useful for console ports)
`-DDOCONFIG_LEGACY_OPENGL=ON` | Make DoConfig use OpenGL 2.1 instead of OpenGL 3.2 (useful for older/limited platforms)
`-DLANCZOS_RESAMPLER=ON` | Use Lanczos filtering for audio resampling instead of linear-interpolation (Lanczos is more performance-intensive, but higher quality)
`-DFREETYPE_FONTS=ON` | Use FreeType2 to render the DejaVu Mono (English) or Migu1M (Japanese) fonts, instead of using pre-rendered copies of Courier New (English) and MS Gothic (Japanese)
`-DBACKEND_RENDERER=OpenGL3` | Render with OpenGL 3.2 (hardware-accelerated)
`-DBACKEND_RENDERER=OpenGLES2` | Render with OpenGL ES 2.0 (hardware-accelerated)
`-DBACKEND_RENDERER=SDLTexture` | (Default) Render with SDL2's Texture API (hardware-accelerated) (note: requires `-DBACKEND_PLATFORM=SDL2`)
`-DBACKEND_RENDERER=SDLSurface` | Render with SDL2's Surface API (software-rendered) (note: requires `-DBACKEND_PLATFORM=SDL2`)
`-DBACKEND_RENDERER=WiiU` | Render with the Wii U's GX2 API (hardware-accelerated)
`-DBACKEND_RENDERER=3DS` | Render with the 3DS's Citro2D/Citro3D API (hardware-accelerated)
`-DBACKEND_RENDERER=Software` | Render with a handwritten software-renderer
`-DBACKEND_AUDIO=SDL2` | (Default) Deliver audio with SDL2 (software-mixer)
`-DBACKEND_AUDIO=miniaudio` | Deliver audio with miniaudio (software-mixer)
`-DBACKEND_AUDIO=WiiU-Hardware` | Deliver audio with Wii U's AXVoice API (hardware-accelerated) (WARNING - currently broken: voices randomly disappear for unknown reasons)
`-DBACKEND_AUDIO=WiiU-Software` | Deliver audio with Wii U's AXVoice API (software-mixer)
`-DBACKEND_AUDIO=3DS-Hardware` | Deliver audio with 3DS's NDSP API (hardware-accelerated)
`-DBACKEND_AUDIO=3DS-Software` | Deliver audio with 3DS's NDSP API (software-mixer)
`-DBACKEND_AUDIO=Null` | Don't deliver audio at all (WARNING - game will have no audio)
`-DBACKEND_PLATFORM=SDL2` | (Default) Use SDL2 for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=GLFW3` | Use GLFW3 for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=WiiU` | Use the Wii U's native APIs for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=3DS` | Use the 3DS's native APIs for miscellaneous platform-dependant operations
`-DBACKEND_PLATFORM=Null` | Don't do platform-dependant operations at all (WARNING - game will have no video or input)
`-DLTO=ON` | Enable link-time optimisation
`-DPKG_CONFIG_STATIC_LIBS=ON` | On platforms with pkg-config, static-link the dependencies (good for Windows builds, so you don't need to bundle DLL files)
`-DMSVC_LINK_STATIC_RUNTIME=ON` | Link the static MSVC runtime library, to reduce the number of required DLL files (Visual Studio only)
`-DFORCE_LOCAL_LIBS=ON` | Compile the built-in versions of SDL2, GLFW3, and FreeType instead of using the system-provided ones

You can pass your own compiler flags with `-DCMAKE_C_FLAGS` and `-DCMAKE_CXX_FLAGS`.

You can then compile CSE2 with this command:

```
cmake --build build --config Release
```

If you're a Visual Studio user, you can open the generated `CSE2.sln` file
instead, which can be found in the `build` folder.

Once built, the executables can be found in the `game_english`/`game_japanese`
folder, depending on the selected language.

### Building for the Wii U

To target the Wii U, you'll need devkitPro, devkitPPC, and WUT.

First, add the devkitPPC tools directory to your PATH (because WUT's CMake
support is broken, as of writing):

```
PATH=$PATH:$DEVKITPPC/bin
```

Then, generate the build files with this command:

```
cmake -B buildwiiu -DCMAKE_BUILD_TYPE=Release -DFORCE_LOCAL_LIBS=ON -DBACKEND_PLATFORM=WiiU -DBACKEND_RENDERER=WiiU -DBACKEND_AUDIO=WiiU-Software -DDOCONFIG=OFF -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/wut/share/wut.toolchain.cmake
```

Finally, build the game with this command:

```
cmake --build buildwiiu
```

This will build a binary, but you still need to convert it to an `.rpx` file
that can be ran on your Wii U.

First, we need to strip the binary:

```
powerpc-eabi-strip -g game_english/CSE2
```

Then, we convert it to an `.rpx`:
```
elf2rpl game_english/CSE2 game_english/CSE2.rpx
```

`game_english/CSE2.rpx` is now ready to be ran on your Wii U. This port expects
the data folder to be in a folder called `CSE2-portable-en`/`CSE2-portable-jp`
on the root of your SD card.

### Building for the 3DS

To target the 3DS, you'll need devkitPro, devkitARM, Citro2D, Citro3D, libctru,
and bannertool, along with the `3dstools` and `devkitpro-pkgbuild-helpers`
packages.

Open a terminal, and `cd` into the CSE2 directory. Then execute this command:

```
cmake -B build3ds -DCMAKE_BUILD_TYPE=Release -DFORCE_LOCAL_LIBS=ON -DBACKEND_PLATFORM=3DS -DBACKEND_RENDERER=3DS -DBACKEND_AUDIO=3DS-Hardware -DDOCONFIG=OFF -DFREETYPE_FONTS=ON -DCMAKE_TOOLCHAIN_FILE=$DEVKITPRO/cmake/3DS.cmake -DCTRU_ROOT=$DEVKITPRO/libctru
```

(Note that `FREETYPE_FONTS` is enabled. If you're creating a Japanese build,
it's best to disable this, as the FreeType font is unreadable at 320x240).

This will create the build files. To build CSE2, run:

```
cmake --build build3ds
```

This will create an elf file. Before we can create a `.3dsx` file from it, we
need to make an `.smdh` file:

```
bannertool makesmdh -i $DEVKITPRO/libctru/default_icon.png -s "CSE2" -l "Port of Cave Story" -p "Clownacy" -o build3ds/smdh.smdh
```

We can finally generate a `.3dsx` file:

```
3dsxtool game_english/CSE2.elf game_english/CSE2.3dsx --romfs=game_english/data --smdh=build3ds/smdh.smdh
```

## Licensing

Being a decompilation, the majority of the code in this project is proprietary
and belongs to Daisuke "Pixel" Amaya.

Modifications and custom code are made available under the MIT licence. See
`LICENCE.txt` for details.
