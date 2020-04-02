#include "Bitmap.h"

#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_BMP
#define STBI_NO_LINEAR
#include "../external/stb_image.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height)
{
	return stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, NULL, 3);
}

unsigned char* DecodeBitmapFromFile(const char *path, unsigned int *width, unsigned int *height)
{
	return stbi_load(path, (int*)width, (int*)height, NULL, 3);
}

void FreeBitmap(unsigned char *buffer)
{
	stbi_image_free(buffer);
}
