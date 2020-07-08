#pragma once

#define RGB(r,g,b) ((r) | ((g) << 8) | ((b) << 16))

// This isn't defined by `windows.h` - it's custom
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
