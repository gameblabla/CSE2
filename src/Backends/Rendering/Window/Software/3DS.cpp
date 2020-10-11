#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>

#include <3ds.h>

static unsigned char *framebuffer;
static size_t framebuffer_pitch;
static size_t framebuffer_width;
static size_t framebuffer_height;

bool WindowBackend_Software_CreateWindow(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

//	gfxSetDoubleBuffering(GFX_TOP, true);

	gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);

	framebuffer = (unsigned char*)malloc(screen_width * screen_height * 3);

	if (framebuffer != NULL)
	{
		framebuffer_pitch = screen_width * 3;
		framebuffer_width = screen_width;
		framebuffer_height = screen_height;

		return true;
	}

	return false;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(framebuffer);
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = framebuffer_pitch;

	return framebuffer;
}

void WindowBackend_Software_Display(void)
{
	// Absolutely disgusting
	unsigned char *actual_framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

	unsigned char *framebuffer_pointer = framebuffer;

	const size_t offset = ((400 - framebuffer_width) / 2) * 240;

	for (unsigned int h = framebuffer_height - 1; h-- != 0;)
	{
		for (unsigned int w = 0; w < framebuffer_width * 240; w += 240)
		{
			actual_framebuffer[(offset + w + h) * 3 + 2] = *framebuffer_pointer++;
			actual_framebuffer[(offset + w + h) * 3 + 1] = *framebuffer_pointer++;
			actual_framebuffer[(offset + w + h) * 3 + 0] = *framebuffer_pointer++;
		}		
	}

	gfxFlushBuffers();
	gfxScreenSwapBuffers(GFX_TOP, false);
}

void WindowBackend_Software_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;
}
