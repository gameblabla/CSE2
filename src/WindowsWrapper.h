#pragma once

#ifdef _WIN32
#include <windef.h>
#include <wingdi.h>
#else

#include <stdio.h>

#define RGB(r,g,b) ((r) | ((g) << 8) | ((b) << 16))

typedef bool BOOL;

#define FALSE false
#define TRUE true

struct RECT
{
	long left;
	long top;
	long right;
	long bottom;
};

#define MAX_PATH FILENAME_MAX

#endif
