#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>

static unsigned char *framebuffer;
static size_t framebuffer_pitch;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync)
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

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;
}
