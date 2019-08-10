#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

typedef struct Backend_Surface
{
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
static SDL_Texture *texture;

static Backend_Surface *surface_list_head;

static SDL_BlendMode premultiplied_blend_mode;

static void FlushSurface(Backend_Surface *surface)
{
	SDL_UpdateTexture(surface->texture, NULL, surface->sdl_surface->pixels, surface->sdl_surface->pitch);
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
	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
}

BOOL Backend_Init(SDL_Window *window, unsigned int internal_screen_width, unsigned int internal_screen_height, BOOL vsync)
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

	if (renderer == NULL)
		return FALSE;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, internal_screen_width, internal_screen_height);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (texture == NULL)
	{
		SDL_DestroyRenderer(renderer);
		return FALSE;
	}

	SDL_SetRenderTarget(renderer, texture);

	// Set up our premultiplied-alpha blend mode
	premultiplied_blend_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD);

	return TRUE;
}

void Backend_Deinit(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
}

void Backend_DrawScreen(void)
{
	SDL_SetRenderTarget(renderer, NULL);

	int renderer_width, renderer_height;
	SDL_GetRendererOutputSize(renderer, &renderer_width, &renderer_height);

	int texture_width, texture_height;
	SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

	SDL_Rect dst_rect;
	if ((float)renderer_width / texture_width < (float)renderer_height / texture_height)
	{
		dst_rect.w = renderer_width;
		dst_rect.h = (int)(texture_height * (float)renderer_width / texture_width);
		dst_rect.x = 0;
		dst_rect.y = (renderer_height - dst_rect.h) / 2;
	}
	else
	{
		dst_rect.w = (int)(texture_width * (float)renderer_height / texture_height);
		dst_rect.h = renderer_height;
		dst_rect.x = (renderer_width - dst_rect.w) / 2;
		dst_rect.y = 0;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, &dst_rect);

	SDL_RenderPresent(renderer);

	SDL_SetRenderTarget(renderer, texture);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

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

	surface->needs_syncing = FALSE;

	surface->next = surface_list_head;
	surface->prev = NULL;
	surface_list_head = surface;
	if (surface->next)
		surface->next->prev = surface;

	return surface;
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

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->sdl_surface->pitch;
	return (unsigned char*)surface->sdl_surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	// Pre-multiply the colour channels with the alpha, so blending works correctly
	for (int y = 0; y < surface->sdl_surface->h; ++y)
	{
		unsigned char *pixels = (unsigned char*)surface->sdl_surface->pixels + y * surface->sdl_surface->pitch;

		for (int x = 0; x < surface->sdl_surface->w; ++x)
		{
			pixels[0] = (pixels[0] * pixels[3]) / 0xFF;
			pixels[1] = (pixels[1] * pixels[3]) / 0xFF;
			pixels[2] = (pixels[2] * pixels[3]) / 0xFF;
			pixels += 4;
		}
	}

	surface->needs_syncing = TRUE;
}

void Backend_BlitToSurface(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y)
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
	SDL_SetSurfaceBlendMode(source_surface->sdl_surface, SDL_BLENDMODE_BLEND);
	SDL_BlitSurface(source_surface->sdl_surface, &source_rect, destination_surface->sdl_surface, &destination_rect);

	// Now blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, premultiplied_blend_mode);
	SDL_Texture *default_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, destination_surface->texture);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
	SDL_SetRenderTarget(renderer, default_target);
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL alpha_blend)
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
	SDL_SetTextureBlendMode(source_surface->texture, alpha_blend ? premultiplied_blend_mode : SDL_BLENDMODE_NONE);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
}

void Backend_ColourFillToSurface(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	// Blit the surface
	SDL_FillRect(surface->sdl_surface, &sdl_rect, SDL_MapRGBA(surface->sdl_surface->format, red, green, blue, alpha));

	// Draw colour
	SDL_SetRenderDrawColor(renderer, (red * alpha) / 0xFF, (green * alpha) / 0xFF, (blue * alpha) / 0xFF, alpha);
	SDL_Texture *default_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, surface->texture);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderFillRect(renderer, &sdl_rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, default_target);
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
	SDL_Surface *screen_surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, SDL_PIXELFORMAT_RGBA32);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, screen_surface->pixels, screen_surface->pitch);

	// Copy to specified surface
	SDL_BlitSurface(screen_surface, &sdl_rect, surface->sdl_surface, &sdl_rect);

	// Cleanup
	SDL_FreeSurface(screen_surface);

	//
	// Copy screen to texture
	//

	SDL_Texture *default_target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, surface->texture);
	SDL_RenderCopy(renderer, default_target, &sdl_rect, &sdl_rect);
	SDL_SetRenderTarget(renderer, default_target);
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->surface = Backend_CreateSurface(width, height);

	if (glyph->surface == NULL)
	{
		free(glyph);
		return NULL;
	}

	unsigned int surface_pitch;
	unsigned char *surface_pixels = Backend_LockSurface(glyph->surface, &surface_pitch);

	switch (pixel_mode)
	{
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

void Backend_DrawGlyphToSurface(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = glyph->surface->sdl_surface->w;
	rect.bottom = glyph->surface->sdl_surface->h;

	SDL_SetSurfaceColorMod(glyph->surface->sdl_surface, colours[0], colours[1], colours[2]);
	SDL_SetTextureColorMod(glyph->surface->texture, colours[0], colours[1], colours[2]);

	Backend_BlitToSurface(glyph->surface, &rect, surface, x, y);
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
