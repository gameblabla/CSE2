// Released under the MIT licence.
// See LICENCE.txt for details.

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

#include "File.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, size_t *width, size_t *height, unsigned int bytes_per_pixel)
{
	int int_width, int_height;
	unsigned char *image_buffer = stbi_load_from_memory(in_buffer, in_buffer_size, &int_width, &int_height, NULL, bytes_per_pixel);

	*width = int_width;
	*height = int_height;

	return image_buffer;
}

unsigned char* DecodeBitmapFromFile(const char *path, size_t *width, size_t *height, unsigned int bytes_per_pixel)
{
	size_t file_size;
	unsigned char *file_buffer = LoadFileToMemory(path, &file_size);

	if (file_buffer != NULL)
	{
		unsigned char *image_buffer = DecodeBitmap(file_buffer, file_size, width, height, bytes_per_pixel);

		free(file_buffer);

		return image_buffer;
	}

	return NULL;
}

void FreeBitmap(unsigned char *buffer)
{
	stbi_image_free(buffer);
}
