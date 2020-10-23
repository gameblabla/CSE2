// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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
		framebuffer_pitch = screen_height * 3;
		framebuffer_width = screen_height;
		framebuffer_height = screen_width;

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
	memcpy(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL) + (400 - framebuffer_height) * 240 * 3 / 2, framebuffer, framebuffer_pitch * framebuffer_height);

	gfxFlushBuffers();
	gfxScreenSwapBuffers(GFX_TOP, false);
}

void WindowBackend_Software_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;
}
