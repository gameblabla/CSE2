#pragma once

#include "../WindowsWrapper.h"

typedef struct Backend_Surface Backend_Surface;
typedef struct Backend_Glyph Backend_Glyph;

Backend_Surface* Backend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen);
void Backend_Deinit(void);
void Backend_DrawScreen(void);
void Backend_ClearScreen(void);
Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height);
void Backend_FreeSurface(Backend_Surface *surface);
BOOL Backend_IsSurfaceLost(Backend_Surface *surface);
void Backend_RestoreSurface(Backend_Surface *surface);
unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height);
void Backend_UnlockSurface(Backend_Surface *surface, unsigned int width, unsigned int height);
void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key);
void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue);
Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch);
void Backend_UnloadGlyph(Backend_Glyph *glyph);
void Backend_PrepareToDrawGlyphs(Backend_Surface *destination_surface, const unsigned char *colour_channels);
void Backend_DrawGlyph(Backend_Glyph *glyph, long x, long y);
void Backend_FlushGlyphs(void);
void Backend_HandleRenderTargetLoss(void);
void Backend_HandleWindowResize(void);
