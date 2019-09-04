#pragma once

#include <windows.h>
// Avoid name collisions
#undef DrawText
#undef FindResource
#undef CreateWindow

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;
