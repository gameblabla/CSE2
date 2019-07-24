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
	if (surface->next)
		surface->next->prev = surface->prev;
	if (surface->prev)
		surface->prev->next = surface->next;

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
	surface->needs_syncing = TRUE;
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface->needs_syncing)
	{
		FlushSurface(source_surface);
		source_surface->needs_syncing = FALSE;
	}

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {x, y, source_rect.w, source_rect.h};

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
	if (source_surface->needs_syncing)
	{
		FlushSurface(source_surface);
		source_surface->needs_syncing = FALSE;
	}

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {x, y, source_rect.w, source_rect.h};

	// Blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
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
					*destination_pointer++ = (unsigned char)(pow((double)*source_pointer++ / (total_greys - 1), 1.0 / 1.8) * 255.0);
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
	Backend_FreeSurface(glyph->surface);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	// This is actually slightly imperfect: the SDL_Texture side of things uses alpha, not a colour-key,
	// so the bug where the font is blended with the colour key doesn't occur. SDL_Textures don't support
	// colour-keys, so the next best thing is relying on the software fallback, but I don't like the idea
	// of uploading textures to the GPU every time a glyph is drawn.

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = glyph->surface->sdl_surface->w;
	rect.bottom = glyph->surface->sdl_surface->h;

	SDL_SetSurfaceColorMod(glyph->surface->sdl_surface, colours[0], colours[1], colours[2]);
	SDL_SetTextureColorMod(glyph->surface->texture, colours[0], colours[1], colours[2]);

	Backend_Blit(glyph->surface, &rect, surface, x, y, TRUE);
}

void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
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
