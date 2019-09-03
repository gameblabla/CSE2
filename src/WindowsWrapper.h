#pragma once

#include <stdio.h>

typedef int HWND;

typedef int BOOL;

typedef unsigned char BYTE;
typedef unsigned long DWORD;

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

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;

#ifndef MAX_PATH
#define MAX_PATH FILENAME_MAX
#endif

BOOL SystemTask();
