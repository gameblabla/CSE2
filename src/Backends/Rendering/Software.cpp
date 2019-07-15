#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

struct Backend_Surface
{
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
};

static SDL_Renderer *renderer;
static SDL_Texture *texture;

static Backend_Surface framebuffer;

BOOL Backend_Init(SDL_Renderer *p_renderer)
{
	renderer = p_renderer;

	SDL_Texture *render_target = SDL_GetRenderTarget(renderer);

	int width, height;
	SDL_QueryTexture(render_target, NULL, NULL, &width, &height);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);

	if (texture == NULL)
		return FALSE;

	framebuffer.pixels = (unsigned char*)malloc(width * height * 4);

	if (framebuffer.pixels == NULL)
	{
		SDL_DestroyTexture(texture);
		return FALSE;
	}

	framebuffer.width = width;
	framebuffer.height = height;
	framebuffer.pitch = width * 4;

	return TRUE;
}

void Backend_Deinit(void)
{
	free(framebuffer.pixels);
	SDL_DestroyTexture(texture);
}

void Backend_DrawScreen(void)
{
	unsigned char *pixels;
	int pitch;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (unsigned int i = 0; i < framebuffer.height; ++i)
		memcpy(&pixels[i * pitch], &framebuffer.pixels[i * framebuffer.pitch], framebuffer.width * 4);

	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
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
	free(surface->pixels);
	free(surface);
}

void Backend_LoadPixels(Backend_Surface *surface, const unsigned char *pixels, unsigned int width, unsigned int height, unsigned int pitch)
{
	for (unsigned int i = 0; i < height; ++i)
	{
		const unsigned char *src_row = &pixels[i * pitch];
		unsigned char *dst_row = &surface->pixels[i * surface->pitch];

		memcpy(dst_row, src_row, width * 4);
	}
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
{
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

	// Do the actual blitting
	if (alpha_blend)
	{
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
	else
	{
		for (long j = 0; j < rect_clamped.bottom - rect_clamped.top; ++j)
		{
			unsigned char *source_pointer = &source_surface->pixels[((rect_clamped.top + j) * source_surface->pitch) + (rect_clamped.left * 4)];
			unsigned char *destination_pointer = &destination_surface->pixels[((y + j) * destination_surface->pitch) + (x * 4)];

			memcpy(destination_pointer, source_pointer, (rect_clamped.right - rect_clamped.left) * 4);
		}
	}
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	Backend_Blit(source_surface, rect, &framebuffer, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
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
		unsigned char *source_pointer = &surface->pixels[((rect_clamped.top + j) * surface->pitch) + (rect_clamped.left * 4)];

		for (long i = 0; i < rect_clamped.right - rect_clamped.left; ++i)
		{
			*source_pointer++ = red;
			*source_pointer++ = green;
			*source_pointer++ = blue;
			*source_pointer++ = alpha;
		}
	}
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	Backend_ColourFill(&framebuffer, rect, red, green, blue, alpha);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	Backend_Blit(&framebuffer, rect, surface, rect->left, rect->top, FALSE);
}

void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	DrawText(font, surface->pixels, surface->pitch, surface->width, surface->height, x, y, colour, text, strlen(text));
}

void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	Backend_DrawText(&framebuffer, font, x, y, text, colour);
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
