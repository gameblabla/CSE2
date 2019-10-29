#pragma once

#include "SDL.h"

#include "../WindowsWrapper.h"

typedef enum FontPixelMode
{
	FONT_PIXEL_MODE_GRAY,
	FONT_PIXEL_MODE_MONO
} FontPixelMode;

typedef struct Backend_Surface Backend_Surface;
typedef struct Backend_Glyph Backend_Glyph;

SDL_Window* Backend_CreateWindow(const char *title, int width, int height);
Backend_Surface* Backend_Init(SDL_Window *window, unsigned int internal_screen_width, unsigned int internal_screen_height, BOOL vsync);
void Backend_Deinit(void);
void Backend_DrawScreen(void);
Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height);
void Backend_FreeSurface(Backend_Surface *surface);
BOOL Backend_IsSurfaceLost(Backend_Surface *surface);
void Backend_RestoreSurface(Backend_Surface *surface);
unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch);
void Backend_UnlockSurface(Backend_Surface *surface);
void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend);
void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, FontPixelMode pixel_mode);
void Backend_UnloadGlyph(Backend_Glyph *glyph);
void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours);
void Backend_HandleRenderTargetLoss(void);
void Backend_HandleWindowResize(void);
