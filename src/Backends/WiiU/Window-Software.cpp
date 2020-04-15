#include "../Window-Software.h"

#include <stdlib.h>
#include <string.h>

//#include <coreinit/cache.h>
#include <coreinit/screen.h>

static unsigned char *fake_framebuffer;

//static unsigned char *tv_framebuffer;
static unsigned char *drc_framebuffer;

static size_t framebuffer_width;
static size_t framebuffer_height;

//static uint32_t tv_buffer_size;
static uint32_t drc_buffer_size;

unsigned char* WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync, size_t *pitch)
{
	(void)window_title;
	(void)fullscreen;

	*vsync = FALSE;

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

	fake_framebuffer = (unsigned char*)malloc(screen_width * screen_height * 4);

	*pitch = screen_width * 4;

	return fake_framebuffer;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(fake_framebuffer);
	free(drc_framebuffer);
//	free(tv_framebuffer);
	OSScreenShutdown();
}

void WindowBackend_Software_Display(void)
{
	const size_t line_size = (drc_buffer_size / 480) / 2;

	static bool flipflop;

	const unsigned char *in_pointer = fake_framebuffer;
	unsigned char *out_pointer = drc_framebuffer;

	if (!flipflop)
		out_pointer += drc_buffer_size / 2;

	out_pointer += ((854 - framebuffer_width) * 4) / 2;

	for (size_t y = 0; y < framebuffer_height; ++y)
	{
		memcpy(out_pointer, in_pointer, framebuffer_width * 4);

		in_pointer += framebuffer_width * 4;
		out_pointer += line_size;
	}

	flipflop = !flipflop;

//	static unsigned char accumulator = 0;

//	accumulator += 0x10;

//	for (size_t i = 0; i < buffer_size; ++i)
//		real_framebuffer[i] = accumulator;

//	DCFlushRange(tv_framebuffer, tv_buffer_size);
//	DCFlushRange(drc_framebuffer, drc_buffer_size);

//	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;
}
