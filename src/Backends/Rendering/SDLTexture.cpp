#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../Misc.h"
#include "../Shared/SDL2.h"

typedef struct RenderBackend_Surface
{
	SDL_Texture *texture;
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	bool lost;

	struct RenderBackend_Surface *next;
	struct RenderBackend_Surface *prev;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	SDL_Texture *texture;
} RenderBackend_GlyphAtlas;

SDL_Window *window;

static SDL_Renderer *renderer;

static RenderBackend_Surface framebuffer;

static RenderBackend_Surface *surface_list_head;

static void RectToSDLRect(const RenderBackend_Rect *rect, SDL_Rect *sdl_rect)
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

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, bool fullscreen)
{
	Backend_PrintInfo("Available SDL render drivers:");

	for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
	{
		SDL_RendererInfo info;
		if (SDL_GetRenderDriverInfo(i, &info) < 0)
			Backend_PrintError("Couldn't get render driver information: %s", SDL_GetError());
		else
			Backend_PrintInfo("%s", info.name);
	}

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
		if (fullscreen)
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0)
				Backend_PrintError("Couldn't set window to fullscreen: %s", SDL_GetError());

	#if SDL_VERSION_ATLEAST(2,0,10)
		SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");	// We never interfere with the renderer, so don't let SDL implicitly disable batching
	#endif

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

		if (renderer != NULL)
		{
			SDL_RendererInfo info;
			if (SDL_GetRendererInfo(renderer, &info) < 0)
				Backend_PrintError("Couldn't get selected render driver information: %s", SDL_GetError());
			else
				Backend_PrintInfo("Selected SDL render driver: %s", info.name);

			framebuffer.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, screen_width, screen_height);

			if (framebuffer.texture != NULL)
			{
				framebuffer.width = screen_width;
				framebuffer.height = screen_height;

				Backend_PostWindowCreation();

				return &framebuffer;
			}
			else
			{
				std::string error_message = std::string("Could not create framebuffer: ") + SDL_GetError();
				Backend_ShowMessageBox("Fatal error (SDLTexture rendering backend)", error_message.c_str());
			}

			SDL_DestroyRenderer(renderer);
		}
		else
		{
			std::string error_message = std::string("Could not create renderer: ") + SDL_GetError();
			Backend_ShowMessageBox("Fatal error (SDLTexture rendering backend)", error_message.c_str());
		}


		SDL_DestroyWindow(window);
	}
	else
	{
		std::string error_message = std::string("Could not create window: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDLTexture rendering backend)", error_message.c_str());
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	SDL_DestroyTexture(framebuffer.texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void RenderBackend_DrawScreen(void)
{
	if (SDL_SetRenderTarget(renderer, NULL) < 0)
		Backend_PrintError("Couldn't set default render target as the current rendering target: %s", SDL_GetError());

	if (SDL_RenderCopy(renderer, framebuffer.texture, NULL, NULL) < 0)
		Backend_PrintError("Failed to copy framebuffer texture to default render target: %s", SDL_GetError());

	SDL_RenderPresent(renderer);
}

RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height, bool render_target)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface == NULL)
		return NULL;

	surface->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, render_target ? SDL_TEXTUREACCESS_TARGET : 0, width, height);

	if (surface->texture == NULL)
	{
		free(surface);
		return NULL;
	}

	surface->width = width;
	surface->height = height;
	surface->lost = false;

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

bool RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface)
{
	return surface->lost;
}

void RenderBackend_RestoreSurface(RenderBackend_Surface *surface)
{
	surface->lost = false;
}

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return NULL;

	*pitch = width * 3;

	surface->pixels = (unsigned char*)malloc(width * height * 3);

	return surface->pixels;
}

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return;

	unsigned char *buffer = (unsigned char*)malloc(width * height * 4);

	if (buffer == NULL)
	{
		Backend_PrintError("Couldn't allocate memory for surface buffer: %s", SDL_GetError());
		return;
	}

	const unsigned char *src_pixel = surface->pixels;

	// Convert the SDL_Surface's colour-keyed pixels to RGBA32
	for (unsigned int y = 0; y < height; ++y)
	{
		unsigned char *buffer_pointer = &buffer[y * width * 4];

		for (unsigned int x = 0; x < width; ++x)
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

	free(surface->pixels);

	SDL_Rect rect = {0, 0, (int)width, (int)height};

	if (SDL_UpdateTexture(surface->texture, &rect, buffer, width * 4) < 0)
		Backend_PrintError("Couldn't update part of texture: %s", SDL_GetError());

	free(buffer);
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect = {(int)x, (int)y, source_rect.w, source_rect.h};

	// Blit the texture
	if (SDL_SetTextureBlendMode(source_surface->texture, colour_key ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE) < 0)
		Backend_PrintError("Couldn't set texture blend mode: %s", SDL_GetError());

	if (SDL_SetRenderTarget(renderer, destination_surface->texture) < 0)
		Backend_PrintError("Couldn't set current rendering target: %s", SDL_GetError());

	if (SDL_RenderCopy(renderer, source_surface->texture, &source_rect, &destination_rect) < 0)
		Backend_PrintError("Couldn't copy part of texture to rendering target: %s", SDL_GetError());
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	SDL_Rect sdl_rect;
	RectToSDLRect(rect, &sdl_rect);

	Uint8 alpha = SDL_ALPHA_OPAQUE;

	// Check colour-key
	if (red == 0 && green == 0 && blue == 0)
		alpha = 0;

	if (SDL_SetRenderDrawColor(renderer, red, green, blue, alpha) < 0)
		Backend_PrintError("Couldn't set color for drawing operations: %s", SDL_GetError());

	// Draw colour
	if (SDL_SetRenderTarget(renderer, surface->texture) < 0)
		Backend_PrintError("Couldn't set texture current rendering target: %s", SDL_GetError());

	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE) < 0)
		Backend_PrintError("Couldn't disable blending for drawing operations: %s", SDL_GetError());

	if (SDL_RenderFillRect(renderer, &sdl_rect) < 0)
		Backend_PrintError("Couldn't fill rectangle on current rendering target: %s", SDL_GetError());

	if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0)
		Backend_PrintError("Couldn't enable alpha blending for drawing operations: %s", SDL_GetError());
}


RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t size)
{
	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		atlas->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, size, size);

		if (atlas->texture != NULL)
		{
			return atlas;
		}
		else
		{
			Backend_PrintError("Couldn't create texture for renderer: %s", SDL_GetError());
		}

		free(atlas);
	}

	return NULL;
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	SDL_DestroyTexture(atlas->texture);
	free(atlas);
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height)
{
	unsigned char *buffer = (unsigned char*)malloc(width * height * 4);

	if (buffer != NULL)
	{
		unsigned char *destination_pointer = buffer;
		const unsigned char *source_pointer = pixels;

		for (size_t i = 0; i < width * height; ++i)
		{
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = *source_pointer++;
		}

		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = width;
		rect.h = height;

		if (SDL_UpdateTexture(atlas->texture, &rect, buffer, width * 4) < 0)
			Backend_PrintError("Couldn't update texture: %s", SDL_GetError());

		free(buffer);
	}
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, const unsigned char *colour_channels)
{
	(void)atlas;

	if (destination_surface == NULL)
		return;

	if (SDL_SetRenderTarget(renderer, destination_surface->texture) < 0)
		Backend_PrintError("Couldn't set texture as current rendering target: %s", SDL_GetError());

	// The SDL_Texture side of things uses alpha, not a colour-key, so the bug where the font is blended
	// with the colour key doesn't occur.
	if (SDL_SetTextureColorMod(atlas->texture, colour_channels[0], colour_channels[1], colour_channels[2]) < 0)
		Backend_PrintError("Couldn't set additional color value: %s", SDL_GetError());

	if (SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND) < 0)
		Backend_PrintError("Couldn't set texture blend mode: %s", SDL_GetError());

}

void RenderBackend_DrawGlyph(RenderBackend_GlyphAtlas *atlas, long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	SDL_Rect source_rect;
	source_rect.x = glyph_x;
	source_rect.y = glyph_y;
	source_rect.w = glyph_width;
	source_rect.h = glyph_height;

	SDL_Rect destination_rect;
	destination_rect.x = x;
	destination_rect.y = y;
	destination_rect.w = glyph_width;
	destination_rect.h = glyph_height;

	if (SDL_RenderCopy(renderer, atlas->texture, &source_rect, &destination_rect) < 0)
		Backend_PrintError("Couldn't copy glyph texture portion to renderer: %s", SDL_GetError());
}

void RenderBackend_FlushGlyphs(void)
{
	
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	for (RenderBackend_Surface *surface = surface_list_head; surface != NULL; surface = surface->next)
		surface->lost = true;
}

void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// No problem for us
}
