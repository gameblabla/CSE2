#include "../Window-Software.h"

#include <stdlib.h>
#include <string.h>

#include <coreinit/cache.h>
#include <coreinit/screen.h>

#include "../../Attributes.h"

static unsigned char *fake_framebuffer;

//static unsigned char *tv_framebuffer;
static unsigned char *drc_framebuffer;

static size_t framebuffer_width;
static size_t framebuffer_height;

//static uint32_t tv_buffer_size;
static uint32_t drc_buffer_size;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	framebuffer_width = screen_width;
	framebuffer_height = screen_height;

	OSScreenInit();

	OSScreenEnableEx(SCREEN_TV, FALSE);
	OSScreenEnableEx(SCREEN_DRC, TRUE);

//	tv_buffer_size = OSScreenGetBufferSizeEx(SCREEN_TV);
	drc_buffer_size = OSScreenGetBufferSizeEx(SCREEN_DRC);

//	tv_framebuffer = (unsigned char*)aligned_alloc(0x100, tv_buffer_size);	// C11 fun
	drc_framebuffer = (unsigned char*)aligned_alloc(0x100, drc_buffer_size);	// C11 fun

//	OSScreenSetBufferEx(SCREEN_TV, tv_framebuffer);
	OSScreenSetBufferEx(SCREEN_DRC, drc_framebuffer);

	fake_framebuffer = (unsigned char*)malloc(framebuffer_width * framebuffer_height * 3);

	return true;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(fake_framebuffer);
	free(drc_framebuffer);
//	free(tv_framebuffer);
	OSScreenShutdown();
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = framebuffer_width * 3;

	return fake_framebuffer;
}

ATTRIBUTE_HOT void WindowBackend_Software_Display(void)
{
	const size_t pitch = (drc_buffer_size / 480) / 2;

	static bool flipflop;

	const unsigned char *in_pointer = fake_framebuffer;

	if (framebuffer_width <= 426 && framebuffer_height <= 240)
	{
		const size_t line_size = framebuffer_width * 2 * 4;
		const size_t line_delta = pitch - line_size;

		unsigned char *out_pointer = drc_framebuffer;

		if (!flipflop)
			out_pointer += drc_buffer_size / 2;

		out_pointer += ((854 - (framebuffer_width * 2)) * 4) / 2;

		for (size_t y = 0; y < framebuffer_height; ++y)
		{
			for (size_t x = 0; x < framebuffer_width; ++x)
			{
				*out_pointer++ = in_pointer[0];
				*out_pointer++ = in_pointer[1];
				*out_pointer++ = in_pointer[2];
				*out_pointer++ = 0;
				*out_pointer++ = in_pointer[0];
				*out_pointer++ = in_pointer[1];
				*out_pointer++ = in_pointer[2];
				*out_pointer++ = 0;

				in_pointer += 3;
			}

			memcpy(out_pointer + line_delta, out_pointer - line_size, line_size);

			out_pointer += line_delta + pitch;
		}
	}
	else
	{
		const size_t line_size = framebuffer_width * 4;
		const size_t line_delta = pitch - line_size;

		unsigned char *out_pointer = drc_framebuffer;

		if (!flipflop)
			out_pointer += drc_buffer_size / 2;

		out_pointer += ((854 - framebuffer_width) * 4) / 2;

		for (size_t y = 0; y < framebuffer_height; ++y)
		{
			for (size_t x = 0; x < framebuffer_width; ++x)
			{
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = 0;
			}

			out_pointer += line_delta;
		}
	}

	flipflop = !flipflop;

//	DCStoreRange(tv_framebuffer, tv_buffer_size);
	DCStoreRange(drc_framebuffer, drc_buffer_size);

//	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;
}