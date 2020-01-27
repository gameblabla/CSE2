#pragma once

#include <stddef.h>

#include "WindowsWrapper.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height, BOOL colour_key);
unsigned char* DecodeBitmapFromFile(const char *path, unsigned int *width, unsigned int *height, BOOL colour_key);
void FreeBitmap(unsigned char *buffer);
