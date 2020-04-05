#include "Bitmap.h"

#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_BMP
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "../external/stb_image.h"

#include "WindowsWrapper.h"

static void DoColourKey(unsigned char *image_buffer, unsigned int width, unsigned int height)
{
	for (size_t i = 0; i < width * height; ++i)
		if (image_buffer[(i * 4) + 0] == 0 && image_buffer[(i * 4) + 1] == 0 && image_buffer[(i * 4) + 2] == 0)
			image_buffer[(i * 4) + 3] = 0;
}

unsigned char* DecodeBitmapWithAlpha(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height, BOOL colour_key)
{
	int channels_in_file;
	unsigned char *image_buffer = stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, &channels_in_file, 4);

	if (image_buffer == NULL)
		return NULL;

	// If image has no alpha channel, then perform colour-keying (set #000000 to transparent)
	if (colour_key && channels_in_file == 3)
		DoColourKey(image_buffer, *width, *height);

	return image_buffer;
}

unsigned char* DecodeBitmapWithAlphaFromFile(const char *path, unsigned int *width, unsigned int *height, BOOL colour_key)
{
	int channels_in_file;
	unsigned char *image_buffer = stbi_load(path, (int*)width, (int*)height, &channels_in_file, 4);

	if (image_buffer == NULL)
		return NULL;

	// If image has no alpha channel, then perform colour-keying (set #000000 to transparent)
	if (colour_key && channels_in_file == 3)
		DoColourKey(image_buffer, *width, *height);

	return image_buffer;
}

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
