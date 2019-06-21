#pragma once

#ifdef WINDOWS
#include <Windows.h>
// Avoid name collisions
#undef DrawText
#undef FindResource
#else

typedef int HWND;

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

struct RECT
{
	long left;
	long top;
	long right;
	long bottom;
};
#endif

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;

int rep_rand();
void rep_srand(unsigned int seed);

BOOL SystemTask();
