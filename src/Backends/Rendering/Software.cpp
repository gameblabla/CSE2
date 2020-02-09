#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Resource.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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
} Backend_Glyph;

static SDL_Window *window;
static SDL_Surface *window_sdlsurface;
static SDL_Surface *framebuffer_sdlsurface;
static Backend_Surface framebuffer;

static unsigned char glyph_colour_channels[3];
static Backend_Surface *glyph_destination_surface;

Backend_Surface* Backend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen)
{
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
	#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
		size_t resource_size;
		const unsigned char *resource_data = FindResource("ICON_MINI", "ICON", &resource_size);
		SDL_RWops *rwops = SDL_RWFromConstMem(resource_data, resource_size);
		SDL_Surface *icon_surface = SDL_LoadBMP_RW(rwops, 1);
		SDL_SetWindowIcon(window, icon_surface);
		SDL_FreeSurface(icon_surface);
	#endif

		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		window_sdlsurface = SDL_GetWindowSurface(window);

		framebuffer_sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

		if (framebuffer_sdlsurface != NULL)
		{
			framebuffer.pixels = (unsigned char*)framebuffer_sdlsurface->pixels;
			framebuffer.width = framebuffer_sdlsurface->w;
			framebuffer.height = framebuffer_sdlsurface->h;
			framebuffer.pitch = framebuffer_sdlsurface->pitch;

			return &framebuffer;
		}
		else
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (software rendering backend)", "Could not create framebuffer surface", window);
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (software rendering backend)", "Could not create window", NULL);
	}

	return NULL;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(framebuffer_sdlsurface);
	SDL_DestroyWindow(window);
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

BOOL Backend_IsSurfaceLost(Backend_Surface *surface)
{
	(void)surface;

	return FALSE;
}

void Backend_RestoreSurface(Backend_Surface *surface)
{
	(void)surface;
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	if (surface == NULL)
		return NULL;

	*pitch = surface->pitch;
	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface, unsigned int width, unsigned int height)
{
	(void)surface;
	(void)width;
	(void)height;
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

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->pixels = (unsigned char*)malloc(width * height);

	if (glyph->pixels == NULL)
	{
		free(glyph);
		return NULL;
	}

	unsigned char *destination_pointer = glyph->pixels;

	for (unsigned int y = 0; y < height; ++y)
		memcpy(&glyph->pixels[y * width], &pixels[y * pitch], width);

	glyph->width = width;
	glyph->height = height;

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	free(glyph->pixels);
	free(glyph);
}

void Backend_PrepareToDrawGlyphs(Backend_Surface *destination_surface, const unsigned char *colour_channels)
{
	if (destination_surface == NULL)
		return;

	glyph_destination_surface = destination_surface;

	memcpy(glyph_colour_channels, colour_channels, sizeof(glyph_colour_channels));
}

void Backend_DrawGlyph(Backend_Glyph *glyph, long x, long y)
{
	if (glyph == NULL)
		return;

	for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, glyph_destination_surface->height); ++iy)
	{
		for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, glyph_destination_surface->width); ++ix)
		{
			const unsigned char alpha_int = glyph->pixels[iy * glyph->width + ix];

			if (alpha_int != 0)
			{
				const float alpha = alpha_int / 255.0f;

				unsigned char *bitmap_pixel = glyph_destination_surface->pixels + (y + iy) * glyph_destination_surface->pitch + (x + ix) * 3;

				for (unsigned int j = 0; j < 3; ++j)
					bitmap_pixel[j] = (unsigned char)((glyph_colour_channels[j] * alpha) + (bitmap_pixel[j] * (1.0f - alpha)));	// Alpha blending
			}
		}
	}
}

void Backend_FlushGlyphs(void)
{
	
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
