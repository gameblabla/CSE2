// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "SDL.h"

#include "../Misc.h"
#include "../Shared/SDL.h"

typedef struct RenderBackend_Surface
{
	SDL_Surface *sdlsurface;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	SDL_Surface *sdlsurface;
} RenderBackend_GlyphAtlas;

SDL_Window *window;

static SDL_Surface *window_sdlsurface;

static RenderBackend_Surface framebuffer;

static RenderBackend_GlyphAtlas *glyph_atlas;
static RenderBackend_Surface *glyph_destination_surface;

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

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, 0);

	if (window != NULL)
	{
		if (fullscreen)
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) < 0)
				Backend_PrintError("Could not set window to fullscreen: %s", SDL_GetError());

		window_sdlsurface = SDL_GetWindowSurface(window);

		if (window_sdlsurface != NULL)
		{
			framebuffer.sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, window_sdlsurface->w, window_sdlsurface->h, 0, SDL_PIXELFORMAT_RGB24);

			if (framebuffer.sdlsurface != NULL)
			{
				Backend_PostWindowCreation();

				return &framebuffer;
			}

			std::string error_message = std::string("Could not create framebuffer surface: ") + SDL_GetError();
			Backend_ShowMessageBox("Fatal error (SDLSurface rendering backend)", error_message.c_str());
		}
		else
		{
			std::string error_message = std::string("Could not get SDL surface of the window: ") + SDL_GetError();
			Backend_ShowMessageBox("Fatal error (SDLSurface rendering backend)", error_message.c_str());
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		std::string error_message = std::string("Could not create window: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDLSurface rendering backend)", error_message.c_str());
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	SDL_FreeSurface(framebuffer.sdlsurface);
	SDL_DestroyWindow(window);
}

void RenderBackend_DrawScreen(void)
{
	if (SDL_BlitSurface(framebuffer.sdlsurface, NULL, window_sdlsurface, NULL) < 0)
		Backend_PrintError("Couldn't blit framebuffer surface to window surface: %s", SDL_GetError());

	if (SDL_UpdateWindowSurface(window) < 0)
		Backend_PrintError("Couldn't put window surface on screen: %s", SDL_GetError());
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	(void)render_target;

	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);

	if (surface->sdlsurface == NULL)
	{
		free(surface);
		return NULL;
	}

	return surface;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	SDL_FreeSurface(surface->sdlsurface);
	free(surface);
}

bool RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface)
{
	(void)surface;

	return false;
}

void RenderBackend_RestoreSurface(RenderBackend_Surface *surface)
{
	(void)surface;
}

void RenderBackend_UploadSurface(RenderBackend_Surface *surface, const unsigned char *pixels, size_t width, size_t height)
{
	if (SDL_LockSurface(surface->sdlsurface) == 0)
	{
		for (size_t y = 0; y < height; ++y)
			memcpy(&((unsigned char*)surface->sdlsurface->pixels)[y * surface->sdlsurface->pitch], &pixels[y * width * 3], width * 3);

		SDL_UnlockSurface(surface->sdlsurface);
	}
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect;
	destination_rect.x = x;
	destination_rect.y = y;
	destination_rect.w = source_rect.w;
	destination_rect.h = source_rect.h;

	// Assumes the colour key will always be #000000 (black)
	if (SDL_SetColorKey(source_surface->sdlsurface, colour_key ? SDL_TRUE : SDL_FALSE, SDL_MapRGB(source_surface->sdlsurface->format, 0, 0, 0)) < 0)
		Backend_PrintError("Couldn't set color key of surface: %s", SDL_GetError());

	if (SDL_BlitSurface(source_surface->sdlsurface, &source_rect, destination_surface->sdlsurface, &destination_rect) < 0)
		Backend_PrintError("Couldn't blit surface: %s", SDL_GetError());
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	SDL_Rect destination_rect;
	RectToSDLRect(rect, &destination_rect);

	if (SDL_FillRect(surface->sdlsurface, &destination_rect, SDL_MapRGB(surface->sdlsurface->format, red, green, blue)) < 0)
		Backend_PrintError("Couldn't fill rectangle with color: %s", SDL_GetError());
}

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t width, size_t height)
{
	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		atlas->sdlsurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

		if (atlas->sdlsurface != NULL)
		{
			return atlas;
		}
		else
		{
			Backend_PrintError("Couldn't create RBG surface: %s", SDL_GetError());
		}

		free(atlas);
	}

	return NULL;	
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	SDL_FreeSurface(atlas->sdlsurface);
	free(atlas);
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height, size_t pitch)
{
	SDL_LockSurface(atlas->sdlsurface);

	for (size_t iy = 0; iy < height; ++iy)
	{
		const unsigned char *source_pointer = &pixels[iy * pitch];
		unsigned char *destination_pointer = &((unsigned char*)atlas->sdlsurface->pixels)[(y + iy) * atlas->sdlsurface->pitch + x * 4];

		for (size_t ix = 0; ix < width; ++ix)
		{
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = *source_pointer++;
		}
	}

	SDL_UnlockSurface(atlas->sdlsurface);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	glyph_atlas = atlas;
	glyph_destination_surface = destination_surface;

	if (SDL_SetSurfaceColorMod(atlas->sdlsurface, red, green, blue) < 0)
		Backend_PrintError("Couldn't set color value: %s", SDL_GetError());
}

void RenderBackend_DrawGlyph(long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
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

	if (SDL_BlitSurface(glyph_atlas->sdlsurface, &source_rect, glyph_destination_surface->sdlsurface, &destination_rect) < 0)
		Backend_PrintError("Couldn't blit glyph indivual surface to final glyph surface: %s", SDL_GetError());
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;

	// https://wiki.libsdl.org/SDL_GetWindowSurface
	// We need to fetch a new surface pointer
	window_sdlsurface = SDL_GetWindowSurface(window);

	if (window_sdlsurface == NULL)
		Backend_PrintError("Couldn't get SDL surface for window after resize: %s", SDL_GetError());
}
