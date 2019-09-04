#pragma once

#include <stdio.h>

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

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;

#define MAX_PATH FILENAME_MAX
