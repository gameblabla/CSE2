#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct Backend_Surface
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
} Backend_Surface;

typedef struct Backend_Glyph
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	int pitch;
	unsigned short total_greys;
	unsigned char pixel_mode;
} Backend_Glyph;

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

unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch)
{
	*pitch = surface->pitch;
	return surface->pixels;
}

void Backend_Unlock(Backend_Surface *surface)
{
	
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

	if (rect_clamped.bottom - rect_clamped.top <= 0)
		return;

	if (rect_clamped.right - rect_clamped.left <= 0)
		return;

	// Do the actual blitting
	if (colour_key)
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 3)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 3)];

			for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
			{
				if (source_pointer[0] == 0 && source_pointer[1] == 0 && source_pointer[2] == 0)	// Assumes the colour key will always be #000000 (black)
				{
					source_pointer += 3;
					destination_pointer += 3;
				}
				else
				{
					*destination_pointer++ = *source_pointer++;
					*destination_pointer++ = *source_pointer++;
					*destination_pointer++ = *source_pointer++;
				}
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

	if (rect_clamped.bottom - rect_clamped.top <= 0)
		return;

	if (rect_clamped.right - rect_clamped.left <= 0)
		return;

	for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
	{
		unsigned char *destination_pointer = &surface->pixels[((rect_clamped.top + j) * surface->pitch) + (rect_clamped.left * 3)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			*destination_pointer++ = red;
			*destination_pointer++ = green;
			*destination_pointer++ = blue;
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

BOOL Backend_SupportsSubpixelGlyph(void)
{
	return TRUE;	// It's a software renderer, baby
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned short total_greys, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->pixels = (unsigned char*)malloc(pitch * height);

	if (glyph->pixels == NULL)
	{
		free(glyph);
		return NULL;
	}

	memcpy(glyph->pixels, pixels, pitch * height);

	glyph->width = width;
	glyph->height = height;
	glyph->pitch = pitch;
	glyph->total_greys = total_greys;
	glyph->pixel_mode = pixel_mode;

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	free(glyph->pixels);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	switch (glyph->pixel_mode)
	{
		case FONT_PIXEL_MODE_LCD:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width / 3, surface->width); ++ix)
				{
					const unsigned char *font_pixel = glyph->pixels + iy * glyph->pitch + ix * 3;

					if (font_pixel[0] || font_pixel[1] || font_pixel[2])
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 3;

						for (unsigned int j = 0; j < 3; ++j)
						{
							const double alpha = pow((font_pixel[j] / 255.0), 1.0 / 1.8);			// Gamma correction
							bitmap_pixel[j] = (unsigned char)((colours[j] * alpha) + (bitmap_pixel[j] * (1.0 - alpha)));	// Alpha blending
						}
					}
				}
			}

			break;

		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					const unsigned char font_pixel = glyph->pixels[iy * glyph->pitch + ix];

					if (font_pixel)
					{
						const double alpha = pow((double)font_pixel / (glyph->total_greys - 1), 1.0 / 1.8);			// Gamma-corrected

						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 3;

						for (unsigned int j = 0; j < 3; ++j)
							bitmap_pixel[j] = (unsigned char)((colours[j] * alpha) + (bitmap_pixel[j] * (1.0 - alpha)));	// Alpha blending
					}
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					if (glyph->pixels[iy * glyph->pitch + ix])
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 3;

						for (unsigned int j = 0; j < 3; ++j)
							bitmap_pixel[j] = colours[j];
					}
				}
			}

			break;
	}
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
