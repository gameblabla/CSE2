#pragma once

#ifdef WINDOWS
#include <windows.h>
// Avoid name collisions
#undef DrawText
#undef FindResource
#undef CreateWindow
#else

#include <stdio.h>

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

#ifndef MAX_PATH
#define MAX_PATH FILENAME_MAX
#endif

BOOL SystemTask();
