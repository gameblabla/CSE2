// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <gx2/context.h>
#include <gx2/display.h>
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

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef enum RenderMode
{
	MODE_BLANK,
	MODE_DRAW_SURFACE,
	MODE_DRAW_SURFACE_WITH_TRANSPARENCY,
	MODE_COLOUR_FILL,
	MODE_DRAW_GLYPH
} RenderMode;

typedef struct RenderBackend_Surface
{
	GX2Texture texture;
	GX2ColorBuffer colour_buffer;
	bool render_target;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	GX2Texture texture;
} RenderBackend_GlyphAtlas;

typedef struct Coordinate2D
{
	float x;
	float y;
} Coordinate2D;

typedef struct Vertex
{
	Coordinate2D position;
	Coordinate2D texture;
} Vertex;

typedef struct VertexBufferSlot
{
	Vertex vertices[4];
} VertexBufferSlot;

static WHBGfxShaderGroup shader_group_texture;
static WHBGfxShaderGroup shader_group_texture_colour_key;
static WHBGfxShaderGroup shader_group_colour_fill;
static WHBGfxShaderGroup shader_group_glyph;

static GX2RBuffer vertex_buffer;

static GX2Sampler sampler_point;
static GX2Sampler sampler_linear;

static RenderBackend_Surface *framebuffer_surface;
static RenderBackend_Surface *upscaled_framebuffer_surface_tv;
static RenderBackend_Surface *upscaled_framebuffer_surface_drc;

static GX2ContextState *gx2_context;

static VertexBufferSlot *local_vertex_buffer;
static size_t local_vertex_buffer_size;
static size_t current_vertex_buffer_slot;

static RenderMode last_render_mode;
static GX2Texture *last_source_texture;
static GX2Texture *last_destination_texture;

static const unsigned char shader_colour_fill[] = {
	#include "WiiUShaders/colour_fill.gsh.h"
};

static const unsigned char shader_glyph[] = {
	#include "WiiUShaders/glyph.gsh.h"
};

static const unsigned char shader_texture[] = {
	#include "WiiUShaders/texture.gsh.h"
};

static const unsigned char shader_texture_colour_key[] = {
	#include "WiiUShaders/texture_colour_key.gsh.h"
};

static VertexBufferSlot* GetVertexBufferSlot(void)
{
	++current_vertex_buffer_slot;

	// Check if buffer needs expanding
	if (current_vertex_buffer_slot > local_vertex_buffer_size)
	{
		local_vertex_buffer_size = 1;

		while (current_vertex_buffer_slot > local_vertex_buffer_size)
			local_vertex_buffer_size <<= 1;

		VertexBufferSlot *new_vertex_buffer = (VertexBufferSlot*)realloc(local_vertex_buffer, local_vertex_buffer_size * sizeof(VertexBufferSlot));

		if (new_vertex_buffer != NULL)
		{
			local_vertex_buffer = new_vertex_buffer;
		}
		else
		{
			Backend_PrintError("Couldn't expand vertex buffer");
			return NULL;
		}
	}

	return &local_vertex_buffer[current_vertex_buffer_slot - 1];
}

static void FlushVertexBuffer(void)
{
	static size_t vertex_buffer_size = 1;

	if (current_vertex_buffer_slot == 0)
		return;

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Upload vertex buffer to VBO, growing it if necessary
	if (local_vertex_buffer_size > vertex_buffer_size)
	{
		vertex_buffer_size = local_vertex_buffer_size;

		GX2RDestroyBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

		vertex_buffer.elemCount = vertex_buffer_size;

		GX2RCreateBuffer(&vertex_buffer);	// We're basically screwed if this fails

		GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
		GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));
	}

	void *vertex_pointer = GX2RLockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	memcpy(vertex_pointer, local_vertex_buffer, current_vertex_buffer_slot * sizeof(VertexBufferSlot));

	GX2RUnlockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4 * current_vertex_buffer_slot, 0, 1);

	current_vertex_buffer_slot = 0;
}

static void Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *source_rect, RenderBackend_Surface *destination_surface, const RenderBackend_Rect *destination_rect, bool colour_key)
{
	const RenderMode render_mode = (colour_key ? MODE_DRAW_SURFACE_WITH_TRANSPARENCY : MODE_DRAW_SURFACE);

	// Flush vertex data if a context-change is needed
	if (last_render_mode != render_mode || last_source_texture != &source_surface->texture || last_destination_texture != &destination_surface->texture)
	{
		FlushVertexBuffer();

		last_render_mode = render_mode;
		last_source_texture = &source_surface->texture;
		last_destination_texture = &destination_surface->texture;

		// Draw to the selected texture, instead of the screen
		GX2SetColorBuffer(&destination_surface->colour_buffer, GX2_RENDER_TARGET_0);
		GX2SetViewport(0.0f, 0.0f, (float)destination_surface->colour_buffer.surface.width, (float)destination_surface->colour_buffer.surface.height, 0.0f, 1.0f);
		GX2SetScissor(0, 0, destination_surface->colour_buffer.surface.width, destination_surface->colour_buffer.surface.height);

		// Select shader
		WHBGfxShaderGroup *shader = colour_key ? &shader_group_texture_colour_key : &shader_group_texture;

		// Bind it
		GX2SetFetchShader(&shader->fetchShader);
		GX2SetVertexShader(shader->vertexShader);
		GX2SetPixelShader(shader->pixelShader);

		// Set shader uniforms
		const float vertex_coordinate_transform[4] = {2.0f / destination_surface->texture.surface.width, -2.0f / destination_surface->texture.surface.height, 1.0f, 1.0f};
		GX2SetVertexUniformReg(shader->vertexShader->uniformVars[0].offset, 4, (uint32_t*)vertex_coordinate_transform);

		const float texture_coordinate_transform[4] = {1.0f / source_surface->texture.surface.width, 1.0f / source_surface->texture.surface.height, 1.0f, 1.0f};
		GX2SetVertexUniformReg(shader->vertexShader->uniformVars[1].offset, 4, (uint32_t*)texture_coordinate_transform);

		// Bind misc. data
		GX2SetPixelSampler(&sampler_point, shader->pixelShader->samplerVars[0].location);
		GX2SetPixelTexture(&source_surface->texture, shader->pixelShader->samplerVars[0].location);

		// Disable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);
	}

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = destination_rect->left;
		const float vertex_top = destination_rect->top;
		const float vertex_right = destination_rect->right;
		const float vertex_bottom = destination_rect->bottom;

		vertex_buffer_slot->vertices[0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1].position.y = vertex_top;
		vertex_buffer_slot->vertices[2].position.x = vertex_right;
		vertex_buffer_slot->vertices[2].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[3].position.x = vertex_left;
		vertex_buffer_slot->vertices[3].position.y = vertex_bottom;

		// Set texture coordinate buffer
		const float texture_left = source_rect->left;
		const float texture_top = source_rect->top;
		const float texture_right = source_rect->right;
		const float texture_bottom = source_rect->bottom;

		vertex_buffer_slot->vertices[0].texture.x = texture_left;
		vertex_buffer_slot->vertices[0].texture.y = texture_top;
		vertex_buffer_slot->vertices[1].texture.x = texture_right;
		vertex_buffer_slot->vertices[1].texture.y = texture_top;
		vertex_buffer_slot->vertices[2].texture.x = texture_right;
		vertex_buffer_slot->vertices[2].texture.y = texture_bottom;
		vertex_buffer_slot->vertices[3].texture.x = texture_left;
		vertex_buffer_slot->vertices[3].texture.y = texture_bottom;
	}
}

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
	(void)window_title;
	(void)fullscreen;

	if (WHBGfxInit())
	{
		// Initialise the shaders

		// Texture shader
		if (WHBGfxLoadGFDShaderGroup(&shader_group_texture, 0, shader_texture))
		{
			WHBGfxInitShaderAttribute(&shader_group_texture, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitShaderAttribute(&shader_group_texture, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitFetchShader(&shader_group_texture);

			// Texture shader (with colour-key)
			if (WHBGfxLoadGFDShaderGroup(&shader_group_texture_colour_key, 0, shader_texture_colour_key))
			{
				WHBGfxInitShaderAttribute(&shader_group_texture_colour_key, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitShaderAttribute(&shader_group_texture_colour_key, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitFetchShader(&shader_group_texture_colour_key);

				// Colour-fill shader
				if (WHBGfxLoadGFDShaderGroup(&shader_group_colour_fill, 0, shader_colour_fill))
				{
					WHBGfxInitShaderAttribute(&shader_group_colour_fill, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
					WHBGfxInitFetchShader(&shader_group_colour_fill);

					// Glyph shader
					if (WHBGfxLoadGFDShaderGroup(&shader_group_glyph, 0, shader_glyph))
					{
						WHBGfxInitShaderAttribute(&shader_group_glyph, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
						WHBGfxInitShaderAttribute(&shader_group_glyph, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
						WHBGfxInitFetchShader(&shader_group_glyph);

						// Initialise samplers
						GX2InitSampler(&sampler_point, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_POINT);
						GX2InitSampler(&sampler_linear, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_LINEAR);

						// Initialise vertex buffer
						vertex_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
																  GX2R_RESOURCE_USAGE_CPU_READ |
																  GX2R_RESOURCE_USAGE_CPU_WRITE |
																  GX2R_RESOURCE_USAGE_GPU_READ);
						vertex_buffer.elemSize = sizeof(VertexBufferSlot);
						vertex_buffer.elemCount = 1;

						if (GX2RCreateBuffer(&vertex_buffer))
						{
							// Create framebuffer surface
							framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

							if (framebuffer_surface != NULL)
							{
								// Create upscaled framebuffer surface
								size_t tv_width, tv_height;

								switch (GX2GetSystemTVScanMode())
								{
									case GX2_TV_SCAN_MODE_480I:
									case GX2_TV_SCAN_MODE_480P:
										tv_width = 854;
										tv_height = 480;
										break;

									case GX2_TV_SCAN_MODE_576I:
									case GX2_TV_SCAN_MODE_720P:
										tv_width = 1280;
										tv_height = 720;
										break;

									case GX2_TV_SCAN_MODE_1080I:
									case GX2_TV_SCAN_MODE_1080P:
										tv_width = 1920;
										tv_height = 1080;
										break;
								}

								size_t upscale_factor = MAX(1, MIN((tv_width + screen_width / 2) / screen_width, (tv_height + screen_height / 2) / screen_height));

								upscaled_framebuffer_surface_tv = RenderBackend_CreateSurface(screen_width * upscale_factor, screen_height * upscale_factor, true);

								if (upscaled_framebuffer_surface_tv != NULL)
								{
									// Create upscaled framebuffer surface
									size_t drc_width = 854;
									size_t drc_height = 480;

									size_t upscale_factor = MAX(1, MIN((drc_width + screen_width / 2) / screen_width, (drc_height + screen_height / 2) / screen_height));

									upscaled_framebuffer_surface_drc = RenderBackend_CreateSurface(screen_width * upscale_factor, screen_height * upscale_factor, true);

									if (upscaled_framebuffer_surface_drc != NULL)
									{
										// From what I can tell, there isn't a 'global context' in GX2: instead there are context objects.
										// wut internally uses (and *switches to*) its own contexts, so we need to maintain one too,
										// and make sure we're always switching back to it when wut is done doing what it's doing.
										gx2_context = (GX2ContextState*)aligned_alloc(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

										if (gx2_context != NULL)
										{
											memset(gx2_context, 0, sizeof(GX2ContextState));
											GX2SetupContextStateEx(gx2_context, TRUE);
											GX2SetContextState(gx2_context);

											// Disable depth-test (enabled by default for some reason)
											GX2SetDepthOnlyControl(FALSE, FALSE, GX2_COMPARE_FUNC_ALWAYS);

											GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
											GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

											return framebuffer_surface;
										}
										else
										{
											Backend_PrintError("Couldn't allocate memory for the GX2 context");
										}

										RenderBackend_FreeSurface(upscaled_framebuffer_surface_drc);
									}
									else
									{
										Backend_PrintError("Couldn't create the DRC upscaled framebuffer surface");
									}

									RenderBackend_FreeSurface(upscaled_framebuffer_surface_tv);
								}
								else
								{
									Backend_PrintError("Couldn't create the TV upscaled framebuffer surface");
								}

								RenderBackend_FreeSurface(framebuffer_surface);
							}
							else
							{
								Backend_PrintError("Couldn't create the framebuffer surface");
							}
						}
						else
						{
							Backend_PrintError("Couldn't create the vertex buffer");
						}

						WHBGfxFreeShaderGroup(&shader_group_glyph);
					}
					else
					{
						Backend_PrintError("Couldn't create the glyph shader");
					}

					WHBGfxFreeShaderGroup(&shader_group_colour_fill);
				}
				else
				{
					Backend_PrintError("Couldn't create the colour-fill shader");
				}

				WHBGfxFreeShaderGroup(&shader_group_texture_colour_key);
			}
			else
			{
				Backend_PrintError("Couldn't create the texture colour-key shader");
			}

			WHBGfxFreeShaderGroup(&shader_group_texture);
		}
		else
		{
			Backend_PrintError("Couldn't create the texture shader");
		}

		WHBGfxShutdown();
	}
	else
	{
		Backend_PrintError("WHBGfxInit failed");
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	free(gx2_context);

	RenderBackend_FreeSurface(framebuffer_surface);

	GX2RDestroyBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	WHBGfxFreeShaderGroup(&shader_group_glyph);
	WHBGfxFreeShaderGroup(&shader_group_colour_fill);
	WHBGfxFreeShaderGroup(&shader_group_texture_colour_key);
	WHBGfxFreeShaderGroup(&shader_group_texture);

	WHBGfxShutdown();
}

void RenderBackend_DrawScreen(void)
{
	FlushVertexBuffer();
	last_render_mode = MODE_BLANK;
	last_source_texture = NULL;
	last_destination_texture = NULL;

	RenderBackend_Rect source_rect;

	source_rect.left = 0;
	source_rect.top = 0;
	source_rect.right = framebuffer_surface->texture.surface.width;
	source_rect.bottom = framebuffer_surface->texture.surface.height;

	RenderBackend_Rect destination_rect;
	destination_rect.left = 0;
	destination_rect.top = 0;
	destination_rect.right = upscaled_framebuffer_surface_tv->texture.surface.width;
	destination_rect.bottom = upscaled_framebuffer_surface_tv->texture.surface.height;

	Blit(framebuffer_surface, &source_rect, upscaled_framebuffer_surface_tv, &destination_rect, false);

	destination_rect.right = upscaled_framebuffer_surface_drc->texture.surface.width;
	destination_rect.bottom = upscaled_framebuffer_surface_drc->texture.surface.height;

	Blit(framebuffer_surface, &source_rect, upscaled_framebuffer_surface_drc, &destination_rect, false);

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	VertexBufferSlot *vertex_buffer_slot = (VertexBufferSlot*)GX2RLockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	// Set buffer to (4:3) full-screen
	vertex_buffer_slot->vertices[0].position.x = -12.0f / 16.0f + 1.0f;
	vertex_buffer_slot->vertices[0].position.y = -2.0f;
	vertex_buffer_slot->vertices[1].position.x = 12.0f / 16.0f + 1.0f;
	vertex_buffer_slot->vertices[1].position.y = -2.0f;
	vertex_buffer_slot->vertices[2].position.x = 12.0f / 16.0f + 1.0f;
	vertex_buffer_slot->vertices[2].position.y = 0.0f;
	vertex_buffer_slot->vertices[3].position.x = -12.0f / 16.0f + 1.0f;
	vertex_buffer_slot->vertices[3].position.y = 0.0f;

	// Set buffer to full-texture
	vertex_buffer_slot->vertices[0].texture.x = 0.0f;
	vertex_buffer_slot->vertices[0].texture.y = 1.0f;
	vertex_buffer_slot->vertices[1].texture.x = 1.0f;
	vertex_buffer_slot->vertices[1].texture.y = 1.0f;
	vertex_buffer_slot->vertices[2].texture.x = 1.0f;
	vertex_buffer_slot->vertices[2].texture.y = 0.0f;
	vertex_buffer_slot->vertices[3].texture.x = 0.0f;
	vertex_buffer_slot->vertices[3].texture.y = 0.0f;

	GX2RUnlockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	// Disable blending
	GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);

	// For some dumbass reason, despite being a vec2, this needs padding to a vec4
	const float plain_vec4[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	// Start drawing
	WHBGfxBeginRender();

	////////////////////
	// Draw to the TV //
	////////////////////

	WHBGfxBeginRenderTV();
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// This might be needed? Not sure.
//	GX2RInvalidateSurface(&framebuffer_surface->texture.surface, 0, (GX2RResourceFlags)0);

	// Select texture shader
	GX2SetFetchShader(&shader_group_texture.fetchShader);
	GX2SetVertexShader(shader_group_texture.vertexShader);
	GX2SetPixelShader(shader_group_texture.pixelShader);

	// Set shader uniforms
	GX2SetVertexUniformReg(shader_group_texture.vertexShader->uniformVars[0].offset, 4, (uint32_t*)plain_vec4);
	GX2SetVertexUniformReg(shader_group_texture.vertexShader->uniformVars[1].offset, 4, (uint32_t*)plain_vec4);

	// Bind a few things
	GX2SetPixelSampler(&sampler_linear, shader_group_texture.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&upscaled_framebuffer_surface_tv->texture, shader_group_texture.pixelShader->samplerVars[0].location);
	GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
	GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

	WHBGfxFinishRenderTV();

	/////////////////////////
	// Draw to the gamepad //
	/////////////////////////

	WHBGfxBeginRenderDRC();
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// This might be needed? Not sure.
//	GX2RInvalidateSurface(&framebuffer_surface->texture.surface, 0, (GX2RResourceFlags)0);

	// Select texture shader
	GX2SetFetchShader(&shader_group_texture.fetchShader);
	GX2SetVertexShader(shader_group_texture.vertexShader);
	GX2SetPixelShader(shader_group_texture.pixelShader);

	// Set shader uniforms
	GX2SetVertexUniformReg(shader_group_texture.vertexShader->uniformVars[0].offset, 4, (uint32_t*)plain_vec4);
	GX2SetVertexUniformReg(shader_group_texture.vertexShader->uniformVars[1].offset, 4, (uint32_t*)plain_vec4);

	// Bind a few things
	GX2SetPixelSampler(&sampler_linear, shader_group_texture.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&upscaled_framebuffer_surface_drc->texture, shader_group_texture.pixelShader->samplerVars[0].location);
	GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
	GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();

	// Switch back to our context
	GX2SetContextState(gx2_context);
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->render_target = render_target;

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

		GX2RResourceFlags resource_flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_TEXTURE |
		                                                       GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_CPU_READ |
		                                                       GX2R_RESOURCE_USAGE_GPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ);

		if (render_target)
			resource_flags = (GX2RResourceFlags)(resource_flags | GX2R_RESOURCE_BIND_COLOR_BUFFER);

		if (GX2RCreateSurface(&surface->texture.surface, resource_flags))
		{
			if (!render_target)
			{
				return surface;
			}
			else
			{
				// Initialise colour buffer (needed so the texture can be drawn to)
				memset(&surface->colour_buffer, 0, sizeof(surface->colour_buffer));
				surface->colour_buffer.surface = surface->texture.surface;
				surface->colour_buffer.viewNumSlices = 1;
				GX2InitColorBufferRegs(&surface->colour_buffer);

				if (GX2RCreateSurfaceUserMemory(&surface->colour_buffer.surface, (uint8_t*)surface->texture.surface.image, (uint8_t*)surface->texture.surface.mipmaps, surface->texture.surface.resourceFlags))
					return surface;
				else
					Backend_PrintError("GX2RCreateSurfaceUserMemory failed in RenderBackend_CreateSurface");

				GX2RDestroySurfaceEx(&surface->texture.surface, (GX2RResourceFlags)0);
			}
		}
		else
		{
			Backend_PrintError("GX2RCreateSurface failed in RenderBackend_CreateSurface");
		}

		free(surface);
	}

	return NULL;
}

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	// Flush the vertex buffer if we're about to destroy its texture
	if (&surface->texture == last_source_texture)
	{
		FlushVertexBuffer();
		last_source_texture = NULL;
	}

	if (&surface->texture == last_destination_texture)
	{
		FlushVertexBuffer();
		last_destination_texture = NULL;
	}

	if (surface->render_target)
		GX2RDestroySurfaceEx(&surface->colour_buffer.surface, (GX2RResourceFlags)0);

	GX2RDestroySurfaceEx(&surface->texture.surface, (GX2RResourceFlags)0);
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
	// Flush the vertex buffer if we're about to modify its texture
	if (&surface->texture == last_source_texture || &surface->texture == last_destination_texture)
		FlushVertexBuffer();

	// Convert from RGB24 to RGBA32, and upload it to the GPU texture
	unsigned char *buffer = (unsigned char*)GX2RLockSurfaceEx(&surface->texture.surface, 0, (GX2RResourceFlags)0);

	if (buffer != NULL)
	{
		const unsigned char *in_pointer = pixels;

		for (size_t y = 0; y < height; ++y)
		{
			unsigned char *out_pointer = &buffer[y * surface->texture.surface.pitch * 4];

			for (size_t x = 0; x < width; ++x)
			{
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = *in_pointer++;
				*out_pointer++ = 0;
			}
		}

		GX2RUnlockSurfaceEx(&surface->texture.surface, 0, (GX2RResourceFlags)0);
	}
}

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	RenderBackend_Rect destination_rect = {x, y, x + (rect->right - rect->left), y + (rect->bottom - rect->top)};

	Blit(source_surface, rect, destination_surface, &destination_rect, colour_key);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_COLOUR_FILL || last_destination_texture != &surface->texture || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_COLOUR_FILL;
		last_source_texture = NULL;
		last_destination_texture = &surface->texture;
		last_red = red;
		last_green = green;
		last_blue = blue;

		// Draw to the selected texture, instead of the screen
		GX2SetColorBuffer(&surface->colour_buffer, GX2_RENDER_TARGET_0);
		GX2SetViewport(0.0f, 0.0f, (float)surface->colour_buffer.surface.width, (float)surface->colour_buffer.surface.height, 0.0f, 1.0f);
		GX2SetScissor(0, 0, (float)surface->colour_buffer.surface.width, (float)surface->colour_buffer.surface.height);

		// Bind the colour-fill shader
		GX2SetFetchShader(&shader_group_colour_fill.fetchShader);
		GX2SetVertexShader(shader_group_colour_fill.vertexShader);
		GX2SetPixelShader(shader_group_colour_fill.pixelShader);

		// Set shader uniforms
		const float vertex_coordinate_transform[4] = {2.0f / surface->texture.surface.width, -2.0f / surface->texture.surface.height, 1.0f, 1.0f};
		GX2SetVertexUniformReg(shader_group_colour_fill.vertexShader->uniformVars[0].offset, 4, (uint32_t*)vertex_coordinate_transform);

		const float uniform_colours[4] = {red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f};
		GX2SetPixelUniformReg(shader_group_colour_fill.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

		// Disable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);
	}

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = rect->left;
		const float vertex_top = rect->top;
		const float vertex_right = rect->right;
		const float vertex_bottom = rect->bottom;

		vertex_buffer_slot->vertices[0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1].position.y = vertex_top;
		vertex_buffer_slot->vertices[2].position.x = vertex_right;
		vertex_buffer_slot->vertices[2].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[3].position.x = vertex_left;
		vertex_buffer_slot->vertices[3].position.y = vertex_bottom;
	}
}

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t width, size_t height)
{
	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		// Initialise texture
		memset(&atlas->texture, 0, sizeof(atlas->texture));
		atlas->texture.surface.width = width;
		atlas->texture.surface.height = height;
		atlas->texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8;
		atlas->texture.surface.depth = 1;
		atlas->texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
		atlas->texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
		atlas->texture.surface.mipLevels = 1;
		atlas->texture.viewNumMips = 1;
		atlas->texture.viewNumSlices = 1;
		atlas->texture.compMap = 0x00000000;
		GX2CalcSurfaceSizeAndAlignment(&atlas->texture.surface);
		GX2InitTextureRegs(&atlas->texture);

		if (GX2RCreateSurface(&atlas->texture.surface, (GX2RResourceFlags)(GX2R_RESOURCE_BIND_TEXTURE |
		                                                                   GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_CPU_READ |
		                                                                   GX2R_RESOURCE_USAGE_GPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ)))
		{
			return atlas;
		}
		else
		{
			Backend_PrintError("GX2RCreateSurface failed in RenderBackend_CreateGlyphAtlas");
		}

		free(atlas);
	}

	return NULL;
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	GX2RDestroySurfaceEx(&atlas->texture.surface, (GX2RResourceFlags)0);
	free(atlas);
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height, size_t pitch)
{
	unsigned char *buffer = (unsigned char*)GX2RLockSurfaceEx(&atlas->texture.surface, 0, (GX2RResourceFlags)0);

	const unsigned char *in_pointer = pixels;
	unsigned char *out_pointer = &buffer[y * atlas->texture.surface.pitch + x];

	for (size_t iy = 0; iy < height; ++iy)
	{
		memcpy(out_pointer, in_pointer, width);

		in_pointer += pitch;
		out_pointer += atlas->texture.surface.pitch;
	}

	GX2RUnlockSurfaceEx(&atlas->texture.surface, 0, (GX2RResourceFlags)0);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_DRAW_GLYPH || last_destination_texture != &destination_surface->texture || last_source_texture != &atlas->texture || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_destination_texture = &destination_surface->texture;
		last_source_texture = &atlas->texture;
		last_red = red;
		last_green = green;
		last_blue = blue;

		// Draw to the selected texture, instead of the screen
		GX2SetColorBuffer(&destination_surface->colour_buffer, GX2_RENDER_TARGET_0);
		GX2SetViewport(0.0f, 0.0f, (float)destination_surface->colour_buffer.surface.width, (float)destination_surface->colour_buffer.surface.height, 0.0f, 1.0f);
		GX2SetScissor(0, 0, destination_surface->colour_buffer.surface.width, destination_surface->colour_buffer.surface.height);

		// Select glyph shader
		GX2SetFetchShader(&shader_group_glyph.fetchShader);
		GX2SetVertexShader(shader_group_glyph.vertexShader);
		GX2SetPixelShader(shader_group_glyph.pixelShader);

		// Set shader uniforms
		const float vertex_coordinate_transform[4] = {2.0f / destination_surface->texture.surface.width, -2.0f / destination_surface->texture.surface.height, 1.0f, 1.0f};
		GX2SetVertexUniformReg(shader_group_glyph.vertexShader->uniformVars[0].offset, 4, (uint32_t*)vertex_coordinate_transform);

		const float texture_coordinate_transform[4] = {1.0f / atlas->texture.surface.width, 1.0f / atlas->texture.surface.height, 1.0f, 1.0f};
		GX2SetVertexUniformReg(shader_group_glyph.vertexShader->uniformVars[1].offset, 4, (uint32_t*)texture_coordinate_transform);

		const float uniform_colours[4] = {red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f};
		GX2SetPixelUniformReg(shader_group_glyph.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

		// Bind misc. data
		GX2SetPixelSampler(&sampler_point, shader_group_glyph.pixelShader->samplerVars[0].location);
		GX2SetPixelTexture(&atlas->texture, shader_group_glyph.pixelShader->samplerVars[0].location);

		// Enable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, FALSE, TRUE);
	}
}

void RenderBackend_DrawGlyph(long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = x;
		const float vertex_top = y;
		const float vertex_right = x + glyph_width;
		const float vertex_bottom = y + glyph_height;

		vertex_buffer_slot->vertices[0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1].position.y = vertex_top;
		vertex_buffer_slot->vertices[2].position.x = vertex_right;
		vertex_buffer_slot->vertices[2].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[3].position.x = vertex_left;
		vertex_buffer_slot->vertices[3].position.y = vertex_bottom;

		// Set texture coordinate buffer
		const float texture_left = glyph_x;
		const float texture_top = glyph_y;
		const float texture_right = glyph_x + glyph_width;
		const float texture_bottom = glyph_y + glyph_height;

		vertex_buffer_slot->vertices[0].texture.x = texture_left;
		vertex_buffer_slot->vertices[0].texture.y = texture_top;
		vertex_buffer_slot->vertices[1].texture.x = texture_right;
		vertex_buffer_slot->vertices[1].texture.y = texture_top;
		vertex_buffer_slot->vertices[2].texture.x = texture_right;
		vertex_buffer_slot->vertices[2].texture.y = texture_bottom;
		vertex_buffer_slot->vertices[3].texture.x = texture_left;
		vertex_buffer_slot->vertices[3].texture.y = texture_bottom;
	}
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// Doesn't happen on the Wii U
}

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	(void)width;
	(void)height;

	// Doesn't happen on the Wii U
}
