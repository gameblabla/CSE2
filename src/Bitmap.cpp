#include "Bitmap.h"

#include <stddef.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_BMP
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#define STBI_NO_STDIO
#include "../external/stb_image.h"

#include "WindowsWrapper.h"

#include "File.h"

unsigned char* DecodeBitmapWithAlpha(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height, BOOL colour_key)
{
	int channels_in_file;
	unsigned char *image_buffer = stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, &channels_in_file, 4);

	if (image_buffer == NULL)
		return NULL;

	// If image has no alpha channel, then perform colour-keying (set #000000 to transparent)
	if (colour_key && channels_in_file == 3)
		for (size_t i = 0; i < *width * *height; ++i)
			if (image_buffer[(i * 4) + 0] == 0 && image_buffer[(i * 4) + 1] == 0 && image_buffer[(i * 4) + 2] == 0)
				image_buffer[(i * 4) + 3] = 0;

	return image_buffer;
}

unsigned char* DecodeBitmapWithAlphaFromFile(const char *path, unsigned int *width, unsigned int *height, BOOL colour_key)
{
	size_t file_size;
	unsigned char *file_buffer = LoadFileToMemory(path, &file_size);

	if (file_buffer != NULL)
	{
		unsigned char *image_buffer = DecodeBitmapWithAlpha(file_buffer, file_size, width, height, colour_key);

		free(file_buffer);

		return image_buffer;
	}

	return NULL;
}

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
