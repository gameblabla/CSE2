// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>

static unsigned char *framebuffer;
static size_t framebuffer_pitch;

bool WindowBackend_Software_CreateWindow(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	framebuffer = (unsigned char*)malloc(screen_width * screen_height * 3);

	if (framebuffer != NULL)
	{
		framebuffer_pitch = screen_width * 3;

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
	
}

void WindowBackend_Software_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;
}
