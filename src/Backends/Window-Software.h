#pragma once

#include <stddef.h>

#include "../WindowsWrapper.h"

unsigned char* WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, BOOL fullscreen, BOOL vsync, size_t *pitch);
void WindowBackend_Software_DestroyWindow(void);
void WindowBackend_Software_Display(void);
void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height);
