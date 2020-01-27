#include "Bitmap.h"

#include <stddef.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_BMP
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#include "stb_image.h"

#include "WindowsWrapper.h"

unsigned char* DecodeBitmap(const unsigned char *in_buffer, size_t in_buffer_size, unsigned int *width, unsigned int *height, BOOL colour_key)
{
	int channels_in_file;
	unsigned char *image_buffer = stbi_load_from_memory(in_buffer, in_buffer_size, (int*)width, (int*)height, &channels_in_file, 4);

	if (image_buffer == NULL)
		return NULL;

	if (colour_key && channels_in_file == 3)
	{
		// If image has no alpha channel, then perform colour-keying (set #000000 to transparent)
		for (size_t i = 0; i < *width * *height; ++i)
			if (image_buffer[(i * 4) + 0] == 0 && image_buffer[(i * 4) + 1] == 0 && image_buffer[(i * 4) + 2] == 0)
				image_buffer[(i * 4) + 3] = 0;
	}

	return image_buffer;
}

void FreeBitmap(unsigned char *buffer)
{
	stbi_image_free(buffer);
}
