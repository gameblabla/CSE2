// Released under the MIT licence.
// See LICENCE.txt for details.

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

// Used to transfer the final rendered display to the framebuffer
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

// Used to convert textures to 3DS tiled format
#define TEXTURE_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct RenderBackend_Surface
{
	C3D_Tex texture;
	C3D_RenderTarget *render_target;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	C3D_Tex texture;
	unsigned char *local_texture_buffer;
} RenderBackend_GlyphAtlas;

static RenderBackend_GlyphAtlas *glyph_atlas;
static RenderBackend_Surface *glyph_destination_surface;
static C2D_ImageTint glyph_tint;

static C3D_RenderTarget *screen_render_target;

static RenderBackend_Surface *framebuffer_surface;
static size_t framebuffer_surface_width;
static size_t framebuffer_surface_height;

static bool frame_started;

static size_t RoundUpToPowerOfTwo(size_t value)
{
	size_t accumulator = 1;

	while (accumulator < value)
		accumulator <<= 1;

	return accumulator;
}

static void EnableAlpha(bool enabled)
{
	static bool previous_setting = true;

	if (enabled != previous_setting)
	{
		// Setting will not take effect mid-frame, so
		// break-up the current frame if we have to.
		if (frame_started)
			C2D_Flush();

		if (enabled)
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
		else
			C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO);

		previous_setting = enabled;
	}
}

static void SelectRenderTarget(C3D_RenderTarget *render_target)
{
	static C3D_RenderTarget *previous_render_target = NULL;

	if (render_target != previous_render_target)
	{
		previous_render_target = render_target;

		C2D_SceneBegin(render_target);
	}
}

static void BeginRendering(void)
{
	if (!frame_started)
	{
		C3D_FrameBegin(0);
		frame_started = true;
	}
}

static void EndRendering(void)
{
	if (frame_started)
	{
		C3D_FrameEnd(0);
		frame_started = false;
	}
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	if (C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
	{
		if (C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
		{
			C2D_Prepare();

			C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_ALL);

			screen_render_target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, -1);

			if (screen_render_target != NULL)
			{
				C3D_RenderTargetSetOutput(screen_render_target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

				framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

				if (framebuffer_surface != NULL)
				{
					framebuffer_surface_width = screen_width;
					framebuffer_surface_height = screen_height;

					return framebuffer_surface;
				}
				else
				{
					Backend_PrintError("RenderBackend_CreateSurface failed in RenderBackend_Init");
				}

				C3D_RenderTargetDelete(screen_render_target);
			}
			else
			{
				Backend_PrintError("C2D_CreateScreenTarget failed in RenderBackend_Init");
			}

			C2D_Fini();
		}
		else
		{
			Backend_PrintError("C2D_Init failed in RenderBackend_Init");
		}

		C3D_Fini();
	}
	else
	{
		Backend_PrintError("C3D_Init failed in RenderBackend_Init");
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	// Just in case
	EndRendering();

	RenderBackend_FreeSurface(framebuffer_surface);

	C3D_RenderTargetDelete(screen_render_target);

	C2D_Fini();
	C3D_Fini();
}

void RenderBackend_DrawScreen(void)
{
	EndRendering();

	EnableAlpha(false);

	const float texture_left = 0.0f;
	const float texture_top = 0.0f;
	const float texture_right = (float)framebuffer_surface_width / framebuffer_surface->texture.width;
	const float texture_bottom = (float)framebuffer_surface_height / framebuffer_surface->texture.height;

	Tex3DS_SubTexture subtexture;
	subtexture.width = framebuffer_surface_width;
	subtexture.height = framebuffer_surface_height;
	subtexture.left = texture_left;
	subtexture.top = 1.0f - texture_top;
	subtexture.right = texture_right;
	subtexture.bottom = 1.0f - texture_bottom;

	C2D_Image image;
	image.tex = &framebuffer_surface->texture;
	image.subtex = &subtexture;

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	C2D_TargetClear(screen_render_target, C2D_Color32(0, 0, 0, 0xFF));

	SelectRenderTarget(screen_render_target);

	C2D_DrawImageAt(image, (400 - framebuffer_surface_width) / 2, (240 - framebuffer_surface_height) / 2, 0.0f);

	C3D_FrameEnd(0);
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	// Just in case
	EndRendering();

	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->render_target = NULL;

		memset(&surface->texture, 0, sizeof(surface->texture));

		if ((render_target ? C3D_TexInitVRAM : C3D_TexInit)(&surface->texture, RoundUpToPowerOfTwo(width), RoundUpToPowerOfTwo(height), GPU_RGBA8))
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
					C2D_TargetClear(surface->render_target, C2D_Color32(0, 0, 0, 0xFF));

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
			Backend_PrintError("C3D_TexInit/C3D_TexInitVRAM failed in RenderBackend_CreateSurface");
		}

		free(surface);
	}

	return NULL;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	// Just in case
	EndRendering();

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
	EndRendering();

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

	BeginRendering();

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

	SelectRenderTarget(destination_surface->render_target);

	C2D_DrawImageAt(image, x, y, 0.0f);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	EnableAlpha(false);

	BeginRendering();

	SelectRenderTarget(surface->render_target);

	C2D_DrawRectSolid(rect->left, rect->top, 0.0f, rect->right - rect->left, rect->bottom - rect->top, C2D_Color32(red, green, blue, red == 0 && green == 0 && blue == 0 ? 0 : 0xFF));
}

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t width, size_t height)
{
	// Just in case
	EndRendering();

	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		width = RoundUpToPowerOfTwo(width);
		height = RoundUpToPowerOfTwo(height);

		atlas->local_texture_buffer = (unsigned char*)linearAlloc(width * height * 4);

		if (atlas->local_texture_buffer != NULL)
		{
			memset(&atlas->texture, 0, sizeof(atlas->texture));

			if (C3D_TexInit(&atlas->texture, width, height, GPU_RGBA8))
			{
				C3D_TexSetFilter(&atlas->texture, GPU_NEAREST, GPU_NEAREST);

				return atlas;
			}
			else
			{
				Backend_PrintError("C3D_TexInit failed in RenderBackend_CreateGlyphAtlas");
			}

			linearFree(atlas->local_texture_buffer);
		}
		else
		{
			Backend_PrintError("linearAlloc failed in RenderBackend_CreateGlyphAtlas");
		}

		free(atlas);
	}
	else
	{
		Backend_PrintError("malloc failed in RenderBackend_CreateGlyphAtlas");
	}


	return NULL;
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	// Just in case
	EndRendering();

	C3D_TexDelete(&atlas->texture);
	linearFree(atlas->local_texture_buffer);
	free(atlas);
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height, size_t pitch)
{
	// This might be needed, but right now it actually *causes*
	// corruption rather than prevent it, so it's been disabled
	// (it causes the 'Studio Pixel presents' text to appear incomplete).
//	EndRendering();

	for (size_t h = 0; h < height; ++h)
	{
		const unsigned char *source_pointer = &pixels[h * pitch];
		unsigned char *destination_pointer = &atlas->local_texture_buffer[((y + h) * atlas->texture.width + x) * 4];

		for (size_t w = 0; w < width; ++w)
		{
			*destination_pointer++ = *source_pointer++;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
			*destination_pointer++ = 0xFF;
		}
	}

	GSPGPU_FlushDataCache(atlas->local_texture_buffer, atlas->texture.width * atlas->texture.height * 4);

	C3D_SyncDisplayTransfer((u32*)atlas->local_texture_buffer, GX_BUFFER_DIM(atlas->texture.width, atlas->texture.height), (u32*)atlas->texture.data, GX_BUFFER_DIM(atlas->texture.width, atlas->texture.height), TEXTURE_TRANSFER_FLAGS);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	EnableAlpha(true);

	glyph_atlas = atlas;
	glyph_destination_surface = destination_surface;

	C2D_PlainImageTint(&glyph_tint, C2D_Color32(red, green, blue, 0xFF), 1.0f);
}

void RenderBackend_DrawGlyph(long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	BeginRendering();

	SelectRenderTarget(glyph_destination_surface->render_target);

	const float texture_left = (float)glyph_x / glyph_atlas->texture.width;
	const float texture_top = (float)(glyph_atlas->texture.height - glyph_y) / glyph_atlas->texture.height;
	const float texture_right = (float)(glyph_x + glyph_width) / glyph_atlas->texture.width;
	const float texture_bottom = (float)(glyph_atlas->texture.height - (glyph_y + glyph_height)) / glyph_atlas->texture.height;

	Tex3DS_SubTexture subtexture;
	subtexture.width = glyph_width;
	subtexture.height = glyph_height;
	subtexture.left = texture_left;
	subtexture.top = texture_top;
	subtexture.right = texture_right;
	subtexture.bottom = texture_bottom;

	C2D_Image image;
	image.tex = &glyph_atlas->texture;
	image.subtex = &subtexture;

	C2D_DrawImageAt(image, x, y, 0.0f, &glyph_tint);
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;

	// Will never happen
}
