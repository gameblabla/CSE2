#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

typedef struct Backend_Surface
{
	BOOL alpha;
	SDL_Texture *texture;
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
} Backend_Surface;

typedef struct Backend_Glyph
{
	Backend_Surface *surface;
} Backend_Glyph;

static SDL_Renderer *renderer;

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

Backend_Surface* Backend_Init(SDL_Window *window)
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (renderer == NULL)
		return FALSE;

	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);
	framebuffer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

	return &framebuffer;
}

void Backend_Deinit(void)
{
	SDL_DestroyTexture(framebuffer.texture);
	SDL_DestroyRenderer(renderer);
}

void Backend_DrawScreen(void)
{
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, framebuffer.texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

static Backend_Surface* CreateSurface(unsigned int width, unsigned int height, BOOL alpha)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

	if (surface->texture == NULL)
	{
		free(surface);
		return NULL;
	}

	surface->alpha = alpha;
	surface->width = width;
	surface->height = height;

	return surface;
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	return CreateSurface(width, height, FALSE);
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	SDL_DestroyTexture(surface->texture);
	free(surface);
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->width * (surface->alpha ? 4 : 3);

	surface->pixels = (unsigned char*)malloc(surface->width * surface->height * (surface->alpha ? 4 : 3));

	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	SDL_UnlockTexture(surface->texture);

	if (surface->alpha)
	{
		SDL_UpdateTexture(surface->texture, NULL, surface->pixels, surface->width * 4);
	}
	else
	{
		unsigned char *buffer = (unsigned char*)malloc(surface->width * surface->height * 4);
		unsigned char *buffer_pointer = buffer;
		const unsigned char *src_pixel = surface->pixels;

		// Convert the SDL_Surface's colour-keyed pixels to RGBA32
		for (int y = 0; y < surface->height; ++y)
		{

			for (int x = 0; x < surface->width; ++x)
			{
				*buffer_pointer++ = src_pixel[0];
				*buffer_pointer++ = src_pixel[1];
				*buffer_pointer++ = src_pixel[2];

				if (src_pixel[0] == 0 && src_pixel[1] == 0 && src_pixel[2] == 0)	// Assumes the colour key will always be #000000 (black)
					*buffer_pointer++ = 0;
				else
					*buffer_pointer++ = 0xFF;

				src_pixel += 3;
			}
		}

		SDL_UpdateTexture(surface->texture, NULL, buffer, surface->width * 4);

		free(buffer);
	}

	free(surface->pixels);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {(int)x, (int)y, source_rect.w, source_rect.h};

	// Blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(renderer, destination_surface->texture);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	// Check colour-key
	if (red == 0 && green == 0 && blue == 0)
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	else
		SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);

	// Draw colour
	SDL_SetRenderTarget(renderer, surface->texture);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderFillRect(renderer, &sdl_rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

BOOL Backend_SupportsSubpixelGlyph(void)
{
	return FALSE;	// SDL_Textures don't have per-component alpha
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->surface = CreateSurface(width, height, TRUE);

	if (glyph->surface == NULL)
	{
		free(glyph);
		return NULL;
	}

	unsigned int surface_pitch;
	unsigned char *surface_pixels = Backend_LockSurface(glyph->surface, &surface_pitch);

	switch (pixel_mode)
	{
		// FONT_PIXEL_MODE_LCD is unsupported

		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = surface_pixels + y * surface_pitch;

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
				unsigned char *destination_pointer = surface_pixels + y * surface_pitch;

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

	Backend_UnlockSurface(glyph->surface);

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	Backend_FreeSurface(glyph->surface);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	// The SDL_Texture side of things uses alpha, not a colour-key, so the bug where the font is blended
	// with the colour key doesn't occur.

	if (glyph == NULL || surface == NULL)
		return;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = glyph->surface->width;
	rect.bottom = glyph->surface->height;

	SDL_SetTextureColorMod(glyph->surface->texture, colours[0], colours[1], colours[2]);

	Backend_Blit(glyph->surface, &rect, surface, x, y, TRUE);
}

void Backend_HandleDeviceLoss(void)
{
	// All of our textures have been lost, so regenerate them
	// TODO
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
