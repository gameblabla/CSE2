#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <3ds.h>
#include <citro3d.h>
#include <c3d/renderqueue.h>

#include "../Misc.h"
#include "../../Attributes.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Used to transfer the final rendered display to the framebuffer
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

// Used to convert textures to 3DS tiled format
// Note: vertical flip flag set so 0,0 is top left of texture
#define TEXTURE_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct
{
	float x, y, z;
	float u, v;
} VBOEntry;

typedef struct RenderBackend_Surface
{
	C3D_Tex texture;
	C3D_RenderTarget *render_target;
	size_t width;
	size_t height;
	size_t padded_width;
	size_t padded_height;
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

static DVLB_s *vertex_shader;
static shaderProgram_s shader_program;
static int uniform_projection;

static VBOEntry *vbo;

static C3D_Mtx projection_matrix;

static const unsigned char vshader[] = {
	#include "vshader.h"
};

static size_t NextPowerOfTwo(size_t value)
{
	size_t accumulator = 1;

	while (accumulator < value)
		accumulator <<= 1;

	return accumulator;
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Set up screen render target
	screen_render_target = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH16);
	C3D_RenderTargetSetOutput(screen_render_target, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	// Set up shader
	vertex_shader = DVLB_ParseFile((u32*)vshader, sizeof(vshader));

	shaderProgramInit(&shader_program);
	shaderProgramSetVsh(&shader_program, &vertex_shader->DVLE[0]);
	C3D_BindProgram(&shader_program);

	uniform_projection = shaderInstanceGetUniformLocation(shader_program.vertexShader, "projection");

	// Set up VBO
	vbo = (VBOEntry*)linearAlloc(sizeof(VBOEntry) * 6);

	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v2=texcoord

	// Buffer
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo, sizeof(VBOEntry), 2, 0x10);

	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

	C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_ALL);

	framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

	if (framebuffer_surface == NULL)
		Backend_PrintError("RenderBackend_CreateSurface failed");

	return framebuffer_surface;
}

void RenderBackend_Deinit(void)
{
	shaderProgramFree(&shader_program);
	DVLB_Free(vertex_shader);

	linearFree(vbo);

	C3D_RenderTargetDelete(screen_render_target);

	C3D_Fini();
}

void RenderBackend_DrawScreen(void)
{
	Mtx_OrthoTilt(&projection_matrix, 0.0f, 400.0f, 240.0f, 0.0f, 0.0f, 1.0f, true);

	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniform_projection, &projection_matrix);

	C3D_TexBind(0, &framebuffer_surface->texture);

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	C3D_RenderTargetClear(screen_render_target, C3D_CLEAR_COLOR, 0x000000FF, 0);
	C3D_FrameDrawOn(screen_render_target);

	const float vertex_left = (400 - framebuffer_surface->width) / 2;
	const float vertex_top = (240 - framebuffer_surface->height) / 2;
	const float vertex_right = vertex_left + framebuffer_surface->width;
	const float vertex_bottom = vertex_top + framebuffer_surface->height;

	const float texture_left = 0.0f;
	const float texture_top = 0.0f;
	const float texture_right = (float)framebuffer_surface->width / framebuffer_surface->padded_width;
	const float texture_bottom = (float)framebuffer_surface->height / framebuffer_surface->padded_height;

/*	vbo[0] = (VBOEntry){vertex_left, vertex_top, 0.5f, texture_left, texture_top};
	vbo[1] = (VBOEntry){vertex_left, vertex_bottom, 0.5f, texture_left, texture_bottom};
	vbo[2] = (VBOEntry){vertex_right, vertex_top, 0.5f, texture_right, texture_top};
	vbo[3] = (VBOEntry){vertex_left, vertex_bottom, 0.5f, texture_left, texture_bottom};
	vbo[4] = (VBOEntry){vertex_right, vertex_bottom, 0.5f, texture_right, texture_bottom};
	vbo[5] = (VBOEntry){vertex_right, vertex_top, 0.5f, texture_right, texture_top};

	C3D_DrawArrays(GPU_TRIANGLES, 0, 6);
*/

	// Draw a textured quad directly
	C3D_ImmDrawBegin(GPU_TRIANGLES);
		C3D_ImmSendAttrib(vertex_left, vertex_top, 0.5f, 0.0f); // v0=position
		C3D_ImmSendAttrib( texture_left, texture_top, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_left, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_left, texture_bottom, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_top, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_top, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_left, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_left, texture_bottom, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_bottom, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_top, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_top, 0.0f, 0.0f);
	C3D_ImmDrawEnd();
/*
	vbo[0] = (VBOEntry){0.0f, 0.0f, 0.5f, 0.0f, 0.0f};
	vbo[1] = (VBOEntry){framebuffer_surface->width, 0.0f, 0.5f, 1.0f, 0.0f};
	vbo[2] = (VBOEntry){0.0f, framebuffer_surface->height, 0.5f, 0.0f, 1.0f};
	vbo[3] = (VBOEntry){0.0f, framebuffer_surface->height, 0.5f, 0.0f, 1.0f};
	vbo[4] = (VBOEntry){framebuffer_surface->width, 0.0f, 0.5f, 1.0f, 0.0f};
	vbo[5] = (VBOEntry){framebuffer_surface->width, framebuffer_surface->height, 0.5f, 1.0f, 1.0f};
*/
//	C3D_DrawArrays(GPU_TRIANGLES, 0, 6);

	C3D_FrameEnd(0);
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->width = width;
		surface->height = height;
		surface->padded_width = NextPowerOfTwo(width);
		surface->padded_height = NextPowerOfTwo(height);
		surface->render_target = NULL;

		memset(&surface->texture, 0, sizeof(surface->texture));
		C3D_TexInitVRAM(&surface->texture, surface->padded_width, surface->padded_height, GPU_RGBA8);
		C3D_TexSetFilter(&surface->texture, GPU_NEAREST, GPU_NEAREST);

		if (!render_target)
		{
			return surface;
		}
		else
		{
			surface->render_target = C3D_RenderTargetCreateFromTex(&surface->texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);

			if (surface->render_target != NULL)
			{
				return surface;
			}
			else
			{
				Backend_PrintError("C3D_RenderTargetCreateFromTex failed");
			}

			C3D_TexDelete(&surface->texture);

			free(surface);
		}
	}

	return NULL;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
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
	u8 *gpusrc = (u8*)linearAlloc(width * height * 4);

	// GX_DisplayTransfer needs input buffer in linear RAM
	const u8* src=pixels; u8 *dst=gpusrc;

	// lodepng outputs big endian rgba so we need to convert
	for (size_t i = 0; i< width * height; ++i)
	{
		unsigned char r = *src++;
		unsigned char g = *src++;
		unsigned char b = *src++;

		*dst++ = r == 0 && g == 0 && b == 0 ? 0 : 0xFF;
		*dst++ = b;
		*dst++ = g;
		*dst++ = r;
	}

	// ensure data is in physical ram
	GSPGPU_FlushDataCache(gpusrc, width * height * 4);

	C3D_SyncDisplayTransfer ((u32*)gpusrc, GX_BUFFER_DIM(width, height), (u32*)surface->texture.data, GX_BUFFER_DIM(surface->padded_width, surface->padded_height), TEXTURE_TRANSFER_FLAGS);
//	C3D_SyncDisplayTransfer ((u32*)gpusrc, GX_BUFFER_DIM(width, height), (u32*)framebuffer_surface->texture.data, GX_BUFFER_DIM(framebuffer_surface->padded_width, framebuffer_surface->padded_height), TEXTURE_TRANSFER_FLAGS);

	linearFree(gpusrc);
}

ATTRIBUTE_HOT void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	Mtx_Ortho(&projection_matrix, 0.0f, destination_surface->padded_width, 0.0f, destination_surface->padded_height, 0.0f, 1.0f, true);
//	Mtx_OrthoTilt(&projection_matrix, 0.0f, 400.0f, 240.0f, 0.0f, 0.0f, 1.0f, true);

	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniform_projection, &projection_matrix);

	C3D_TexBind(0, &source_surface->texture);

	C3D_FrameBegin(0);

	C3D_FrameDrawOn(destination_surface->render_target);
//	C3D_FrameDrawOn(screen_render_target);

	const float vertex_left = x;
	const float vertex_top = y;
	const float vertex_right = x + (rect->right - rect->left);
	const float vertex_bottom = y + (rect->bottom - rect->top);

	const float texture_left = (float)rect->left / source_surface->padded_width;
	const float texture_top = (float)rect->top / source_surface->padded_height;
	const float texture_right = (float)rect->right / source_surface->padded_width;
	const float texture_bottom = (float)rect->bottom / source_surface->padded_height;

/*	vbo[0] = (VBOEntry){vertex_left, vertex_top, 0.5f, texture_left, texture_top};
	vbo[1] = (VBOEntry){vertex_left, vertex_bottom, 0.5f, texture_left, texture_bottom};
	vbo[2] = (VBOEntry){vertex_right, vertex_top, 0.5f, texture_right, texture_top};
	vbo[3] = (VBOEntry){vertex_left, vertex_bottom, 0.5f, texture_left, texture_bottom};
	vbo[4] = (VBOEntry){vertex_right, vertex_bottom, 0.5f, texture_right, texture_bottom};
	vbo[5] = (VBOEntry){vertex_right, vertex_top, 0.5f, texture_right, texture_top};

	C3D_DrawArrays(GPU_TRIANGLES, 0, 6);
*/

	// Draw a textured quad directly
	C3D_ImmDrawBegin(GPU_TRIANGLES);
		C3D_ImmSendAttrib(vertex_left, vertex_top, 0.5f, 0.0f); // v0=position
		C3D_ImmSendAttrib( texture_left, texture_top, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_top, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_top, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_left, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_left, texture_bottom, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_left, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_left, texture_bottom, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_top, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_top, 0.0f, 0.0f);

		C3D_ImmSendAttrib(vertex_right, vertex_bottom, 0.5f, 0.0f);
		C3D_ImmSendAttrib( texture_right, texture_bottom, 0.0f, 0.0f);
	C3D_ImmDrawEnd();

	C3D_FrameEnd(0);
}

ATTRIBUTE_HOT void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	
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
