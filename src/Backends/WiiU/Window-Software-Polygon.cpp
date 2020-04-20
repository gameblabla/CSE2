// Sexy new backend that bounces the software-rendered frame to the GPU,
// eliminating V-tearing, and gaining support for rendering to the TV for
// free!

#include "../Window-Software.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <gx2/draw.h>
#include <gx2/sampler.h>
#include <gx2/texture.h>
#include <gx2r/buffer.h>
#include <gx2r/draw.h>
#include <gx2r/resource.h>
#include <gx2r/surface.h>
#include <whb/gfx.h>

#include "../../Attributes.h"

#include "newshader.gsh.h"

static unsigned char *fake_framebuffer;

static size_t fake_framebuffer_width;
static size_t fake_framebuffer_height;

static WHBGfxShaderGroup shader_group;

static GX2RBuffer vertex_position_buffer;
static GX2RBuffer texture_coordinate_buffer;

static GX2Sampler sampler;

static GX2Texture screen_texture;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	fake_framebuffer_width = screen_width;
	fake_framebuffer_height = screen_height;

	fake_framebuffer = (unsigned char*)malloc(fake_framebuffer_width * fake_framebuffer_height * 3);

	if (fake_framebuffer != NULL)
	{
		WHBGfxInit();

		if (WHBGfxLoadGFDShaderGroup(&shader_group, 0, rnewshader))
		{
			WHBGfxInitShaderAttribute(&shader_group, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitShaderAttribute(&shader_group, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitFetchShader(&shader_group);

			// Initialise vertex position buffer
			const float vertex_positions[4][2] = {
				{-640.0f / 854.0f,  1.0f},
				{ 640.0f / 854.0f,  1.0f},
				{ 640.0f / 854.0f, -1.0f},
				{-640.0f / 854.0f, -1.0f}
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
				return true;
			}

			GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
			GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);
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
	unsigned char *out_pointer = framebuffer;

	for (size_t y = 0; y < fake_framebuffer_height; ++y)
	{
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
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GX2SetPixelTexture(&screen_texture, shader_group.pixelShader->samplerVars[0].location);
	GX2SetPixelSampler(&sampler, shader_group.pixelShader->samplerVars[0].location);
	GX2SetFetchShader(&shader_group.fetchShader);
	GX2SetVertexShader(shader_group.vertexShader);
	GX2SetPixelShader(shader_group.pixelShader);
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
	WHBGfxFinishRenderTV();

	// Draw to the gamepad
	WHBGfxBeginRenderDRC();
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GX2SetPixelTexture(&screen_texture, shader_group.pixelShader->samplerVars[0].location);
	GX2SetPixelSampler(&sampler, shader_group.pixelShader->samplerVars[0].location);
	GX2SetFetchShader(&shader_group.fetchShader);
	GX2SetVertexShader(shader_group.vertexShader);
	GX2SetPixelShader(shader_group.pixelShader);
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// The window doesn't resize on the Wii U
}
