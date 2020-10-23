// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Software.h"

#include <stddef.h>
#include <string>

#include "SDL.h"

#include "../../../Misc.h"

static int bits_per_pixel = 24;
static Uint32 window_flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT;

static SDL_Surface *window_sdlsurface;
static SDL_Surface *framebuffer_sdlsurface;

bool WindowBackend_Software_CreateWindow(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	if (fullscreen)
		window_flags |= SDL_FULLSCREEN;
	else
		window_flags &= ~SDL_FULLSCREEN;

	window_sdlsurface = SDL_SetVideoMode(screen_width, screen_height, bits_per_pixel, window_flags);
	if (window_sdlsurface == NULL) {
		Backend_PrintError("Couldn't create 24bpp window: %s", SDL_GetError());
		bits_per_pixel = 32;
		window_sdlsurface = SDL_SetVideoMode(screen_width, screen_height, bits_per_pixel, window_flags);
	}

	if (window_sdlsurface != NULL)
	{
		SDL_WM_SetCaption(window_title, NULL);
		framebuffer_sdlsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, window_sdlsurface->w, window_sdlsurface->h, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);

		if (framebuffer_sdlsurface != NULL)
		{
			SDL_LockSurface(framebuffer_sdlsurface); // If this errors then oh dear

			Backend_PostWindowCreation();

			return true;
		}
		else
		{
			std::string error_message = std::string("Couldn't create framebuffer surface: ") + SDL_GetError();
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
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = framebuffer_sdlsurface->pitch;

	return (unsigned char*)framebuffer_sdlsurface->pixels;
}

void WindowBackend_Software_Display(void)
{
	SDL_UnlockSurface(framebuffer_sdlsurface);

	if (SDL_BlitSurface(framebuffer_sdlsurface, NULL, window_sdlsurface, NULL) < 0)
		Backend_PrintError("Couldn't blit framebuffer surface to window surface: %s", SDL_GetError());

	SDL_LockSurface(framebuffer_sdlsurface); // If this errors then oh dear

	if (SDL_Flip(window_sdlsurface) < 0)
		Backend_PrintError("Couldn't copy window surface to the screen: %s", SDL_GetError());
}

void WindowBackend_Software_HandleWindowResize(size_t width, size_t height)
{
	window_sdlsurface = SDL_SetVideoMode(width, height, bits_per_pixel, window_flags);
	if (window_sdlsurface == NULL)
		Backend_PrintError("Couldn't get SDL surface associated with window: %s", SDL_GetError());
}
