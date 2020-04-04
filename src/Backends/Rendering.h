#pragma once

#include "../WindowsWrapper.h"

typedef struct RenderBackend_Surface RenderBackend_Surface;
typedef struct RenderBackend_Glyph RenderBackend_Glyph;

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen);
void RenderBackend_Deinit(void);
void RenderBackend_DrawScreen(void);
void RenderBackend_ClearScreen(void);
RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height);
void RenderBackend_FreeSurface(RenderBackend_Surface *surface);
BOOL RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface);
void RenderBackend_RestoreSurface(RenderBackend_Surface *surface);
unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height);
void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height);
void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RECT *rect, RenderBackend_Surface *destination_surface, long x, long y, BOOL colour_key);
void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue);
RenderBackend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch);
void RenderBackend_UnloadGlyph(RenderBackend_Glyph *glyph);
void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels);
void RenderBackend_DrawGlyph(RenderBackend_Glyph *glyph, long x, long y);
void RenderBackend_FlushGlyphs(void);
void RenderBackend_HandleRenderTargetLoss(void);
void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height);
