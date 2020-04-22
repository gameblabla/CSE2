#include "Bitmap.h"

#include <stddef.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_BMP
#define STBI_NO_LINEAR
#define STBI_NO_STDIO
#include "../external/stb_image.h"

#include "File.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height)
{
	return stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, NULL, 3);
}

unsigned char* DecodeBitmapFromFile(const char *path, unsigned int *width, unsigned int *height)
{
	size_t file_size;
	unsigned char *file_buffer = LoadFileToMemory(path, &file_size);

	if (file_buffer != NULL)
	{
		unsigned char *image_buffer = stbi_load_from_memory(file_buffer, file_size, (int*)width, (int*)height, NULL, 3);

		free(file_buffer);

		return image_buffer;
	}

	return NULL;
}

void FreeBitmap(unsigned char *buffer)
{
	stbi_image_free(buffer);
}
