#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

#include "../Misc.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Used to convert textures to 3DS tiled format
// Note: vertical flip flag set so 0,0 is top left of texture (lol no it isn't)
#define TEXTURE_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct RenderBackend_Surface
{
	C3D_Tex texture;
	C3D_RenderTarget *render_target;
	size_t width;
	size_t height;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
} RenderBackend_GlyphAtlas;
/*
static RenderBackend_Surface framebuffer;

static RenderBackend_GlyphAtlas *glyph_atlas;
static RenderBackend_Surface *glyph_destination_surface;
static unsigned char glyph_colour_channels[3];
*/

static C3D_RenderTarget *screen_render_target;

static RenderBackend_Surface *framebuffer_surface;

static bool frame_started;

static size_t NextPowerOfTwo(size_t value)
{
	size_t accumulator = 1;

	while (accumulator < value)
		accumulator <<= 1;

	return accumulator;
}

static void EnableAlpha(bool enabled)
{
	static bool previous_setting = true;

	// Setting will not take effect mid-frame, so
	// break-up the current frame if we have to.
	if (frame_started && enabled != previous_setting)
	{
		C3D_FrameEnd(0);
		frame_started = false;

		if (enabled)
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
		else
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO);

		previous_setting = enabled;
	}
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

	// Set up screen render target
	screen_render_target = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	if (screen_render_target != NULL)
	{
		framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

		if (framebuffer_surface != NULL)
			return framebuffer_surface;
		else
			Backend_PrintError("RenderBackend_CreateSurface failed in RenderBackend_Init");

		C3D_RenderTargetDelete(screen_render_target);
	}
	else
	{
		Backend_PrintError("C2D_CreateScreenTarget failed in RenderBackend_Init");
	}

	C2D_Fini();
	C3D_Fini();

	return NULL;
}

void RenderBackend_Deinit(void)
{
	// Just in case
	if (frame_started)
	{
		C3D_FrameEnd(0);
		frame_started = false;
	}

	RenderBackend_FreeSurface(framebuffer_surface);

	C3D_RenderTargetDelete(screen_render_target);

	C2D_Fini();
	C3D_Fini();
}

void RenderBackend_DrawScreen(void)
{
	EnableAlpha(false);

	if (!frame_started)
	{
		C3D_FrameBegin(0);
		frame_started = true;
	}

	const float texture_left = 0.0f;
	const float texture_top = 0.0f;
	const float texture_right = (float)framebuffer_surface->width / framebuffer_surface->texture.width;
	const float texture_bottom = (float)framebuffer_surface->height / framebuffer_surface->texture.height;

	Tex3DS_SubTexture subtexture;
	subtexture.width = framebuffer_surface->width;
	subtexture.height = framebuffer_surface->height;
	subtexture.left = texture_left;
	subtexture.top = 1.0f - texture_top;
	subtexture.right = texture_right;
	subtexture.bottom = 1.0f - texture_bottom;

	C2D_Image image;
	image.tex = &framebuffer_surface->texture;
	image.subtex = &subtexture;

	C2D_TargetClear(screen_render_target, 0xFF000000);

	C2D_SceneBegin(screen_render_target);

	C2D_DrawImageAt(image, (400 - framebuffer_surface->width) / 2, (240 - framebuffer_surface->height) / 2, 0.5f, NULL, 1.0f, 1.0f);

	C3D_FrameEnd(0);

	frame_started = false;
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	// Just in case
	if (frame_started)
	{
		C3D_FrameEnd(0);
		frame_started = false;
	}

	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->width = width;
		surface->height = height;
		surface->render_target = NULL;

		memset(&surface->texture, 0, sizeof(surface->texture));

		if (render_target ? C3D_TexInitVRAM(&surface->texture, NextPowerOfTwo(width), NextPowerOfTwo(height), GPU_RGBA8) : C3D_TexInit(&surface->texture, NextPowerOfTwo(width), NextPowerOfTwo(height), GPU_RGBA8))
		{
			C3D_TexSetFilter(&surface->texture, GPU_NEAREST, GPU_NEAREST);

			if (!render_target)
			{
				return surface;
			}
			else
			{
				surface->render_target = C3D_RenderTargetCreateFromTex(&surface->texture, GPU_TEXFACE_2D, 0, -1);

				if (surface->render_target != NULL)
				{
					C2D_TargetClear(surface->render_target, 0xFF000000);

					return surface;
				}
				else
				{
					Backend_PrintError("C3D_RenderTargetCreateFromTex failed in RenderBackend_CreateSurface");
				}
			}

			C3D_TexDelete(&surface->texture);
		}
		else
		{
			Backend_PrintError("C3D_TexInitVRAM failed in RenderBackend_CreateSurface");
		}

		free(surface);
	}

	return NULL;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	// Just in case
	if (frame_started)
	{
		C3D_FrameEnd(0);
		frame_started = false;
	}

	if (surface->render_target != NULL)
		C3D_RenderTargetDelete(surface->render_target);

	C3D_TexDelete(&surface->texture);

	free(surface);
}

bool RenderBackend_IsSurfaceLost(RenderBackend_Surface *surface)
{
	(void)surface;

	return false;
}

void RenderBackend_RestoreSurface(RenderBackend_Surface *surface)
{
	(void)surface;
}

void RenderBackend_UploadSurface(RenderBackend_Surface *surface, const unsigned char *pixels, size_t width, size_t height)
{
	// If we upload while drawing, we get corruption (visible after stage transitions)
	if (frame_started)
	{
		C3D_FrameEnd(0);
		frame_started = false;
	}

	unsigned char *abgr_buffer = (unsigned char*)linearAlloc(surface->texture.width * surface->texture.height * 4);

	if (abgr_buffer != NULL)
	{
		const unsigned char *src = pixels;

		// Convert from colour-keyed RGB to ABGR
		for (size_t h = 0; h < height; ++h)
		{
			unsigned char *dst = &abgr_buffer[h * surface->texture.width * 4];

			for (size_t w = 0; w < width; ++w)
			{
				unsigned char r = *src++;
				unsigned char g = *src++;
				unsigned char b = *src++;

				*dst++ = r == 0 && g == 0 && b == 0 ? 0 : 0xFF;
				*dst++ = b;
				*dst++ = g;
				*dst++ = r;
			}
		}

		GSPGPU_FlushDataCache(abgr_buffer, surface->texture.width * surface->texture.height * 4);

		C3D_SyncDisplayTransfer((u32*)abgr_buffer, GX_BUFFER_DIM(surface->texture.width, surface->texture.height), (u32*)surface->texture.data, GX_BUFFER_DIM(surface->texture.width, surface->texture.height), TEXTURE_TRANSFER_FLAGS);

		linearFree(abgr_buffer);
	}
	else
	{
		Backend_PrintError("Couldn't allocate memory for RenderBackend_UploadSurface");
	}
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	EnableAlpha(colour_key);

	if (!frame_started)
	{
		C3D_FrameBegin(0);
		frame_started = true;
	}

	const float texture_left = (float)rect->left / source_surface->texture.width;
	const float texture_top = (float)(source_surface->texture.height - rect->top) / source_surface->texture.height;
	const float texture_right = (float)rect->right / source_surface->texture.width;
	const float texture_bottom = (float)(source_surface->texture.height - rect->bottom) / source_surface->texture.height;

	Tex3DS_SubTexture subtexture;
	subtexture.width = rect->right - rect->left;
	subtexture.height = rect->bottom - rect->top;
	subtexture.left = texture_left;
	subtexture.top = texture_top;
	subtexture.right = texture_right;
	subtexture.bottom = texture_bottom;

	C2D_Image image;
	image.tex = &source_surface->texture;
	image.subtex = &subtexture;

	C2D_SceneBegin(destination_surface->render_target);

	C2D_DrawImageAt(image, x, y, 0.5f, NULL, 1.0f, 1.0f);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	EnableAlpha(false);

	if (!frame_started)
	{
		C3D_FrameBegin(0);
		frame_started = true;
	}

	C2D_SceneBegin(surface->render_target);

	C2D_DrawRectSolid(rect->left, rect->top, 0.5f, rect->right - rect->left, rect->bottom - rect->top, C2D_Color32(red, green, blue, red == 0 && green == 0 && blue == 0 ? 0 : 0xFF));
}

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t width, size_t height)
{
	return NULL;
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height, size_t pitch)
{
	
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	
}

void RenderBackend_DrawGlyph(long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	
}
