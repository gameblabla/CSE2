#pragma once

#include <stddef.h>

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, int *width, int *height);
void FreeBitmap(unsigned char *buffer);
