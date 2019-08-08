#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

typedef struct Backend_Surface
{
	BOOL alpha;
	BOOL needs_syncing;
	SDL_Surface *sdl_surface;
	SDL_Texture *texture;

	struct Backend_Surface *next;
	struct Backend_Surface *prev;
} Backend_Surface;

typedef struct Backend_Glyph
{
	Backend_Surface *surface;
} Backend_Glyph;

static SDL_Renderer *renderer;
static SDL_Texture *screen_texture;

static Backend_Surface *surface_list_head;

static void FlushSurface(Backend_Surface *surface)
{
	if (surface->alpha)
	{
		SDL_UpdateTexture(surface->texture, NULL, surface->sdl_surface->pixels, surface->sdl_surface->pitch);
	}
	else
	{
		unsigned char *buffer = (unsigned char*)malloc(surface->sdl_surface->w * surface->sdl_surface->h * 4);
		unsigned char *buffer_pointer = buffer;

		// Convert the SDL_Surface's colour-keyed pixels to RGBA32
		for (int y = 0; y < surface->sdl_surface->h; ++y)
		{
			unsigned char *src_pixel = (unsigned char*)surface->sdl_surface->pixels + (y * surface->sdl_surface->pitch);

			for (int x = 0; x < surface->sdl_surface->w; ++x)
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

		SDL_UpdateTexture(surface->texture, NULL, buffer, surface->sdl_surface->w * 4);

		free(buffer);
	}
}

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

static unsigned char GammaCorrect(unsigned char value)
{
	/*
		Generated with...
		for (unsigned int i = 0; i < 0x100; ++i)
			lookup[i] = pow(i / 255.0, 1.0 / 1.8) * 255.0;
	*/

	const unsigned char lookup[0x100] = {
		0x00, 0x0B, 0x11, 0x15, 0x19, 0x1C, 0x1F, 0x22, 0x25, 0x27, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34,
		0x36, 0x38, 0x3A, 0x3C, 0x3D, 0x3F, 0x41, 0x43, 0x44, 0x46, 0x47, 0x49, 0x4A, 0x4C, 0x4D, 0x4F,
		0x50, 0x51, 0x53, 0x54, 0x55, 0x57, 0x58, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x62, 0x63,
		0x64, 0x65, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
		0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x84,
		0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93,
		0x94, 0x95, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0x9F, 0xA0,
		0xA1, 0xA2, 0xA3, 0xA3, 0xA4, 0xA5, 0xA6, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAA, 0xAB, 0xAC, 0xAD,
		0xAD, 0xAE, 0xAF, 0xB0, 0xB0, 0xB1, 0xB2, 0xB3, 0xB3, 0xB4, 0xB5, 0xB6, 0xB6, 0xB7, 0xB8, 0xB8,
		0xB9, 0xBA, 0xBB, 0xBB, 0xBC, 0xBD, 0xBD, 0xBE, 0xBF, 0xBF, 0xC0, 0xC1, 0xC2, 0xC2, 0xC3, 0xC4,
		0xC4, 0xC5, 0xC6, 0xC6, 0xC7, 0xC8, 0xC8, 0xC9, 0xCA, 0xCA, 0xCB, 0xCC, 0xCC, 0xCD, 0xCE, 0xCE,
		0xCF, 0xD0, 0xD0, 0xD1, 0xD2, 0xD2, 0xD3, 0xD4, 0xD4, 0xD5, 0xD6, 0xD6, 0xD7, 0xD7, 0xD8, 0xD9,
		0xD9, 0xDA, 0xDB, 0xDB, 0xDC, 0xDC, 0xDD, 0xDE, 0xDE, 0xDF, 0xE0, 0xE0, 0xE1, 0xE1, 0xE2, 0xE3,
		0xE3, 0xE4, 0xE4, 0xE5, 0xE6, 0xE6, 0xE7, 0xE7, 0xE8, 0xE9, 0xE9, 0xEA, 0xEA, 0xEB, 0xEC, 0xEC,
		0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF0, 0xF1, 0xF1, 0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5,
		0xF6, 0xF7, 0xF7, 0xF8, 0xF8, 0xF9, 0xF9, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC, 0xFD, 0xFD, 0xFE, 0xFF
	}

	return lookup[value];
}

SDL_Window* Backend_CreateWindow(const char *title, int width, int height)
{
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
}

BOOL Backend_Init(SDL_Window *window)
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (renderer == NULL)
		return FALSE;

	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);
	screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_SetRenderTarget(renderer, screen_texture);

	return TRUE;
}

void Backend_Deinit(void)
{
	SDL_DestroyTexture(screen_texture);
	SDL_DestroyRenderer(renderer);
}

void Backend_DrawScreen(void)
{
	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
	SDL_SetRenderTarget(renderer, screen_texture);
	SDL_RenderPresent(renderer);
}

static Backend_Surface* CreateSurface(unsigned int width, unsigned int height, BOOL alpha)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, alpha ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24);

	if (surface->sdl_surface == NULL)
	{
		free(surface);
		return NULL;
	}

	surface->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);

	if (surface->texture == NULL)
	{
		SDL_FreeSurface(surface->sdl_surface);
		free(surface);
		return NULL;
	}

	surface->alpha = alpha;

	if (!surface->alpha)
		SDL_SetColorKey(surface->sdl_surface, SDL_TRUE, SDL_MapRGB(surface->sdl_surface->format, 0, 0, 0));

	surface->needs_syncing = FALSE;

	surface->next = surface_list_head;
	surface->prev = NULL;
	surface_list_head = surface;
	if (surface->next)
		surface->next->prev = surface;

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

	if (surface->next)
		surface->next->prev = surface->prev;
	if (surface->prev)
		surface->prev->next = surface->next;

	SDL_DestroyTexture(surface->texture);
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
	if (surface == NULL)
		return;

	surface->needs_syncing = TRUE;
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	if (source_surface->needs_syncing)
	{
		FlushSurface(source_surface);
		source_surface->needs_syncing = FALSE;
	}

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {(int)x, (int)y, source_rect.w, source_rect.h};

	// Blit the surface
	SDL_SetSurfaceBlendMode(source_surface->sdl_surface, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_BlitSurface(source_surface->sdl_surface, &source_rect, destination_surface->sdl_surface, &destination_rect);

	// Now blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(renderer, destination_surface->texture);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
	SDL_SetRenderTarget(renderer, screen_texture);
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL)
		return;

	if (source_surface->needs_syncing)
	{
		FlushSurface(source_surface);
		source_surface->needs_syncing = FALSE;
	}

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {(int)x, (int)y, source_rect.w, source_rect.h};

	// Blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	// Blit the surface
	SDL_FillRect(surface->sdl_surface, &sdl_rect, SDL_MapRGB(surface->sdl_surface->format, red, green, blue));

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
	SDL_SetRenderTarget(renderer, screen_texture);
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	// Draw colour
	SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderFillRect(renderer, &sdl_rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	//
	// Copy screen to surface
	//

	// Get renderer size
	int w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);

	// Get surface of what's currently rendered on screen
	SDL_Surface *screen_surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGB24);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGB24, screen_surface->pixels, screen_surface->pitch);

	// Copy to specified surface
	SDL_BlitSurface(screen_surface, &sdl_rect, surface->sdl_surface, &sdl_rect);

	// Cleanup
	SDL_FreeSurface(screen_surface);

	//
	// Copy screen to texture
	//

	SDL_SetRenderTarget(renderer, surface->texture);
	SDL_RenderCopy(renderer, screen_texture, &sdl_rect, &sdl_rect);
	SDL_SetRenderTarget(renderer, screen_texture);
}

BOOL Backend_SupportsSubpixelGlyph(void)
{
	return FALSE;	// SDL_Textures don't have per-component alpha
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned short total_greys, unsigned char pixel_mode)
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
	unsigned char *surface_pixels = Backend_Lock(glyph->surface, &surface_pitch);

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
					*destination_pointer++ = GammaCorrect((*source_pointer++ * 0xFF) / (total_greys - 1));
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

	Backend_Unlock(glyph->surface);

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
	// with the colour key doesn't occur. SDL_Textures don't support colour-keys, so the next best thing
	// is relying on the software fallback.

	if (glyph == NULL || surface == NULL)
		return;

	SDL_Rect destination_rect = {(int)x, (int)y, glyph->surface->sdl_surface->w, glyph->surface->sdl_surface->h};

	SDL_SetSurfaceColorMod(glyph->surface->sdl_surface, colours[0], colours[1], colours[2]);
	SDL_SetSurfaceBlendMode(glyph->surface->sdl_surface, SDL_BLENDMODE_BLEND);
	SDL_BlitSurface(glyph->surface->sdl_surface, NULL, surface->sdl_surface, &destination_rect);

	surface->needs_syncing = TRUE;
}

void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL)
		return;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = glyph->surface->sdl_surface->w;
	rect.bottom = glyph->surface->sdl_surface->h;

	SDL_SetSurfaceColorMod(glyph->surface->sdl_surface, colours[0], colours[1], colours[2]);
	SDL_SetTextureColorMod(glyph->surface->texture, colours[0], colours[1], colours[2]);

	Backend_BlitToScreen(glyph->surface, &rect, x, y, TRUE);
}

void Backend_HandleDeviceLoss(void)
{
	// All of our textures have been lost, so regenerate them
	for (Backend_Surface *surface = surface_list_head; surface != NULL; surface = surface->next)
		surface->needs_syncing = TRUE;
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
