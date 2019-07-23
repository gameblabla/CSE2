#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

typedef struct Backend_Surface
{
	SDL_Surface *sdl_surface;
} Backend_Surface;

static SDL_Renderer *renderer;
static SDL_Texture *texture;

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

BOOL Backend_Init(SDL_Renderer *p_renderer)
{
	renderer = p_renderer;

	SDL_Texture *render_target = SDL_GetRenderTarget(renderer);

	int width, height;
	SDL_QueryTexture(render_target, NULL, NULL, &width, &height);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

	if (texture == NULL)
		return FALSE;

	framebuffer.sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGB24);

	if (framebuffer.sdl_surface == NULL)
	{
		SDL_DestroyTexture(texture);
		return FALSE;
	}

	return TRUE;
}

void Backend_Deinit(void)
{
	SDL_FreeSurface(framebuffer.sdl_surface);
	SDL_DestroyTexture(texture);
}

void Backend_DrawScreen(void)
{
	unsigned char *pixels;
	int pitch;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (int i = 0; i < framebuffer.sdl_surface->h; ++i)
		memcpy(&pixels[i * pitch], (unsigned char*)framebuffer.sdl_surface->pixels + i * framebuffer.sdl_surface->pitch, framebuffer.sdl_surface->w * 3);

	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
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

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
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
	
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
{
	SDL_Rect source_rect;
	RectToSDLRect(rect, &source_rect);

	SDL_Rect destination_rect;
	destination_rect.x = x;
	destination_rect.y = y;
	destination_rect.w = source_rect.w;
	destination_rect.h = source_rect.h;

	SDL_SetSurfaceBlendMode(source_surface->sdl_surface, alpha_blend ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE); // Assumes the colour key will always be #000000 (black)

	SDL_BlitSurface(source_surface->sdl_surface, &source_rect, destination_surface->sdl_surface, &destination_rect);
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	Backend_Blit(source_surface, rect, &framebuffer, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	SDL_Rect destination_rect;
	RectToSDLRect(rect, &destination_rect);

	SDL_FillRect(surface->sdl_surface, &destination_rect, SDL_MapRGBA(surface->sdl_surface->format, red, green, blue, alpha));
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	Backend_ColourFill(&framebuffer, rect, red, green, blue, 0xFF);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	Backend_Blit(&framebuffer, rect, surface, rect->left, rect->top, FALSE);
}

void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	DrawText(font, (unsigned char*)surface->sdl_surface->pixels, surface->sdl_surface->pitch, surface->sdl_surface->w, surface->sdl_surface->h, x, y, colour, text, strlen(text), TRUE);
}

void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	DrawText(font, (unsigned char*)framebuffer.sdl_surface->pixels, framebuffer.sdl_surface->pitch, framebuffer.sdl_surface->w, framebuffer.sdl_surface->h, x, y, colour, text, strlen(text), FALSE);
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
