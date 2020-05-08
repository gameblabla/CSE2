#pragma once

#include <stddef.h>

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync);
void WindowBackend_Software_DestroyWindow(void);
unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch);
void WindowBackend_Software_Display(void);
void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height);
