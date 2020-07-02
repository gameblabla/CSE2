#include "../Software.h"

#include <stddef.h>
#include <string>

#include "SDL.h"

#include "../../../Misc.h"
#include "../../../Shared/SDL2.h"

SDL_Window *window;

static SDL_Surface *window_sdlsurface;
static SDL_Surface *framebuffer_sdlsurface;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync)
{
	*vsync = false;

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
		if (fullscreen)
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0)
				Backend_PrintError("Couldn't set window to fullscree: %s", SDL_GetError());

		window_sdlsurface = SDL_GetWindowSurface(window);

		if (window_sdlsurface != NULL)
		{

			framebuffer_sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGBA32);

			if (framebuffer_sdlsurface != NULL)
			{
				Backend_PostWindowCreation();

				return true;
			}
			else
			{
				std::string error_message = std::string("Couldn't create framebuffer surface: ") + SDL_GetError();
				Backend_ShowMessageBox("Fatal error (software rendering backend)", error_message.c_str());
				SDL_DestroyWindow(window);
			}
		}
		else
		{
			std::string error_message = std::string("Couldn't get SDL surface associated with window: ") + SDL_GetError();
			Backend_ShowMessageBox("Fatal error (software rendering backend)", error_message.c_str());
		}
	}
	else
	{
		std::string error_message = std::string("Couldn't create window: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (software rendering backend)", error_message.c_str());
	}

	return false;
}

void WindowBackend_Software_DestroyWindow(void)
{
	SDL_FreeSurface(framebuffer_sdlsurface);
	SDL_DestroyWindow(window);
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = framebuffer_sdlsurface->pitch;

	return (unsigned char*)framebuffer_sdlsurface->pixels;
}

void WindowBackend_Software_Display(void)
{
	if (SDL_BlitSurface(framebuffer_sdlsurface, NULL, window_sdlsurface, NULL) < 0)
		Backend_PrintError("Couldn't blit framebuffer surface to window surface: %s", SDL_GetError());

	if (SDL_UpdateWindowSurface(window) < 0)
		Backend_PrintError("Couldn't copy window surface to the screen: %s", SDL_GetError());
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);

	if (window_sdlsurface == NULL)
		Backend_PrintError("Couldn't get SDL surface associated with window: %s", SDL_GetError());
}
