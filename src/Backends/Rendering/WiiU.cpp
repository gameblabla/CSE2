#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/registers.h>
#include <gx2/sampler.h>
#include <gx2/texture.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>
#include <gx2r/resource.h>
#include <gx2r/surface.h>
#include <whb/gfx.h>

#include "../Misc.h"

#include "../WiiU/colour_fill.gsh.h"
#include "../WiiU/texture.gsh.h"
#include "../WiiU/texture_colour_key.gsh.h"

typedef struct RenderBackend_Surface
{
	bool is_screen;
	GX2Texture texture;
	unsigned int width;
	unsigned int height;
	unsigned char *lock_buffer;	// TODO - Dumb
} RenderBackend_Surface;

typedef struct RenderBackend_Glyph
{
} RenderBackend_Glyph;

static WHBGfxShaderGroup texture_shader;
static WHBGfxShaderGroup texture_colour_key_shader;
static WHBGfxShaderGroup colour_fill_shader;

static GX2RBuffer vertex_position_buffer;
static GX2RBuffer texture_coordinate_buffer;

static GX2Sampler sampler;

RenderBackend_Surface *framebuffer;
static unsigned int framebuffer_width;
static unsigned int framebuffer_height;

static void StartFrameDraw(void)
{
	WHBGfxBeginRender();

	// Draw to the gamepad
	WHBGfxBeginRenderDRC();
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

static void EndFrameDraw(void)
{
	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	framebuffer_width = screen_width;
	framebuffer_height = screen_height;

	framebuffer = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (framebuffer != NULL)
	{
		framebuffer->is_screen = true;

		WHBGfxInit();

		if (WHBGfxLoadGFDShaderGroup(&texture_shader, 0, rtexture))
		{
			WHBGfxInitShaderAttribute(&texture_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitShaderAttribute(&texture_shader, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitFetchShader(&texture_shader);

			if (WHBGfxLoadGFDShaderGroup(&texture_colour_key_shader, 0, rtexture_colour_key))
			{
				WHBGfxInitShaderAttribute(&texture_colour_key_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitShaderAttribute(&texture_colour_key_shader, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitFetchShader(&texture_colour_key_shader);

				if (WHBGfxLoadGFDShaderGroup(&colour_fill_shader, 0, rcolour_fill))
				{
					WHBGfxInitShaderAttribute(&colour_fill_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
					WHBGfxInitFetchShader(&colour_fill_shader);

					// Initialise vertex position buffer
					vertex_position_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
																	   GX2R_RESOURCE_USAGE_CPU_READ |
																	   GX2R_RESOURCE_USAGE_CPU_WRITE |
																	   GX2R_RESOURCE_USAGE_GPU_READ);
					vertex_position_buffer.elemSize = 2 * sizeof(float);
					vertex_position_buffer.elemCount = 4;
					GX2RCreateBuffer(&vertex_position_buffer);

					// Initialise texture coordinate buffer
					texture_coordinate_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
																		  GX2R_RESOURCE_USAGE_CPU_READ |
																		  GX2R_RESOURCE_USAGE_CPU_WRITE |
																		  GX2R_RESOURCE_USAGE_GPU_READ);
					texture_coordinate_buffer.elemSize = 2 * sizeof(float);
					texture_coordinate_buffer.elemCount = 4;
					GX2RCreateBuffer(&texture_coordinate_buffer);

					// Initialise sampler
					GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_POINT);

					{
						// Disable depth-test (enabled by default for some reason)
						GX2SetDepthOnlyControl(FALSE, FALSE, GX2_COMPARE_FUNC_ALWAYS);



						// Enable blending
				//		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, FALSE, TRUE);

						// Set custom blending mode for pre-multiplied alpha
		/*				GX2SetBlendControl(GX2_RENDER_TARGET_0,
										   GX2_BLEND_MODE_ZERO,
										   GX2_BLEND_MODE_ONE,
										   GX2_BLEND_COMBINE_MODE_ADD,
										   TRUE,
										   GX2_BLEND_MODE_ZERO,
										   GX2_BLEND_MODE_ONE,
										   GX2_BLEND_COMBINE_MODE_ADD);
		*/
						// Do some binding

						StartFrameDraw();

						return framebuffer;
					}

					GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
					GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

					WHBGfxFreeShaderGroup(&colour_fill_shader);
				}

				WHBGfxFreeShaderGroup(&texture_colour_key_shader);
			}

			WHBGfxFreeShaderGroup(&texture_shader);
		}

		WHBGfxShutdown();

		free(framebuffer);
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	WHBGfxFreeShaderGroup(&colour_fill_shader);
	WHBGfxFreeShaderGroup(&texture_colour_key_shader);
	WHBGfxFreeShaderGroup(&texture_shader);

	WHBGfxShutdown();

	free(framebuffer);
}

void RenderBackend_DrawScreen(void)
{
	EndFrameDraw();

	StartFrameDraw();
}

RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->is_screen = false;
		surface->width = width;
		surface->height = height;

		// Initialise texture
		memset(&surface->texture, 0, sizeof(surface->texture));
		surface->texture.surface.width = width;
		surface->texture.surface.height = height;
		surface->texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
		surface->texture.surface.depth = 1;
		surface->texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
		surface->texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
		surface->texture.surface.mipLevels = 1;
		surface->texture.viewNumMips = 1;
		surface->texture.viewNumSlices = 1;
		surface->texture.compMap = 0x00010203;
		GX2CalcSurfaceSizeAndAlignment(&surface->texture.surface);
		GX2InitTextureRegs(&surface->texture);

		if (GX2RCreateSurface(&surface->texture.surface, (GX2RResourceFlags)(GX2R_RESOURCE_BIND_TEXTURE | GX2R_RESOURCE_BIND_COLOR_BUFFER |
		                                                                     GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_CPU_READ |
		                                                                     GX2R_RESOURCE_USAGE_GPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ)))
		{
			return surface;
		}

		free(surface);
	}

	return NULL;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	if (surface != NULL)
	{
		GX2RDestroySurfaceEx(&surface->texture.surface, (GX2RResourceFlags)0);
		free(surface);
	}
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

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	if (surface != NULL)
	{
		surface->lock_buffer = (unsigned char*)malloc(width * height * 3);
		*pitch = width * 3;

		return surface->lock_buffer;
	}

	return NULL;
}

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height)
{
	if (surface != NULL)
	{
		if (surface->lock_buffer != NULL)
		{
			// Convert from RGB24 to RGBA32, and upload it to the GPU texture
			unsigned char *framebuffer = (unsigned char*)GX2RLockSurfaceEx(&surface->texture.surface, 0, (GX2RResourceFlags)0);

			const unsigned char *in_pointer = surface->lock_buffer;

			for (size_t y = 0; y < height; ++y)
			{
				unsigned char *out_pointer = &framebuffer[surface->texture.surface.pitch * 4 * y];

				for (size_t x = 0; x < width; ++x)
				{
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = 0xFF;
				}
			}

			free(surface->lock_buffer);

			GX2RUnlockSurfaceEx(&surface->texture.surface, 0, (GX2RResourceFlags)0);
		}
	}
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	if (source_surface->is_screen || !destination_surface->is_screen)
		return;

	GX2DrawDone();

	const float destination_left = x;
	const float destination_top = y;
	const float destination_right = x + (rect->right - rect->left);
	const float destination_bottom = y + (rect->bottom - rect->top);

	float *position_pointer = (float*)GX2RLockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);
	position_pointer[0] = destination_left;
	position_pointer[1] = destination_top;
	position_pointer[2] = destination_right;
	position_pointer[3] = destination_top;
	position_pointer[4] = destination_right;
	position_pointer[5] = destination_bottom;
	position_pointer[6] = destination_left;
	position_pointer[7] = destination_bottom;
	for (unsigned int i = 0; i < 8; i += 2)
	{
		position_pointer[i + 0] /= framebuffer_width;
		position_pointer[i + 0] *= 2.0f;
		position_pointer[i + 0] -= 1.0f;

		position_pointer[i + 1] /= framebuffer_height;
		position_pointer[i + 1] *= -2.0f;
		position_pointer[i + 1] += 1.0f;
	}
	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	float *texture_coordinate_pointer = (float*)GX2RLockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	texture_coordinate_pointer[0] = rect->left / (float)source_surface->width;
	texture_coordinate_pointer[1] = rect->top / (float)source_surface->height;
	texture_coordinate_pointer[2] = rect->right / (float)source_surface->width;
	texture_coordinate_pointer[3] = rect->top / (float)source_surface->height;
	texture_coordinate_pointer[4] = rect->right / (float)source_surface->width;
	texture_coordinate_pointer[5] = rect->bottom / (float)source_surface->height;
	texture_coordinate_pointer[6] = rect->left / (float)source_surface->width;
	texture_coordinate_pointer[7] = rect->bottom / (float)source_surface->height;
	GX2RUnlockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);

	GX2SetFetchShader(&texture_colour_key_shader.fetchShader);
	GX2SetVertexShader(texture_colour_key_shader.vertexShader);
	GX2SetPixelShader(texture_colour_key_shader.pixelShader);

	GX2SetPixelSampler(&sampler, texture_colour_key_shader.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&source_surface->texture, texture_colour_key_shader.pixelShader->samplerVars[0].location);
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	if (!surface->is_screen)
		return;

	GX2DrawDone();

	float *position_pointer = (float*)GX2RLockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);
	position_pointer[0] = rect->left;
	position_pointer[1] = rect->top;
	position_pointer[2] = rect->right;
	position_pointer[3] = rect->top;
	position_pointer[4] = rect->right;
	position_pointer[5] = rect->bottom;
	position_pointer[6] = rect->left;
	position_pointer[7] = rect->bottom;
	for (unsigned int i = 0; i < 8; i += 2)
	{
		position_pointer[i + 0] /= framebuffer_width;
		position_pointer[i + 0] *= 2.0f;
		position_pointer[i + 0] -= 1.0f;

		position_pointer[i + 1] /= framebuffer_height;
		position_pointer[i + 1] *= -2.0f;
		position_pointer[i + 1] += 1.0f;
	}
	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	float uniform_colours[4] = {red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f};

	GX2SetPixelUniformReg(colour_fill_shader.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

	GX2SetFetchShader(&colour_fill_shader.fetchShader);
	GX2SetVertexShader(colour_fill_shader.vertexShader);
	GX2SetPixelShader(colour_fill_shader.pixelShader);

	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);

	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
}

RenderBackend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	return NULL;
}

void RenderBackend_UnloadGlyph(RenderBackend_Glyph *glyph)
{
	if (glyph == NULL)
		return;
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels)
{
	if (destination_surface == NULL)
		return;
}

void RenderBackend_DrawGlyph(RenderBackend_Glyph *glyph, long x, long y)
{
	if (glyph == NULL)
		return;
}

void RenderBackend_FlushGlyphs(void)
{
	
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// No problem for us
}
