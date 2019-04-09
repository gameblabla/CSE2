# Cave Story Engine 2

Cave Story Engine 2 is a decompilation of Cave Story, ported from DirectX to SDL2.

![Screenshot](screenshot.png)

## Disclaimer

Cave Story Engine 2 is based off of the *original freeware release* by Studio Pixel, and is not based off of Nicalis' ports, and contains no extra features included in said ports (graphics, audio, and other changes).

## Dependencies

* SDL2
* FreeType
* pkg-config

## Building

The main way to build the project is 'the Linux way':

Just run 'make' in the base directory, preferably with some of the following settings:

* RELEASE=1 to compile a release build (optimised, stripped, etc.)
* STATIC=1 to produce a statically-linked executable (good for Windows builds, so you don't need to bundle DLL files)
* JAPANESE=1 to enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* FIX_BUGS=1 to fix certain bugs (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* WINDOWS=1 to enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)
* RASPBERRY_PI=1 to enable tweaks to improve performance on Raspberry Pis
* NONPORTABLE=1 to enable bits of code that aren't portable, but are what the original game used

Project files for Visual Studio 2003 are also available, in the 'msvc/msvc2003' directory (VS2003 was what Pixel used to compile the original EXE).

## Running

In order to run Cave Story Engine 2, you have to get the "data" folder from a vanilla copy of Cave Story. Just copy and paste it into the same folder as the executable.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
