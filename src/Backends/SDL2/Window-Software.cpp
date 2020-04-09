#include "../Window-Software.h"
#include "Window.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../Misc.h"

SDL_Window *window;

static SDL_Surface *window_sdlsurface;
static SDL_Surface *framebuffer_sdlsurface;

unsigned char* WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, BOOL fullscreen, size_t *pitch)
{
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		window_sdlsurface = SDL_GetWindowSurface(window);

		framebuffer_sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

		if (framebuffer_sdlsurface != NULL)
		{
			*pitch = framebuffer_sdlsurface->pitch;

			Backend_PostWindowCreation();

			return (unsigned char*)framebuffer_sdlsurface->pixels;
		}
		else
		{
			Backend_ShowMessageBox("Fatal error (software rendering backend)", "Could not create framebuffer surface");
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		Backend_ShowMessageBox("Fatal error (software rendering backend)", "Could not create window");
	}

	return NULL;
}

void WindowBackend_Software_DestroyWindow(void)
{
	SDL_FreeSurface(framebuffer_sdlsurface);
	SDL_DestroyWindow(window);
}

void WindowBackend_Software_Display(void)
{
	SDL_BlitSurface(framebuffer_sdlsurface, NULL, window_sdlsurface, NULL);
	SDL_UpdateWindowSurface(window);
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);
}
