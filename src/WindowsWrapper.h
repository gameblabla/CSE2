#pragma once

#include <stdio.h>

#define RGB(r,g,b) ((r) | ((g) << 8) | ((b) << 16))
#define RGBA(r,g,b,a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))

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
