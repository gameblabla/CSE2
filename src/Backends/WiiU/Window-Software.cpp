#include "../Window-Software.h"

#include <stdlib.h>
#include <string.h>

#include <coreinit/cache.h>
#include <coreinit/screen.h>

//static unsigned char *tv_framebuffer;
static unsigned char *drc_framebuffer;

static size_t framebuffer_width;
static size_t framebuffer_height;

//static uint32_t tv_buffer_size;
static uint32_t drc_buffer_size;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync)
{
	(void)window_title;
	(void)fullscreen;

	*vsync = false;

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

	return true;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(drc_framebuffer);
//	free(tv_framebuffer);
	OSScreenShutdown();
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	const size_t line_size = (drc_buffer_size / 480) / 2;

	static bool flipflop;

	unsigned char *out_pointer = drc_framebuffer;

	if (!flipflop)
		out_pointer += drc_buffer_size / 2;

	out_pointer += ((854 - framebuffer_width) * 4) / 2;
	out_pointer += ((480 - framebuffer_height) * line_size) / 2;

	flipflop = !flipflop;

	*pitch = line_size;

	return out_pointer;
}

void WindowBackend_Software_Display(void)
{
//	DCFlushRange(tv_framebuffer, tv_buffer_size);
	DCFlushRange(drc_framebuffer, drc_buffer_size);

//	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;
}
