#pragma once

#include "../WindowsWrapper.h"

BOOL WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, BOOL fullscreen);
void WindowBackend_OpenGL_DestroyWindow(void);
void WindowBackend_OpenGL_Display(void);
