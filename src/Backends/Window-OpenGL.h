#pragma once

#include "../WindowsWrapper.h"

bool WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, bool fullscreen);
void WindowBackend_OpenGL_DestroyWindow(void);
void WindowBackend_OpenGL_Display(void);
