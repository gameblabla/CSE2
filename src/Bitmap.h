#pragma once

#include <stddef.h>

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height);
unsigned char* DecodeBitmapFromFile(const char *path, unsigned int *width, unsigned int *height);
void FreeBitmap(unsigned char *buffer);
