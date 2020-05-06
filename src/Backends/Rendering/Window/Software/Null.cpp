#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>

static unsigned char *framebuffer;

unsigned char* WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, size_t *pitch)
{
	(void)window_title;
	(void)fullscreen;

	framebuffer = (unsigned char*)malloc(screen_width * screen_height * 3);

	if (framebuffer != NULL)
	{
		*pitch = screen_width * 3;

		return framebuffer;
	}

	return NULL;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(framebuffer);
}

void WindowBackend_Software_Display(void)
{
	
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;
}
