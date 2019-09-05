# Cave Story Engine 2

Cave Story Engine 2 is a decompilation of Cave Story.

![Screenshot](screenshot.png)

## Background

In 2007, a Linux port of Cave Story was made by Peter Mackey and Simon Parzer. Details about it can be found in [Peter's old blog](https://web.archive.org/web/20070911202919/http://aaiiee.wordpress.com:80/). This port received an update in 2011, including two shiny new executables. What Peter and Simon didn't realise was that they left huge amounts of debugging information in these executables, including the names of every C++ source file, and the variables and functions they contained.

Another important element was that, when Pixel made Cave Story, he compiled it with no optimisations. This left the generated assembly code extremely verbose and easy to read. It also made the code very decompiler-friendly. Additionally, the EXE's [Rich Header](http://bytepointer.com/articles/the_microsoft_rich_header.htm) told us exactly what compiler was used: Visual C++ .NET 2003.

The last thing of note was that some of Cave Story's source code actually saw a release: the game's Organya sound engine was [released on GitHub](https://github.com/shbow/organya) in early 2018. This provided an insight into Pixel's coding style, and made decompiling Organya much easier.

Together, these findings are a goldmine of information on the game's inner-workings and source code, making a decompilation feasible.

## Building

### Visual Studio .NET 2003

Of course, project files for Visual Studio .NET 2003 are available.

Visual Studio .NET 2003 was used by Pixel to create the original `Doukutsu.exe`, so these project files allow us to check the accuracy of the decompilation by comparing the generated assembly code to that of the original executable. The tool for this can be found in the 'devilution' folder.

### Makefile (MinGW-w64)

Run 'make' in this folder, preferably with some of the following settings:

Name | Function
--------|--------
`RELEASE=1` | Compile a release build (optimised, stripped, etc.)
`STATIC=1` | Produce a statically-linked executable (so you don't need to bundle DLL files)
`JAPANESE=1` | Enable the Japanese-language build (instead of the unofficial Aeon Genesis English translation)
`FIX_BUGS=1` | Fix various bugs in the game
`DEBUG_SAVE=1` | Re-enable the dummied-out 'Debug Save' option, and the ability to drag-and-drop save files onto the window

## Licensing

Being a decompilation, the majority of the code in this project belongs to Daisuke "Pixel" Amaya - not us. We've yet to agree on a license for our own code.
