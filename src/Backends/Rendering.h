#pragma once

#include <stddef.h>

typedef struct RenderBackend_Surface RenderBackend_Surface;
typedef struct RenderBackend_GlyphAtlas RenderBackend_GlyphAtlas;

typedef struct RenderBackend_Rect
{
	long left;
	long top;
	long right;
	long bottom;
} RenderBackend_Rect;

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, bool fullscreen);
void RenderBackend_Deinit(void);
void RenderBackend_DrawScreen(void);
RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height, bool render_target);
void RenderBackend_FreeSurface(RenderBackend_Surface *surface);
bool RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface);
void RenderBackend_RestoreSurface(RenderBackend_Surface *surface);
unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height);
void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height);
void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key);
void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue);
RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t size);
void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas);
void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height);
void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels);
void RenderBackend_DrawGlyph(RenderBackend_GlyphAtlas *atlas, long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height);
void RenderBackend_FlushGlyphs(void);
void RenderBackend_HandleRenderTargetLoss(void);
void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height);
