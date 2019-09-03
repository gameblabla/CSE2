# Cave Story Engine 2

Cave Story Engine 2 is a decompilation of Cave Story.

![Screenshot](screenshot.png)

## Building

### Makefile

Run 'make' in this folder, preferably with some of the following settings:

* `RELEASE=1` - Compile a release build (optimised, stripped, etc.)
* `STATIC=1` - Produce a statically-linked executable (so you don't need to bundle DLL files)
* `JAPANESE=1` - Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
* `FIX_BUGS=1` - Fix bugs in the game (see [src/Bug Fixes.txt](src/Bug%20Fixes.txt))
* `DEBUG_SAVE=1` - Re-enable the dummied-out 'Debug Save' option, and the ability to drag-and-drop save files onto the window

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
