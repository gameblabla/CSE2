#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

typedef struct Backend_Surface
{
	SDL_Surface *sdlsurface;
} Backend_Surface;

typedef struct Backend_Glyph
{
	SDL_Surface *sdlsurface;
} Backend_Glyph;

static SDL_Window *window;
static SDL_Surface *window_sdlsurface;

static Backend_Surface framebuffer;

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

SDL_Window* Backend_CreateWindow(const char *title, int width, int height)
{
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
}

Backend_Surface* Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	window_sdlsurface = SDL_GetWindowSurface(window);

	framebuffer.sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

	if (framebuffer.sdlsurface == NULL)
		return NULL;

	return &framebuffer;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(framebuffer.sdlsurface);
}

void Backend_DrawScreen(void)
{
	SDL_BlitSurface(framebuffer.sdlsurface, NULL, window_sdlsurface, NULL);
	SDL_UpdateWindowSurface(window);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

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

void Backend_FreeSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	SDL_FreeSurface(surface->sdlsurface);
	free(surface);
}

BOOL Backend_IsSurfaceLost(Backend_Surface *surface)
{
	return FALSE;
}

void Backend_RestoreSurface(Backend_Surface *surface)
{
	
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->sdlsurface->pitch;
	return (unsigned char*)surface->sdlsurface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	(void)surface;
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
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

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect destination_rect;
	RectToSDLRect(rect, &destination_rect);

	SDL_FillRect(surface->sdlsurface, &destination_rect, SDL_MapRGB(surface->sdlsurface->format, red, green, blue));
}

BOOL Backend_SupportsSubpixelGlyphs(void)
{
	return FALSE;	// SDL_Surfaces don't have per-component alpha
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, FontPixelMode pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

	if (glyph->sdlsurface == NULL)
	{
		free(glyph);
		return NULL;
	}

	switch (pixel_mode)
	{
		case FONT_PIXEL_MODE_LCD:
			// Unsupported
			break;

		case FONT_PIXEL_MODE_GRAY:
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

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->sdlsurface->pixels + y * glyph->sdlsurface->pitch;

				for (unsigned int x = 0; x < width; ++x)
				{
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = *source_pointer++ ? 0xFF : 0;
				}
			}

			break;
	}

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	SDL_FreeSurface(glyph->sdlsurface);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = glyph->sdlsurface->w;
	rect.h = glyph->sdlsurface->h;

	SDL_SetSurfaceColorMod(glyph->sdlsurface, colours[0], colours[1], colours[2]);

	SDL_BlitSurface(glyph->sdlsurface, NULL, surface->sdlsurface, &rect);
}

void Backend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);
}
