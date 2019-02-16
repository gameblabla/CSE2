# Cave Story Engine 2 (Enhanced)

Cave Story Engine is an in-progress decompilation of Cave Story, ported from DirectX to SDL2.

This branch adds several enhancements such as PNG support, full alpha blending support, externalised resource files, Booster's Lab support, 60FPS, widescreen, and vastly-improved fullscreen.

## Dependencies

This project currently depends on SDL2 and Freetype2.

## Building

The project is currently built 'the Linux way':

Just run 'make' in the base directory, preferably with some of the following settings:

* RELEASE=1 to compile a release build (optimised, stripped, etc.)
* STATIC=1 to produce a statically-linked executable (good for Windows builds)
* JAPANESE=1 to enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* FIX_BUGS=1 to fix certain bugs (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* WINDOWS=1 to enable Windows-only features like a unique file/taskbar icon, and system font loading (needed for the font setting in Config.dat to do anything)

## Running

Because of the modifications made in this branch, a custom data folder is needed. These can be found in the res folder: one for Japanese builds, and one for English.

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
