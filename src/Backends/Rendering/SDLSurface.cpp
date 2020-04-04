#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../Platform.h"
#include "../SDL2/Platform.h"

typedef struct RenderBackend_Surface
{
	SDL_Surface *sdlsurface;
} RenderBackend_Surface;

typedef struct RenderBackend_Glyph
{
	SDL_Surface *sdlsurface;
} RenderBackend_Glyph;

static SDL_Surface *window_sdlsurface;

static RenderBackend_Surface framebuffer;

static unsigned char glyph_colour_channels[3];
static SDL_Surface *glyph_destination_sdlsurface;

static void RectToSDLRect(const RECT *rect, SDL_Rect *sdl_rect)
{
	sdl_rect->x = (int)rect->left;
	sdl_rect->y = (int)rect->top;
	sdl_rect->w = (int)(rect->right - rect->left);
	sdl_rect->h = (int)(rect->bottom - rect->top);

	if (sdl_rect->w < 0)
		sdl_rect->w = 0;

	if (sdl_rect->h < 0)
		sdl_rect->h = 0;
}

Backend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen)
{
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		window_sdlsurface = SDL_GetWindowSurface(window);

		framebuffer.sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

		if (framebuffer.sdlsurface != NULL)
		{
			PlatformBackend_PostWindowCreation();

			return &framebuffer;
		}
		else
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (SDLSurface rendering backend)", "Could not create framebuffer surface", window);
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (SDLSurface rendering backend)", "Could not create window", NULL);
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	SDL_FreeSurface(framebuffer.sdlsurface);
	SDL_DestroyWindow(window);
}

void RenderBackend_DrawScreen(void)
{
	SDL_BlitSurface(framebuffer.sdlsurface, NULL, window_sdlsurface, NULL);
	SDL_UpdateWindowSurface(window);
}

Backend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);

	if (surface->sdlsurface == NULL)
	{
		free(surface);
		return NULL;
	}

	return surface;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	if (surface == NULL)
		return;

	SDL_FreeSurface(surface->sdlsurface);
	free(surface);
}

BOOL RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface)
{
	(void)surface;

	return FALSE;
}

void RenderBackend_RestoreSurface(RenderBackend_Surface *surface)
{
	(void)surface;
}

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	if (surface == NULL)
		return NULL;

	*pitch = surface->sdlsurface->pitch;
	return (unsigned char*)surface->sdlsurface->pixels;
}

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height)
{
	(void)surface;
	(void)width;
	(void)height;
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RECT *rect, RenderBackend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect;
	destination_rect.x = x;
	destination_rect.y = y;
	destination_rect.w = source_rect.w;
	destination_rect.h = source_rect.h;

	SDL_SetColorKey(source_surface->sdlsurface, colour_key ? SDL_TRUE : SDL_FALSE, SDL_MapRGB(source_surface->sdlsurface->format, 0, 0, 0)); // Assumes the colour key will always be #000000 (black)

	SDL_BlitSurface(source_surface->sdlsurface, &source_rect, destination_surface->sdlsurface, &destination_rect);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect destination_rect;
	RectToSDLRect(rect, &destination_rect);

	SDL_FillRect(surface->sdlsurface, &destination_rect, SDL_MapRGB(surface->sdlsurface->format, red, green, blue));
}

Backend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	RenderBackend_Glyph *glyph = (RenderBackend_Glyph*)malloc(sizeof(RenderBackend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

	if (glyph->sdlsurface == NULL)
	{
		free(glyph);
		return NULL;
	}

	for (unsigned int y = 0; y < height; ++y)
	{
		const unsigned char *source_pointer = pixels + y * pitch;
		unsigned char *destination_pointer = (unsigned char*)glyph->sdlsurface->pixels + y * glyph->sdlsurface->pitch;

		for (unsigned int x = 0; x < width; ++x)
		{
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = *source_pointer++;
		}
	}

	return glyph;
}

void RenderBackend_UnloadGlyph(RenderBackend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	SDL_FreeSurface(glyph->sdlsurface);
	free(glyph);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels)
{
	if (destination_surface == NULL)
		return;

	glyph_destination_sdlsurface = destination_surface->sdlsurface;

	memcpy(glyph_colour_channels, colour_channels, sizeof(glyph_colour_channels));
}

void RenderBackend_DrawGlyph(RenderBackend_Glyph *glyph, long x, long y)
{
	if (glyph == NULL)
		return;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = glyph->sdlsurface->w;
	rect.h = glyph->sdlsurface->h;

	SDL_SetSurfaceColorMod(glyph->sdlsurface, glyph_colour_channels[0], glyph_colour_channels[1], glyph_colour_channels[2]);

	SDL_BlitSurface(glyph->sdlsurface, NULL, glyph_destination_sdlsurface, &rect);
}

void RenderBackend_FlushGlyphs(void)
{
	
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);
}
