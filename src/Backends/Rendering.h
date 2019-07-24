#pragma once

#include "SDL.h"

#include "../WindowsWrapper.h"

enum
{
	FONT_PIXEL_MODE_GRAY,
	FONT_PIXEL_MODE_MONO,
};

typedef struct Backend_Surface Backend_Surface;
typedef struct Backend_Glyph Backend_Glyph;

BOOL Backend_Init(SDL_Window *window, unsigned int width, unsigned int height, BOOL vsync);
void Backend_Deinit(void);
void Backend_DrawScreen(void);
Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height);
void Backend_FreeSurface(Backend_Surface *surface);
unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch);
void Backend_Unlock(Backend_Surface *surface);
void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend);
void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL alpha_blend);
void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue);
void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect);
Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned short total_greys, unsigned char pixel_mode);
void Backend_UnloadGlyph(Backend_Glyph *glyph);
void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours);
void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours);
void Backend_HandleDeviceLoss(void);
void Backend_HandleWindowResize(void);
