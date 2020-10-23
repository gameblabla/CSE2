// Released under the MIT licence.
// See LICENCE.txt for details.

#pragma once

#include <stddef.h>

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, size_t *width, size_t *height, unsigned int bytes_per_pixel);
unsigned char* DecodeBitmapFromFile(const char *path, size_t *width, size_t *height, unsigned int bytes_per_pixel);
void FreeBitmap(unsigned char *buffer);
