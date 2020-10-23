// Released under the MIT licence.
// See LICENCE.txt for details.

#pragma once

#include <stddef.h>

bool WindowBackend_OpenGL_CreateWindow(const char *window_title, size_t *screen_width, size_t *screen_height, bool fullscreen);
void WindowBackend_OpenGL_DestroyWindow(void);
void WindowBackend_OpenGL_Display(void);
