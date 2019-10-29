#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#undef MIN
#undef MAX
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct Backend_Surface
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
} Backend_Surface;

typedef struct Backend_Glyph
{
	void *pixels;
	unsigned int width;
	unsigned int height;
	FontPixelMode pixel_mode;
} Backend_Glyph;

static SDL_Renderer *renderer;
static SDL_Texture *texture;

static Backend_Surface framebuffer;

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
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, internal_screen_width, internal_screen_height);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	if (texture == NULL)
	{
		SDL_DestroyRenderer(renderer);
		return NULL;
	}

	SDL_LockTexture(texture, NULL, (void**)&framebuffer.pixels, (int*)&framebuffer.pitch);

	framebuffer.width = internal_screen_width;
	framebuffer.height = internal_screen_height;

	return &framebuffer;
}

void Backend_Deinit(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
}

void Backend_DrawScreen(void)
{
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

	SDL_LockTexture(texture, NULL, (void**)&framebuffer.pixels, (int*)&framebuffer.pitch);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	surface->pixels = (unsigned char*)malloc(width * height * 4);

	if (surface->pixels == NULL)
	{
		free(surface);
		return NULL;
	}

	surface->width = width;
	surface->height = height;
	surface->pitch = width * 4;

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	free(surface->pixels);
	free(surface);
}

BOOL Backend_IsSurfaceLost(Backend_Surface *surface)
{
	(void)surface;

	return FALSE;
}

void Backend_RestoreSurface(Backend_Surface *surface)
{
	(void)surface;
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	*pitch = surface->pitch;
	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	(void)surface;
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect and coordinates so we don't write outside the pixel buffer
	long overflow;

	overflow = 0 - x;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
		x += overflow;
	}

	overflow = 0 - y;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
		y += overflow;
	}

	overflow = (x + (rect_clamped.right - rect_clamped.left)) - destination_surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = (y + (rect_clamped.bottom - rect_clamped.top)) - destination_surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	if (rect_clamped.bottom - rect_clamped.top <= 0)
		return;

	if (rect_clamped.right - rect_clamped.left <= 0)
		return;

	for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
	{
		unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 4)];
		unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 4)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			if (source_pointer[3] == 0xFF)
			{
				*destination_pointer++ = *source_pointer++;
				*destination_pointer++ = *source_pointer++;
				*destination_pointer++ = *source_pointer++;
				*destination_pointer++ = *source_pointer++;
			}
			else if (source_pointer[3] != 0)
			{
				const float src_alpha = source_pointer[3] / 255.0f;
				const float dst_alpha = destination_pointer[3] / 255.0f;
				const float out_alpha = src_alpha + dst_alpha * (1.0f - src_alpha);

				for (unsigned int j = 0; j < 3; ++j)
					destination_pointer[j] = (unsigned char)((source_pointer[j] * src_alpha + destination_pointer[j] * dst_alpha * (1.0f - src_alpha)) / out_alpha);

				destination_pointer[3] = (unsigned char)(out_alpha * 255.0f);

				source_pointer += 4;
				destination_pointer += 4;
			}
			else
			{
				source_pointer += 4;
				destination_pointer += 4;
			}
		}
	}
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	if (surface == NULL)
		return;

	RECT rect_clamped;

	rect_clamped.left = rect->left;
	rect_clamped.top = rect->top;
	rect_clamped.right = rect->right;
	rect_clamped.bottom = rect->bottom;

	// Clamp the rect so it doesn't write outside the pixel buffer
	long overflow;

	overflow = 0 - rect_clamped.left;
	if (overflow > 0)
	{
		rect_clamped.left += overflow;
	}

	overflow = 0 - rect_clamped.top;
	if (overflow > 0)
	{
		rect_clamped.top += overflow;
	}

	overflow = rect_clamped.right - surface->width;
	if (overflow > 0)
	{
		rect_clamped.right -= overflow;
	}

	overflow = rect_clamped.bottom - surface->height;
	if (overflow > 0)
	{
		rect_clamped.bottom -= overflow;
	}

	if (rect_clamped.bottom - rect_clamped.top <= 0)
		return;

	if (rect_clamped.right - rect_clamped.left <= 0)
		return;

	for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
	{
		unsigned char *destination_pointer = &surface->pixels[((rect_clamped.top + j) * surface->pitch) + (rect_clamped.left * 4)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			*destination_pointer++ = red;
			*destination_pointer++ = green;
			*destination_pointer++ = blue;
			*destination_pointer++ = alpha;
		}
	}
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, FontPixelMode pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	switch (pixel_mode)
	{
		case FONT_PIXEL_MODE_GRAY:
		{
			glyph->pixels = malloc(width * height * sizeof(float));

			if (glyph->pixels == NULL)
			{
				free(glyph);
				return NULL;
			}

			float *destination_pointer = (float*)glyph->pixels;

			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;

				for (unsigned int x = 0; x < width; ++x)
					*destination_pointer++ = *source_pointer++ / 255.0f;
			}

			break;
		}

		case FONT_PIXEL_MODE_MONO:
		{
			glyph->pixels = malloc(width * height);

			if (glyph->pixels == NULL)
			{
				free(glyph);
				return NULL;
			}

			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = (unsigned char*)glyph->pixels + y * width;

				memcpy(destination_pointer, source_pointer, width);
			}

			break;
		}
	}

	glyph->width = width;
	glyph->height = height;
	glyph->pixel_mode = pixel_mode;

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	free(glyph->pixels);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	switch (glyph->pixel_mode)
	{
		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					const float src_alpha = ((float*)glyph->pixels)[iy * glyph->width + ix];

					if (src_alpha)
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 4;

						const float dst_alpha = bitmap_pixel[3] / 255.0f;
						const float out_alpha = src_alpha + dst_alpha * (1.0f - src_alpha);

						for (unsigned int j = 0; j < 3; ++j)
							bitmap_pixel[j] = (unsigned char)((colours[j] * src_alpha + bitmap_pixel[j] * dst_alpha * (1.0f - src_alpha)) / out_alpha);	// Alpha blending			// Gamma-corrected alpha blending

						bitmap_pixel[3] = (unsigned char)(out_alpha * 255.0f);
					}
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int iy = MAX(-y, 0); y + iy < MIN(y + glyph->height, surface->height); ++iy)
			{
				for (unsigned int ix = MAX(-x, 0); x + ix < MIN(x + glyph->width, surface->width); ++ix)
				{
					if (((unsigned char*)glyph->pixels)[iy * glyph->width + ix])
					{
						unsigned char *bitmap_pixel = surface->pixels + (y + iy) * surface->pitch + (x + ix) * 4;

						for (unsigned int j = 0; j < 3; ++j)
							bitmap_pixel[j] = colours[j];

						bitmap_pixel[3] = 0xFF;
					}
				}
			}

			break;
	}
}

void Backend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
