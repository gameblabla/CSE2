#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NODRAWTEXT
#include <windows.h>
#undef FindResource
#else

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

#endif
