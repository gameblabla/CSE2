This is a build of FreeType 2.10.0, built with MSVC2003.

Though Freetype's source code doesn't come with a Visual Studio 2003 solution
file, the VC6 project in 'builds/windows/visualc' works just as well.

This build comes with subpixel rendering enabled: this was done by defining
FT_CONFIG_OPTION_SUBPIXEL_RENDERING in 'include/freetype/config/ftoption.h'. 
