#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

struct Backend_Surface
{
	BOOL needs_syncing;
	SDL_Surface *sdl_surface;
	SDL_Texture *texture;

	struct Backend_Surface *next;
	struct Backend_Surface *prev;
};

static SDL_Renderer *renderer;
static SDL_Texture *screen_texture;

static Backend_Surface *surface_list_head;

static void FlushSurface(Backend_Surface *surface)
{
	unsigned char *buffer = (unsigned char*)malloc(surface->sdl_surface->w * surface->sdl_surface->h * 4);
	unsigned char *buffer_pointer = buffer;

	// Convert the SDL_Surface's colour-keyed pixels to RGBA32
	for (int h = 0; h < surface->sdl_surface->h; ++h)
	{
		unsigned char *src_pixel = (unsigned char*)surface->sdl_surface->pixels + (h * surface->sdl_surface->pitch);

		for (int w = 0; w < surface->sdl_surface->w; ++w)
		{
			*buffer_pointer++ = src_pixel[0];
			*buffer_pointer++ = src_pixel[1];
			*buffer_pointer++ = src_pixel[2];

			if (src_pixel[0] != 0 || src_pixel[1] != 0 || src_pixel[2] != 0)	// Assumes the colour key will always be #00000000 (black)
				*buffer_pointer++ = 0xFF;
			else
				*buffer_pointer++ = 0;

			src_pixel += 3;
		}
	}

	SDL_UpdateTexture(surface->texture, NULL, buffer, surface->sdl_surface->w * 4);

	free(buffer);
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

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);

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

	SDL_SetColorKey(surface->sdl_surface, SDL_TRUE, SDL_MapRGB(surface->sdl_surface->format, 0, 0, 0));

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
	if (surface->next)
		surface->next->prev = surface->prev;
	if (surface->prev)
		surface->prev->next = surface->next;

	SDL_FreeSurface(surface->sdl_surface);
	free(surface);
}

void Backend_LoadPixels(Backend_Surface *surface, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int pitch)
{
	for (unsigned int h = 0; h < height; ++h)
	{
		const unsigned char *src_row = &pixels[h * pitch];
		unsigned char *dst_row = (unsigned char*)surface->sdl_surface->pixels + h * surface->sdl_surface->pitch;

		memcpy(dst_row, src_row, width * 3);
	}

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
		SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);

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
	SDL_SetRenderDrawColor(renderer, red, green, blue, 0xFF);
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

void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	DrawText(font, (unsigned char*)surface->sdl_surface->pixels, surface->sdl_surface->pitch, surface->sdl_surface->w, surface->sdl_surface->h, x, y, colour, text, strlen(text));
	surface->needs_syncing = TRUE;
}

void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	// Painfully slow. Really need to add hardware-accelerated font rendering.
	int surface_width, surface_height;
	SDL_GetRendererOutputSize(renderer, &surface_width, &surface_height);

	SDL_Surface *screen_surface = SDL_CreateRGBSurfaceWithFormat(0, surface_width, surface_height, 0, SDL_PIXELFORMAT_RGB24);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGB24, screen_surface->pixels, screen_surface->pitch);

	DrawText(font, (unsigned char*)screen_surface->pixels, screen_surface->pitch, screen_surface->w, screen_surface->h, x, y, colour, text, strlen(text));

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen_surface);
	SDL_FreeSurface(screen_surface);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
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
