#pragma once

#include <stddef.h>

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen);
void WindowBackend_Software_DestroyWindow(void);
unsigned char* WindowBackend_Software_LockFramebuffer(size_t *pitch);
void WindowBackend_Software_UnlockFramebuffer(void);
void WindowBackend_Software_Display(void);
void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height);
