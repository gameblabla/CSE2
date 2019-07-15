#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

struct Backend_Surface
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
};

static SDL_Window *window;
static SDL_Surface *window_surface;
static SDL_Surface *screen_surface;

static Backend_Surface framebuffer;

BOOL Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	window_surface = SDL_GetWindowSurface(window);

	screen_surface = SDL_CreateRGBSurfaceWithFormat(0, window_surface->w, window_surface->h, 0, SDL_PIXELFORMAT_RGB24);

	if (screen_surface == NULL)
		return FALSE;

	framebuffer.pixels = (unsigned char*)screen_surface->pixels;
	framebuffer.width = screen_surface->w;
	framebuffer.height = screen_surface->h;
	framebuffer.pitch = screen_surface->pitch;

	return TRUE;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(screen_surface);
}

void Backend_DrawScreen(void)
{
	SDL_BlitSurface(screen_surface, NULL, window_surface, NULL);
	SDL_UpdateWindowSurface(window);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->pixels = (unsigned char*)malloc(width * height * 3);

	if (surface->pixels == NULL)
	{
		free(surface);
		return NULL;
	}

	surface->width = width;
	surface->height = height;
	surface->pitch = width * 3;

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	free(surface->pixels);
	free(surface);
}

void Backend_LoadPixels(Backend_Surface *surface, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int pitch)
{
	for (unsigned int i = 0; i < height; ++i)
	{
		const unsigned char *src_row = &pixels[i * pitch];
		unsigned char *dst_row = &surface->pixels[i * surface->pitch];

		memcpy(dst_row, src_row, width * 3);
	}
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect and coordinates so we don't write outside the pixel buffer
	long overflow;

	overflow = 0 - x;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
		x += overflow;
	}

	overflow = 0 - y;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
		y += overflow;
	}

	overflow = (x + (rect_clamped.right - rect_clamped.left)) - destination_surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = (y + (rect_clamped.bottom - rect_clamped.top)) - destination_surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	// Do the actual blitting
	if (colour_key)
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 3)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 3)];

			for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
			{
				if (source_pointer[0] != 0 || source_pointer[1] != 0 || source_pointer[2] != 0)	// Assumes the colour key will always be #00000000 (black)
				{
					destination_pointer[0] = source_pointer[0];
					destination_pointer[1] = source_pointer[1];
					destination_pointer[2] = source_pointer[2];
				}

				source_pointer += 3;
				destination_pointer += 3;
			}
		}
	}
	else
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 3)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 3)];

			memcpy(destination_pointer, source_pointer, (rect_clamped.right - rect_clamped.left) * 3);
		}
	}
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	Backend_Blit(source_surface, rect, &framebuffer, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect so it doesn't write outside the pixel buffer
	long overflow;

	overflow = 0 - rect_clamped.left;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
	}

	overflow = 0 - rect_clamped.top;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
	}

	overflow = rect_clamped.right - surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = rect_clamped.bottom - surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
	{
		unsigned char *source_pointer = &surface->pixels[((rect_clamped.top + j) * surface->pitch) + (rect_clamped.left * 3)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			*source_pointer++ = red;
			*source_pointer++ = green;
			*source_pointer++ = blue;
		}
	}
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
	DrawText(font, surface->pixels, surface->pitch, surface->width, surface->height, x, y, colour, text, strlen(text));
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
