#include "Bitmap.h"

#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_BMP
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#include "stb_image.h"

#include "File.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height)
{
	return stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, NULL, 3);
}

unsigned char* DecodeBitmapFromFile(const char *path, unsigned int *width, unsigned int *height)
{
	unsigned char *image_buffer = NULL;

	size_t size;
	unsigned char *data = LoadFileToMemory(path, &size);

	if (data != NULL)
	{
		image_buffer = DecodeBitmap(data, size, width, height);

		free(data);
	}

	return image_buffer;
}

void FreeBitmap(unsigned char *buffer)
{
	stbi_image_free(buffer);
}
