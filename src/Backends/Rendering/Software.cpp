#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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
	void *pixels;
	unsigned int width;
	unsigned int height;
	FontPixelMode pixel_mode;
} Backend_Glyph;

static SDL_Window *window;
static SDL_Surface *window_sdlsurface;
static SDL_Surface *framebuffer_sdlsurface;
static Backend_Surface framebuffer;

SDL_Window* Backend_CreateWindow(const char *title, int width, int height)
{
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
}

Backend_Surface* Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	window_sdlsurface = SDL_GetWindowSurface(window);

	framebuffer_sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

	if (framebuffer_sdlsurface == NULL)
		return NULL;

	framebuffer.pixels = (unsigned char*)framebuffer_sdlsurface->pixels;
	framebuffer.width = framebuffer_sdlsurface->w;
	framebuffer.height = framebuffer_sdlsurface->h;
	framebuffer.pitch = framebuffer_sdlsurface->pitch;

	return &framebuffer;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(framebuffer_sdlsurface);
}

void Backend_DrawScreen(void)
{
	SDL_BlitSurface(framebuffer_sdlsurface, NULL, window_sdlsurface, NULL);
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
	if (surface == NULL)
		return;

	free(surface->pixels);
	free(surface);
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->pitch;
	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	(void)surface;
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

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

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

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

BOOL Backend_SupportsSubpixelGlyphs(void)
{
	return TRUE;	// It's a software renderer, baby
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, FontPixelMode pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	switch (pixel_mode)
	{
		case FONT_PIXEL_MODE_LCD:
		case FONT_PIXEL_MODE_GRAY:
		{
			glyph->pixels = malloc(width * height * sizeof(float));

			if (glyph->pixels == NULL)
			{
				free(glyph);
				return NULL;
			}

			float *destination_pointer = (float*)glyph->pixels;

			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;

				for (unsigned int x = 0; x < width; ++x)
					*destination_pointer++ = *source_pointer++ / 255.0f;
			}

			break;
		}

		case FONT_PIXEL_MODE_MONO:
		{
			glyph->pixels = malloc(width * height);

			if (glyph->pixels == NULL)
			{
				free(glyph);
				return NULL;
			}

			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->pixels + y * width;

				memcpy(destination_pointer, source_pointer, width);
			}

			break;
		}
	}

	glyph->width = (pixel_mode == FONT_PIXEL_MODE_LCD ? width / 3 : width);
	glyph->height = height;
	glyph->pixel_mode = pixel_mode;

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	free(glyph->pixels);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	switch (glyph->pixel_mode)
	{
		case FONT_PIXEL_MODE_LCD:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					const float *font_pixel = (float*)glyph->pixels + (iy * glyph->width + ix) * 3;

					if (font_pixel[0] || font_pixel[1] || font_pixel[2])
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 3;

						for (unsigned int j = 0; j < 3; ++j)
						{
							const float alpha = font_pixel[j];
							bitmap_pixel[j] = (unsigned char)((colours[j] * alpha) + (bitmap_pixel[j] * (1.0f - alpha)));	// Alpha blending
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
					const float alpha = ((float*)glyph->pixels)[iy * glyph->width + ix];

					if (alpha)
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 3;

						for (unsigned int j = 0; j < 3; ++j)
							bitmap_pixel[j] = (unsigned char)((colours[j] * alpha) + (bitmap_pixel[j] * (1.0f - alpha)));	// Alpha blending
					}
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					if (((unsigned char*)glyph->pixels)[iy * glyph->width + ix])
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

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);
}
