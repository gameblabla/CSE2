#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

typedef struct Backend_Surface
{
	SDL_Surface *sdl_surface;
} Backend_Surface;

typedef struct Backend_Glyph
{
	SDL_Surface *sdl_surface;
} Backend_Glyph;

static SDL_Window *window;
static SDL_Surface *window_surface;

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

BOOL Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	window_surface = SDL_GetWindowSurface(window);

	framebuffer.sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, window_surface->w, window_surface->h, 0, SDL_PIXELFORMAT_RGB24);

	if (framebuffer.sdl_surface == NULL)
		return FALSE;

	return TRUE;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(framebuffer.sdl_surface);
}

void Backend_DrawScreen(void)
{
	SDL_BlitSurface(framebuffer.sdl_surface, NULL, window_surface, NULL);
	SDL_UpdateWindowSurface(window);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);

	if (surface->sdl_surface == NULL)
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

	SDL_FreeSurface(surface->sdl_surface);
	free(surface);
}

unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->sdl_surface->pitch;
	return (unsigned char*)surface->sdl_surface->pixels;
}

void Backend_Unlock(Backend_Surface *surface)
{
	(void)surface;
}

static void BlitCommon(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
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

	SDL_SetColorKey(source_surface->sdl_surface, colour_key ? SDL_TRUE : SDL_FALSE, SDL_MapRGB(source_surface->sdl_surface->format, 0, 0, 0)); // Assumes the colour key will always be #000000 (black)

	SDL_BlitSurface(source_surface->sdl_surface, &source_rect, destination_surface->sdl_surface, &destination_rect);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y)
{
	BlitCommon(source_surface, rect, &framebuffer, x, y, TRUE);
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	BlitCommon(source_surface, rect, &framebuffer, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect destination_rect;
	RectToSDLRect(rect, &destination_rect);

	SDL_FillRect(surface->sdl_surface, &destination_rect, SDL_MapRGB(surface->sdl_surface->format, red, green, blue));
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	Backend_ColourFill(&framebuffer, rect, red, green, blue);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	BlitCommon(&framebuffer, rect, surface, rect->left, rect->top, FALSE);
}

BOOL Backend_SupportsSubpixelGlyph(void)
{
	return FALSE;	// SDL_Surfaces don't have per-component alpha
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

	if (glyph->sdl_surface == NULL)
	{
		free(glyph);
		return NULL;
	}

	switch (pixel_mode)
	{
		// FONT_PIXEL_MODE_LCD is unsupported

		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->sdl_surface->pixels + y * glyph->sdl_surface->pitch;

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
				unsigned char *destination_pointer = (unsigned char*)glyph->sdl_surface->pixels + y * glyph->sdl_surface->pitch;

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

	SDL_FreeSurface(glyph->sdl_surface);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = glyph->sdl_surface->w;
	rect.h = glyph->sdl_surface->h;

	SDL_SetSurfaceColorMod(glyph->sdl_surface, colours[0], colours[1], colours[2]);

	SDL_BlitSurface(glyph->sdl_surface, NULL, surface->sdl_surface, &rect);
}

void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	Backend_DrawGlyph(&framebuffer, glyph, x, y, colours);
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_surface = SDL_GetWindowSurface(window);
}
