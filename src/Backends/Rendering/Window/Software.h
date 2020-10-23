// Released under the MIT licence.
// See LICENCE.txt for details.

#pragma once

#include <stddef.h>

bool WindowBackend_Software_CreateWindow(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen);
void WindowBackend_Software_DestroyWindow(void);
unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch);
void WindowBackend_Software_Display(void);
void WindowBackend_Software_HandleWindowResize(size_t width, size_t height);
