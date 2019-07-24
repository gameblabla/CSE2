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

typedef struct Backend_Glyph
{
	SDL_Surface *sdl_surface;
} Backend_Glyph;

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

BOOL Backend_Init(SDL_Window *window, unsigned int width, unsigned int height, BOOL vsync)
{
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | (vsync ? SDL_RENDERER_PRESENTVSYNC : 0));

	if (renderer == NULL)
		return FALSE;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (texture == NULL)
	{
		SDL_DestroyRenderer(renderer);
		return FALSE;
	}

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
	SDL_DestroyRenderer(renderer);
}

void Backend_DrawScreen(void)
{
	unsigned char *pixels;
	int pitch;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (int i = 0; i < framebuffer.sdl_surface->h; ++i)
		memcpy(&pixels[i * pitch], (unsigned char*)framebuffer.sdl_surface->pixels + i * framebuffer.sdl_surface->pitch, framebuffer.sdl_surface->w * 3);

	SDL_UnlockTexture(texture);

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

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned short total_greys, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	glyph->sdl_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 0, SDL_PIXELFORMAT_RGBA32);

	if (glyph->sdl_surface == NULL)
	{
		free(glyph);
		return NULL;
	}

	switch (pixel_mode)
	{
		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->sdl_surface->pixels + y * glyph->sdl_surface->pitch;

				for (unsigned int x = 0; x < width; ++x)
				{
					const unsigned char alpha = (unsigned char)(pow((double)*source_pointer++ / (total_greys - 1), 1.0 / 1.8) * 255.0);

					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->sdl_surface->pixels + y * glyph->sdl_surface->pitch;

				for (unsigned int x = 0; x < width; ++x)
				{
					const unsigned char alpha = *source_pointer++ ? 0xFF : 0;

					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
					*destination_pointer++ = alpha;
				}
			}

			break;
	}

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	SDL_FreeSurface(glyph->sdl_surface);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = glyph->sdl_surface->w;
	rect.h = glyph->sdl_surface->h;

	SDL_SetSurfaceColorMod(glyph->sdl_surface, colours[0], colours[1], colours[2]);

	SDL_BlitSurface(glyph->sdl_surface, NULL, surface->sdl_surface, &rect);
}

void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	Backend_DrawGlyph(&framebuffer, glyph, x, y, colours);
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
