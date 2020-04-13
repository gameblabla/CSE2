#pragma once

#include <stddef.h>

unsigned char* WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, size_t *pitch);
void WindowBackend_Software_DestroyWindow(void);
void WindowBackend_Software_Display(void);
void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height);
