#pragma once

#include "../WindowsWrapper.h"

typedef struct Backend_Surface Backend_Surface;
typedef struct Backend_Glyph Backend_Glyph;

Backend_Surface* RenderBackend_Init(int screen_width, int screen_height);
void RenderBackend_Deinit(void);
void RenderBackend_DrawScreen(void);
void RenderBackend_ClearScreen(void);
Backend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height);
void RenderBackend_FreeSurface(Backend_Surface *surface);
BOOL RenderBackend_IsSurfaceLost(Backend_Surface *surface);
void RenderBackend_RestoreSurface(Backend_Surface *surface);
unsigned char* RenderBackend_LockSurface(Backend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height);
void RenderBackend_UnlockSurface(Backend_Surface *surface, unsigned int width, unsigned int height);
void RenderBackend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key);
void RenderBackend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue);
Backend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch);
void RenderBackend_UnloadGlyph(Backend_Glyph *glyph);
void RenderBackend_PrepareToDrawGlyphs(Backend_Surface *destination_surface, const unsigned char *colour_channels);
void RenderBackend_DrawGlyph(Backend_Glyph *glyph, long x, long y);
void RenderBackend_FlushGlyphs(void);
void RenderBackend_HandleRenderTargetLoss(void);
void RenderBackend_HandleWindowResize(void);
