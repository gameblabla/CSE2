// Released under the MIT licence.
// See LICENCE.txt for details.

// Sexy new backend that bounces the software-rendered frame to the GPU,
// eliminating V-tearing, and gaining support for rendering to the TV for
// free!

#include "../Window-Software.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <gx2/display.h>
#include <gx2/draw.h>
#include <gx2/registers.h>
#include <gx2/sampler.h>
#include <gx2/texture.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>
#include <gx2r/resource.h>
#include <gx2r/surface.h>
#include <whb/gfx.h>

#include "../../Attributes.h"

#include "WiiUShaders/texture.gsh.h"

typedef struct Viewport
{
	float x;
	float y;
	float width;
	float height;
} Viewport;

static unsigned char *fake_framebuffer;

static size_t fake_framebuffer_width;
static size_t fake_framebuffer_height;

static WHBGfxShaderGroup shader_group;

static GX2RBuffer vertex_position_buffer;
static GX2RBuffer texture_coordinate_buffer;

static GX2Sampler sampler;

static GX2Texture screen_texture;

static Viewport tv_viewport;
static Viewport drc_viewport;

static void CalculateViewport(size_t actual_screen_width, size_t actual_screen_height, Viewport *viewport)
{
	if (actual_screen_width * fake_framebuffer_height > fake_framebuffer_width * actual_screen_height) // Fancy way to do `if (actual_screen_width / actual_screen_height > fake_framebuffer_width / fake_framebuffer_height)` without floats
	{
		viewport->y = 0.0f;
		viewport->height = actual_screen_height;

		viewport->width = (fake_framebuffer_width * actual_screen_height) / fake_framebuffer_height;
		viewport->x = (actual_screen_width - viewport->width) / 2;
	}
	else
	{
		viewport->x = 0.0f;
		viewport->width = actual_screen_width;

		viewport->height = (fake_framebuffer_height * actual_screen_width) / fake_framebuffer_width;
		viewport->y = (actual_screen_height - viewport->height) / 2;
	}
}

bool WindowBackend_Software_CreateWindow(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	fake_framebuffer_width = screen_width;
	fake_framebuffer_height = screen_height;

	fake_framebuffer = (unsigned char*)malloc(fake_framebuffer_width * fake_framebuffer_height * 3);

	if (fake_framebuffer != NULL)
	{
		WHBGfxInit();

		if (WHBGfxLoadGFDShaderGroup(&shader_group, 0, rtexture))
		{
			WHBGfxInitShaderAttribute(&shader_group, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitShaderAttribute(&shader_group, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitFetchShader(&shader_group);

			// Initialise vertex position buffer
			const float vertex_positions[4][2] = {
				{-1.0f,  1.0f},
				{ 1.0f,  1.0f},
				{ 1.0f, -1.0f},
				{-1.0f, -1.0f}
			};

			vertex_position_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
			                                                   GX2R_RESOURCE_USAGE_CPU_READ |
			                                                   GX2R_RESOURCE_USAGE_CPU_WRITE |
			                                                   GX2R_RESOURCE_USAGE_GPU_READ);
			vertex_position_buffer.elemSize = sizeof(vertex_positions[0]);
			vertex_position_buffer.elemCount = sizeof(vertex_positions) / sizeof(vertex_positions[0]);
			GX2RCreateBuffer(&vertex_position_buffer);
			memcpy(GX2RLockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0), vertex_positions, sizeof(vertex_positions));
			GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

			// Initialise texture coordinate buffer
			const float texture_coordinates[4][2] = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f}
			};

			texture_coordinate_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
			                                                      GX2R_RESOURCE_USAGE_CPU_READ |
			                                                      GX2R_RESOURCE_USAGE_CPU_WRITE |
			                                                      GX2R_RESOURCE_USAGE_GPU_READ);
			texture_coordinate_buffer.elemSize = sizeof(texture_coordinates[0]);
			texture_coordinate_buffer.elemCount = sizeof(texture_coordinates) / sizeof(texture_coordinates[0]);
			GX2RCreateBuffer(&texture_coordinate_buffer);
			memcpy(GX2RLockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0), texture_coordinates, sizeof(texture_coordinates));
			GX2RUnlockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);

			// Initialise sampler
			GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_POINT);

			// Initialise screen texture
			screen_texture.surface.width = fake_framebuffer_width;
			screen_texture.surface.height = fake_framebuffer_height;
			screen_texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
			screen_texture.surface.depth = 1;
			screen_texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
			screen_texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
			screen_texture.surface.mipLevels = 1;
			screen_texture.viewNumMips = 1;
			screen_texture.viewNumSlices = 1;
			screen_texture.compMap = 0x00010203;
			GX2CalcSurfaceSizeAndAlignment(&screen_texture.surface);
			GX2InitTextureRegs(&screen_texture);

			if (GX2RCreateSurface(&screen_texture.surface, (GX2RResourceFlags)(GX2R_RESOURCE_BIND_TEXTURE | GX2R_RESOURCE_BIND_COLOR_BUFFER |
			                                                                   GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_CPU_READ |
			                                                                   GX2R_RESOURCE_USAGE_GPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ)))
			{
				// Do some binding
				GX2SetPixelSampler(&sampler, shader_group.pixelShader->samplerVars[0].location);
				GX2SetPixelTexture(&screen_texture, shader_group.pixelShader->samplerVars[0].location);
				GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
				GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

				// Calculate centred viewports
				switch (GX2GetSystemTVScanMode())
				{
					// For now, we have to match WUT's broken behaviour (its `GX2TVScanMode`
					// enum is missing values, and the rest are off-by-one)
					//case GX2_TV_SCAN_MODE_576I:
					case GX2_TV_SCAN_MODE_480I:	// Actually 576i
					case GX2_TV_SCAN_MODE_480P:	// Actually 480i
						CalculateViewport(854, 480, &tv_viewport);
						break;

					case GX2_TV_SCAN_MODE_720P:	// Actually 480p
					default:	// Funnel the *real* 1080p into this
						CalculateViewport(1280, 720, &tv_viewport);
						break;

					case GX2_TV_SCAN_MODE_1080I:	// Actually invalid
					case GX2_TV_SCAN_MODE_1080P:	// Actually 1080i
						CalculateViewport(1920, 1080, &tv_viewport);
						break;
				}

				CalculateViewport(854, 480, &drc_viewport);

				return true;
			}

			GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
			GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

			WHBGfxFreeShaderGroup(&shader_group);
		}

		WHBGfxShutdown();

		free(fake_framebuffer);
	}

	return false;
}

void WindowBackend_Software_DestroyWindow(void)
{
	GX2RDestroySurfaceEx(&screen_texture.surface, (GX2RResourceFlags)0);

	GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	WHBGfxFreeShaderGroup(&shader_group);

	WHBGfxShutdown();

	free(fake_framebuffer);
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = fake_framebuffer_width * 3;

	return fake_framebuffer;
}

ATTRIBUTE_HOT void WindowBackend_Software_Display(void)
{
	// Convert frame from RGB24 to RGBA32, and upload it to the GPU texture
	unsigned char *framebuffer = (unsigned char*)GX2RLockSurfaceEx(&screen_texture.surface, 0, (GX2RResourceFlags)0);

	const unsigned char *in_pointer = fake_framebuffer;

	for (size_t y = 0; y < fake_framebuffer_height; ++y)
	{
		unsigned char *out_pointer = &framebuffer[screen_texture.surface.pitch * 4 * y];

		for (size_t x = 0; x < fake_framebuffer_width; ++x)
		{
			*out_pointer++ = *in_pointer++;
			*out_pointer++ = *in_pointer++;
			*out_pointer++ = *in_pointer++;
			*out_pointer++ = 0;
		}
	}

	GX2RUnlockSurfaceEx(&screen_texture.surface, 0, (GX2RResourceFlags)0);

	WHBGfxBeginRender();

	// Draw to the TV
	WHBGfxBeginRenderTV();
	GX2SetViewport(tv_viewport.x, tv_viewport.y, tv_viewport.width, tv_viewport.height, 0.0f, 1.0f);
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GX2SetFetchShader(&shader_group.fetchShader);
	GX2SetVertexShader(shader_group.vertexShader);
	GX2SetPixelShader(shader_group.pixelShader);
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
	WHBGfxFinishRenderTV();

	// Draw to the gamepad
	WHBGfxBeginRenderDRC();
	GX2SetViewport(drc_viewport.x, drc_viewport.y, drc_viewport.width, drc_viewport.height, 0.0f, 1.0f);
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GX2SetFetchShader(&shader_group.fetchShader);
	GX2SetVertexShader(shader_group.vertexShader);
	GX2SetPixelShader(shader_group.pixelShader);
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();
}

void WindowBackend_Software_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;

	// The window doesn't resize on the Wii U
}
