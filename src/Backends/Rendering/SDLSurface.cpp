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
	SDL_FreeSurface(surface->sdl_surface);
	free(surface);
}

unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch)
{
	*pitch = surface->sdl_surface->pitch;
	return (unsigned char*)surface->sdl_surface->pixels;
}

void Backend_Unlock(Backend_Surface *surface)
{
	
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
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

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	Backend_Blit(source_surface, rect, &framebuffer, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
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
	Backend_Blit(&framebuffer, rect, surface, rect->left, rect->top, FALSE);
}

void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	DrawText(font, (unsigned char*)surface->sdl_surface->pixels, surface->sdl_surface->pitch, surface->sdl_surface->w, surface->sdl_surface->h, x, y, colour, text, strlen(text));
}

void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	Backend_DrawText(&framebuffer, font, x, y, text, colour);
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
