#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Draw.h"
#include "../../Ending.h"
#include "../../MapName.h"
#include "../../TextScr.h"

typedef struct Backend_Surface
{
	SDL_Texture *texture;
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
} Backend_Surface;

typedef struct Backend_Glyph
{
	Backend_Surface *surface;
	unsigned char pixel_mode;
} Backend_Glyph;

static SDL_Renderer *renderer;

static Backend_Surface framebuffer;

static SDL_BlendMode premultiplied_blend_mode;

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

Backend_Surface* Backend_Init(SDL_Window *window, unsigned int internal_screen_width, unsigned int internal_screen_height, BOOL vsync)
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

	if (renderer == NULL)
		return NULL;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	framebuffer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, internal_screen_width, internal_screen_height);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (framebuffer.texture == NULL)
	{
		SDL_DestroyRenderer(renderer);
		return NULL;
	}

	framebuffer.width = internal_screen_width;
	framebuffer.height = internal_screen_height;

	// Set up our premultiplied-alpha blend mode
	premultiplied_blend_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD);

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

	int renderer_width, renderer_height;
	SDL_GetRendererOutputSize(renderer, &renderer_width, &renderer_height);

	int texture_width, texture_height;
	SDL_QueryTexture(framebuffer.texture, NULL, NULL, &texture_width, &texture_height);

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
	SDL_RenderCopy(renderer, framebuffer.texture, NULL, &dst_rect);

	SDL_RenderPresent(renderer);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
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

	surface->width = width;
	surface->height = height;

	return surface;
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

	*pitch = surface->width * 4;

	surface->pixels = (unsigned char*)malloc(surface->width * surface->height * 4);

	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	// Pre-multiply the colour channels with the alpha, so blending works correctly
	for (unsigned int y = 0; y < surface->height; ++y)
	{
		unsigned char *pixels = surface->pixels + y * surface->width * 4;

		for (unsigned int x = 0; x < surface->width; ++x)
		{
			pixels[0] = (pixels[0] * pixels[3]) / 0xFF;
			pixels[1] = (pixels[1] * pixels[3]) / 0xFF;
			pixels[2] = (pixels[2] * pixels[3]) / 0xFF;
			pixels += 4;
		}
	}

	SDL_UpdateTexture(surface->texture, NULL, surface->pixels, surface->width * 4);

	free(surface->pixels);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {(int)x, (int)y, source_rect.w, source_rect.h};

	// Blit the texture
	SDL_SetTextureBlendMode(source_surface->texture, alpha_blend ? premultiplied_blend_mode : SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(renderer, destination_surface->texture);
	SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	// Draw colour
	SDL_SetRenderDrawColor(renderer, (red * alpha) / 0xFF, (green * alpha) / 0xFF, (blue * alpha) / 0xFF, alpha);
	SDL_SetRenderTarget(renderer, surface->texture);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderFillRect(renderer, &sdl_rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

Backend_Glyph* Backend_CreateGlyph(unsigned int width, unsigned int height, unsigned char pixel_mode)
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

	glyph->pixel_mode = pixel_mode;

	return glyph;
}

void Backend_FreeGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	Backend_FreeSurface(glyph->surface);
	free(glyph);
}

void Backend_LoadGlyphPixels(Backend_Glyph *glyph, const unsigned char *pixels, int pitch)
{
	if (glyph == NULL)
		return;

	unsigned int surface_pitch;
	unsigned char *surface_pixels = Backend_LockSurface(glyph->surface, &surface_pitch);

	switch (glyph->pixel_mode)
	{
		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < glyph->surface->height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = surface_pixels + y * surface_pitch;

				for (unsigned int x = 0; x < glyph->surface->width; ++x)
				{
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = 0xFF;
					*destination_pointer++ = *source_pointer++;
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int y = 0; y < glyph->surface->height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = surface_pixels + y * surface_pitch;

				for (unsigned int x = 0; x < glyph->surface->width; ++x)
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
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
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
	RestoreSurfaces();
	RestoreStripper();
	RestoreMapName();
	RestoreTextScript();
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
