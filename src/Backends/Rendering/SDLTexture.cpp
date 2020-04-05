#include "../Rendering.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#define SPRITEBATCH_IMPLEMENTATION
#include "../../../external/cute_spritebatch.h"

#include "../../WindowsWrapper.h"

#include "../Misc.h"
#include "../SDL2/Window.h"
#include "../../Draw.h"
#include "../../Ending.h"
#include "../../MapName.h"
#include "../../TextScr.h"

typedef struct RenderBackend_Surface
{
	SDL_Texture *texture;
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	BOOL lost;

	struct RenderBackend_Surface *next;
	struct RenderBackend_Surface *prev;
} RenderBackend_Surface;

typedef struct RenderBackend_Glyph
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
} RenderBackend_Glyph;

SDL_Window *window;

static SDL_Renderer *renderer;

static RenderBackend_Surface framebuffer;

static RenderBackend_Surface *surface_list_head;

static unsigned char glyph_colour_channels[3];

static spritebatch_t glyph_batcher;

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

// Blit the glyphs in the batch
static void GlyphBatch_Draw(spritebatch_sprite_t *sprites, int count, int texture_w, int texture_h, void *udata)
{
	(void)udata;

	SDL_Texture *texture_atlas = (SDL_Texture*)sprites[0].texture_id;

	// The SDL_Texture side of things uses alpha, not a colour-key, so the bug where the font is blended
	// with the colour key doesn't occur.
	SDL_SetTextureColorMod(texture_atlas, glyph_colour_channels[0], glyph_colour_channels[1], glyph_colour_channels[2]);
	SDL_SetTextureBlendMode(texture_atlas, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < count; ++i)
	{
		RenderBackend_Glyph *glyph = (RenderBackend_Glyph*)sprites[i].image_id;

		SDL_Rect source_rect = {(int)(texture_w * sprites[i].minx), (int)(texture_h * sprites[i].maxy), (int)glyph->width, (int)glyph->height};
		SDL_Rect destination_rect = {(int)sprites[i].x, (int)sprites[i].y, (int)glyph->width, (int)glyph->height};

		SDL_RenderCopy(renderer, texture_atlas, &source_rect, &destination_rect);
	}
}

// Upload the glyph's pixels
static void GlyphBatch_GetPixels(SPRITEBATCH_U64 image_id, void *buffer, int bytes_to_fill, void *udata)
{
	(void)udata;

	RenderBackend_Glyph *glyph = (RenderBackend_Glyph*)image_id;

	memcpy(buffer, glyph->pixels, bytes_to_fill);
}

// Create a texture atlas, and upload pixels to it
static SPRITEBATCH_U64 GlyphBatch_CreateTexture(void *pixels, int w, int h, void *udata)
{
	(void)udata;

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);

	SDL_UpdateTexture(texture, NULL, pixels, w * 4);

	return (SPRITEBATCH_U64)texture;
}

// Destroy texture atlas
static void GlyphBatch_DestroyTexture(SPRITEBATCH_U64 texture_id, void *udata)
{
	(void)udata;

	SDL_DestroyTexture((SDL_Texture*)texture_id);
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen, BOOL vsync)
{
	puts("Available SDL2 render drivers:");

	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(i, &info);
		puts(info.name);
	}

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_RESIZABLE);

	if (window != NULL)
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	#if SDL_VERSION_ATLEAST(2,0,10)
		SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");	// We never interfere with the renderer, so don't let SDL implicitly disable batching
	#endif

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

		if (renderer != NULL)
		{
			SDL_RendererInfo info;
			SDL_GetRendererInfo(renderer, &info);
			printf("Selected SDL2 render driver: %s\n", info.name);

			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
			framebuffer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, screen_width, screen_height);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

			if (framebuffer.texture != NULL)
			{
				framebuffer.width = screen_width;
				framebuffer.height = screen_height;

				// Set up our premultiplied-alpha blend mode
				premultiplied_blend_mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA, SDL_BLENDOPERATION_ADD);

				// Set-up glyph-batcher
				spritebatch_config_t config;
				spritebatch_set_default_config(&config);
				config.pixel_stride = 4;
				config.atlas_width_in_pixels = 256;
				config.atlas_height_in_pixels = 256;
				config.lonely_buffer_count_till_flush = 4; // Start making atlases immediately
				config.batch_callback = GlyphBatch_Draw;
				config.get_pixels_callback = GlyphBatch_GetPixels;
				config.generate_texture_callback = GlyphBatch_CreateTexture;
				config.delete_texture_callback = GlyphBatch_DestroyTexture;
				spritebatch_init(&glyph_batcher, &config, NULL);

				Backend_PostWindowCreation();

				return &framebuffer;
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (SDLTexture rendering backend)", "Could not create framebuffer", window);
			}

			SDL_DestroyRenderer(renderer);
		}
		else
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (SDLTexture rendering backend)", "Could not create renderer", window);
		}


		SDL_DestroyWindow(window);
	}
	else
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (SDLTexture rendering backend)", "Could not create window", NULL);
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	spritebatch_term(&glyph_batcher);
	SDL_DestroyTexture(framebuffer.texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void RenderBackend_DrawScreen(void)
{
	spritebatch_tick(&glyph_batcher);

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

RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

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
	surface->lost = FALSE;

	// Add to linked-list
	surface->prev = NULL;
	surface->next = surface_list_head;
	surface_list_head = surface;

	if (surface->next != NULL)
		surface->next->prev = surface;

	return surface;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	if (surface == NULL)
		return;

	// Remove from linked list
	if (surface->next != NULL)
		surface->next->prev = surface->prev;
	if (surface->prev != NULL)
		surface->prev->next = surface->next;

	SDL_DestroyTexture(surface->texture);
	free(surface);
}

BOOL RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface)
{
	return surface->lost;
}

void RenderBackend_RestoreSurface(RenderBackend_Surface *surface)
{
	surface->lost = FALSE;
}

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return NULL;

	*pitch = width * 4;

	surface->pixels = (unsigned char*)malloc(width * height * 4);

	return surface->pixels;
}

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return;

	// Pre-multiply the colour channels with the alpha, so blending works correctly
	for (unsigned int y = 0; y < height; ++y)
	{
		unsigned char *pixels = surface->pixels + y * width * 4;

		for (unsigned int x = 0; x < width; ++x)
		{
			pixels[0] = (pixels[0] * pixels[3]) / 0xFF;
			pixels[1] = (pixels[1] * pixels[3]) / 0xFF;
			pixels[2] = (pixels[2] * pixels[3]) / 0xFF;
			pixels += 4;
		}
	}

	SDL_Rect rect = {0, 0, (int)width, (int)height};
	SDL_UpdateTexture(surface->texture, &rect, surface->pixels, width * 4);

	free(surface->pixels);
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RECT *rect, RenderBackend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
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

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
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

RenderBackend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	RenderBackend_Glyph *glyph = (RenderBackend_Glyph*)malloc(sizeof(RenderBackend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->pixels = (unsigned char*)malloc(width * height * 4);

	if (glyph->pixels == NULL)
	{
		free(glyph);
		return NULL;
	}

	unsigned char *destination_pointer = glyph->pixels;

	for (unsigned int y = 0; y < height; ++y)
	{
		const unsigned char *source_pointer = pixels + y * pitch;

		for (unsigned int x = 0; x < width; ++x)
		{
			const unsigned char alpha = *source_pointer++;

			*destination_pointer++ = alpha;
			*destination_pointer++ = alpha;
			*destination_pointer++ = alpha;
			*destination_pointer++ = alpha;
		}
	}

	glyph->width = width;
	glyph->height = height;

	return glyph;
}

void RenderBackend_UnloadGlyph(RenderBackend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	free(glyph->pixels);
	free(glyph);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels)
{
	if (destination_surface == NULL)
		return;

	SDL_SetRenderTarget(renderer, destination_surface->texture);

	memcpy(glyph_colour_channels, colour_channels, sizeof(glyph_colour_channels));
}

void RenderBackend_DrawGlyph(RenderBackend_Glyph *glyph, long x, long y)
{
	spritebatch_push(&glyph_batcher, (SPRITEBATCH_U64)glyph, glyph->width, glyph->height, x, y, 1.0f, 1.0f, 0.0f, 0.0f, 0);
}

void RenderBackend_FlushGlyphs(void)
{
	spritebatch_defrag(&glyph_batcher);
	spritebatch_flush(&glyph_batcher);
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	for (RenderBackend_Surface *surface = surface_list_head; surface != NULL; surface = surface->next)
		surface->lost = TRUE;
}

void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// No problem for us
}
