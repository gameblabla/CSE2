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

#include "WiiUShaders/colour_fill.gsh.h"
#include "WiiUShaders/glyph.gsh.h"
#include "WiiUShaders/texture.gsh.h"
#include "WiiUShaders/texture_colour_key.gsh.h"

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
	size_t width;
	size_t height;
	bool render_target;
	unsigned char *lock_buffer;	// TODO - Dumb
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	GX2Texture texture;
} RenderBackend_GlyphAtlas;

typedef struct Viewport
{
	float x;
	float y;
	float width;
	float height;
} Viewport;

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

static WHBGfxShaderGroup texture_shader;
static WHBGfxShaderGroup texture_colour_key_shader;
static WHBGfxShaderGroup colour_fill_shader;
static WHBGfxShaderGroup glyph_shader;

static GX2RBuffer vertex_buffer;

static GX2Sampler sampler;

static RenderBackend_Surface *framebuffer_surface;

static GX2ContextState *gx2_context;

static RenderBackend_Surface *glyph_destination_surface;

static Viewport tv_viewport;
static Viewport drc_viewport;

static VertexBufferSlot *local_vertex_buffer;
static size_t local_vertex_buffer_size;
static size_t current_vertex_buffer_slot;

static RenderMode last_render_mode;
static GX2Texture *last_source_texture;
static GX2Texture *last_destination_texture;

static VertexBufferSlot* GetVertexBufferSlot(void)
{
	++current_vertex_buffer_slot;

	// Check if buffer needs expanding
	if (current_vertex_buffer_slot > local_vertex_buffer_size)
	{
		local_vertex_buffer_size = 1;

		while (current_vertex_buffer_slot > local_vertex_buffer_size)
			local_vertex_buffer_size <<= 1;

		VertexBufferSlot *realloc_result = (VertexBufferSlot*)realloc(local_vertex_buffer, local_vertex_buffer_size * sizeof(VertexBufferSlot));

		if (realloc_result != NULL)
		{
			local_vertex_buffer = realloc_result;
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
	static size_t vertex_buffer_size;
	static size_t current_vertex_buffer = 0;

	if (current_vertex_buffer_slot == 0)
		return;

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Upload vertex buffer to VBO, growing it if necessary
	if (local_vertex_buffer_size > vertex_buffer_size)
	{
		vertex_buffer_size = local_vertex_buffer_size;

		GX2RDestroyBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

		vertex_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
		                                          GX2R_RESOURCE_USAGE_CPU_READ |
		                                          GX2R_RESOURCE_USAGE_CPU_WRITE |
		                                          GX2R_RESOURCE_USAGE_GPU_READ);
		vertex_buffer.elemSize = sizeof(VertexBufferSlot);
		vertex_buffer.elemCount = vertex_buffer_size;

		if (GX2RCreateBuffer(&vertex_buffer))
		{
			void *vertex_pointer = GX2RLockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

			memcpy(vertex_pointer, local_vertex_buffer, vertex_buffer_size * sizeof(VertexBufferSlot));

			GX2RUnlockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);
		}
	}
	else
	{
		void *vertex_pointer = GX2RLockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

		memcpy(vertex_pointer, local_vertex_buffer, current_vertex_buffer_slot * sizeof(VertexBufferSlot));

		GX2RUnlockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);
	}

	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4 * current_vertex_buffer_slot, 0, 1);

	current_vertex_buffer_slot = 0;
}

static void CalculateViewport(size_t actual_screen_width, size_t actual_screen_height, Viewport *viewport)
{
	if ((float)actual_screen_width / (float)actual_screen_height > (float)framebuffer_surface->width / (float)framebuffer_surface->height)
	{
		viewport->y = 0.0f;
		viewport->height = actual_screen_height;

		viewport->width = framebuffer_surface->width * ((float)actual_screen_height / (float)framebuffer_surface->height);
		viewport->x = (actual_screen_width - viewport->width) / 2;
	}
	else
	{
		viewport->x = 0.0f;
		viewport->width = actual_screen_width;

		viewport->height = framebuffer_surface->height * ((float)actual_screen_width / (float)framebuffer_surface->width);
		viewport->y = (actual_screen_height - viewport->height) / 2;
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
		if (WHBGfxLoadGFDShaderGroup(&texture_shader, 0, rtexture))
		{
			WHBGfxInitShaderAttribute(&texture_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitShaderAttribute(&texture_shader, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
			WHBGfxInitFetchShader(&texture_shader);

			// Texture shader (with colour-key)
			if (WHBGfxLoadGFDShaderGroup(&texture_colour_key_shader, 0, rtexture_colour_key))
			{
				WHBGfxInitShaderAttribute(&texture_colour_key_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitShaderAttribute(&texture_colour_key_shader, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
				WHBGfxInitFetchShader(&texture_colour_key_shader);

				// Colour-fill shader
				if (WHBGfxLoadGFDShaderGroup(&colour_fill_shader, 0, rcolour_fill))
				{
					WHBGfxInitShaderAttribute(&colour_fill_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
					WHBGfxInitFetchShader(&colour_fill_shader);

					// Glyph shader
					if (WHBGfxLoadGFDShaderGroup(&glyph_shader, 0, rglyph))
					{
						WHBGfxInitShaderAttribute(&glyph_shader, "input_vertex_coordinates", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
						WHBGfxInitShaderAttribute(&glyph_shader, "input_texture_coordinates", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
						WHBGfxInitFetchShader(&glyph_shader);

						// Initialise sampler
						GX2InitSampler(&sampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_LINEAR);

						// Create framebuffer surface
						framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

						if (framebuffer_surface != NULL)
						{
							// Create a 'context' (this voodoo magic can be used to undo `GX2SetColorBuffer`,
							// allowing us to draw to the screen once again)
							gx2_context = (GX2ContextState*)aligned_alloc(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

							if (gx2_context != NULL)
							{
								memset(gx2_context, 0, sizeof(GX2ContextState));
								GX2SetupContextStateEx(gx2_context, TRUE);
								GX2SetContextState(gx2_context);

								// Disable depth-test (enabled by default for some reason)
								GX2SetDepthOnlyControl(FALSE, FALSE, GX2_COMPARE_FUNC_ALWAYS);

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

								return framebuffer_surface;
							}
							else
							{
								Backend_PrintError("Couldn't allocate memory for the GX2 context");
							}

							RenderBackend_FreeSurface(framebuffer_surface);
						}
						else
						{
							Backend_PrintError("Couldn't create the framebuffer surface");
						}

						WHBGfxFreeShaderGroup(&glyph_shader);
					}
					else
					{
						Backend_PrintError("Couldn't create the glyph shader");
					}

					WHBGfxFreeShaderGroup(&colour_fill_shader);
				}
				else
				{
					Backend_PrintError("Couldn't create the colour-fill shader");
				}

				WHBGfxFreeShaderGroup(&texture_colour_key_shader);
			}
			else
			{
				Backend_PrintError("Couldn't create the texture colour-key shader");
			}

			WHBGfxFreeShaderGroup(&texture_shader);
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

	WHBGfxFreeShaderGroup(&glyph_shader);
	WHBGfxFreeShaderGroup(&colour_fill_shader);
	WHBGfxFreeShaderGroup(&texture_colour_key_shader);
	WHBGfxFreeShaderGroup(&texture_shader);

	WHBGfxShutdown();
}

void RenderBackend_DrawScreen(void)
{
	FlushVertexBuffer();
	last_render_mode = MODE_BLANK;
	last_source_texture = NULL;
	last_destination_texture = NULL;

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	VertexBufferSlot *vertex_buffer_slot = (VertexBufferSlot*)GX2RLockBufferEx(&vertex_buffer, (GX2RResourceFlags)0);

	// Set buffer to (4:3) full-screen
	vertex_buffer_slot->vertices[0].position.x = -1.0f;
	vertex_buffer_slot->vertices[0].position.y = -1.0f;
	vertex_buffer_slot->vertices[1].position.x =  1.0f;
	vertex_buffer_slot->vertices[1].position.y = -1.0f;
	vertex_buffer_slot->vertices[2].position.x =  1.0f;
	vertex_buffer_slot->vertices[2].position.y =  1.0f;
	vertex_buffer_slot->vertices[3].position.x = -1.0f;
	vertex_buffer_slot->vertices[3].position.y =  1.0f;

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

	// Start drawing
	WHBGfxBeginRender();

	////////////////////
	// Draw to the TV //
	////////////////////

	WHBGfxBeginRenderTV();
	WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set Viewport
	GX2SetViewport(tv_viewport.x, tv_viewport.y, tv_viewport.width, tv_viewport.height, 0.0f, 1.0f);

	// This might be needed? Not sure.
//	GX2RInvalidateSurface(&framebuffer_surface->texture.surface, 0, (GX2RResourceFlags)0);

	// Select texture shader
	GX2SetFetchShader(&texture_shader.fetchShader);
	GX2SetVertexShader(texture_shader.vertexShader);
	GX2SetPixelShader(texture_shader.pixelShader);

	// Bind a few things
	GX2SetPixelSampler(&sampler, texture_shader.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&framebuffer_surface->texture, texture_shader.pixelShader->samplerVars[0].location);
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

	// Set Viewport
	GX2SetViewport(drc_viewport.x, drc_viewport.y, drc_viewport.width, drc_viewport.height, 0.0f, 1.0f);

	// This might be needed? Not sure.
//	GX2RInvalidateSurface(&framebuffer_surface->texture.surface, 0, (GX2RResourceFlags)0);

	// Select texture shader
	GX2SetFetchShader(&texture_shader.fetchShader);
	GX2SetVertexShader(texture_shader.vertexShader);
	GX2SetPixelShader(texture_shader.pixelShader);

	// Bind a few things
	GX2SetPixelSampler(&sampler, texture_shader.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&framebuffer_surface->texture, texture_shader.pixelShader->samplerVars[0].location);
	GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
	GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();

	// Do this or else the screen will never update. I wish I understood why.
	GX2SetContextState(gx2_context);
}

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

	if (surface != NULL)
	{
		surface->width = width;
		surface->height = height;
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
	if (surface != NULL)
	{
		if (surface->render_target)
			GX2RDestroySurfaceEx(&surface->colour_buffer.surface, (GX2RResourceFlags)0);

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

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, size_t *pitch, size_t width, size_t height)
{
	if (surface != NULL)
	{
		// Create a temporary RGB24 buffer (this backend uses RGBA32
		// internally, so we can't just use a locked texture)
		surface->lock_buffer = (unsigned char*)malloc(width * height * 3);
		*pitch = width * 3;

		return surface->lock_buffer;
	}

	return NULL;
}

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, size_t width, size_t height)
{
	if (surface != NULL)
	{
		if (surface->lock_buffer != NULL)
		{
			// Convert from RGB24 to RGBA32, and upload it to the GPU texture
			unsigned char *buffer = (unsigned char*)GX2RLockSurfaceEx(&surface->texture.surface, 0, (GX2RResourceFlags)0);

			const unsigned char *in_pointer = surface->lock_buffer;

			for (size_t y = 0; y < height; ++y)
			{
				unsigned char *out_pointer = &buffer[surface->texture.surface.pitch * 4 * y];

				for (size_t x = 0; x < width; ++x)
				{
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = *in_pointer++;
					*out_pointer++ = 0;
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
		WHBGfxShaderGroup *shader = colour_key ? &texture_colour_key_shader : &texture_shader;

		// Bind it
		GX2SetFetchShader(&shader->fetchShader);
		GX2SetVertexShader(shader->vertexShader);
		GX2SetPixelShader(shader->pixelShader);

		// Bind misc. data
		GX2SetPixelSampler(&sampler, shader->pixelShader->samplerVars[0].location);
		GX2SetPixelTexture(&source_surface->texture, shader->pixelShader->samplerVars[0].location);
		GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
		GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

		// Disable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);
	}

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = x * 2.0f / destination_surface->width - 1.0f;
		const float vertex_top = y * -2.0f / destination_surface->height + 1.0f;
		const float vertex_right = (x + (rect->right - rect->left)) * 2.0f / destination_surface->width - 1.0f;
		const float vertex_bottom = (y + (rect->bottom - rect->top)) * -2.0f / destination_surface->height + 1.0f;

		vertex_buffer_slot->vertices[0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1].position.y = vertex_top;
		vertex_buffer_slot->vertices[2].position.x = vertex_right;
		vertex_buffer_slot->vertices[2].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[3].position.x = vertex_left;
		vertex_buffer_slot->vertices[3].position.y = vertex_bottom;

		const float texture_left = rect->left / (float)source_surface->width;
		const float texture_top = rect->top / (float)source_surface->height;
		const float texture_right = rect->right / (float)source_surface->width;
		const float texture_bottom = rect->bottom / (float)source_surface->height;

		// Set texture coordinate buffer
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

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	if (surface == NULL)
		return;

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
		GX2SetViewport(0, 0, (float)surface->colour_buffer.surface.width, (float)surface->colour_buffer.surface.height, 0.0f, 1.0f);
		GX2SetScissor(0, 0, (float)surface->colour_buffer.surface.width, (float)surface->colour_buffer.surface.height);

		// Set the colour-fill... colour
		const float uniform_colours[4] = {red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f};
		GX2SetPixelUniformReg(colour_fill_shader.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

		// Bind the colour-fill shader
		GX2SetFetchShader(&colour_fill_shader.fetchShader);
		GX2SetVertexShader(colour_fill_shader.vertexShader);
		GX2SetPixelShader(colour_fill_shader.pixelShader);

		// Bind misc. data
		GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));

		// Disable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);
	}

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = rect->left * 2.0f / surface->width - 1.0f;
		const float vertex_top = rect->top * -2.0f / surface->height + 1.0f;
		const float vertex_right = rect->right * 2.0f / surface->width - 1.0f;
		const float vertex_bottom = rect->bottom * -2.0f / surface->height + 1.0f;

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

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t size)
{
	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		// Initialise texture
		memset(&atlas->texture, 0, sizeof(atlas->texture));
		atlas->texture.surface.width = size;
		atlas->texture.surface.height = size;
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

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height)
{
	// Convert from RGB24 to RGBA32, and upload it to the GPU texture
	unsigned char *buffer = (unsigned char*)GX2RLockSurfaceEx(&atlas->texture.surface, 0, (GX2RResourceFlags)0);

	const unsigned char *in_pointer = pixels;
	unsigned char *out_pointer = &buffer[y * atlas->texture.surface.pitch + x];

	for (size_t iy = 0; iy < height; ++iy)
	{
		memcpy(out_pointer, in_pointer, width);

		in_pointer += width;
		out_pointer += atlas->texture.surface.pitch;
	}

	GX2RUnlockSurfaceEx(&atlas->texture.surface, 0, (GX2RResourceFlags)0);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	(void)atlas;

	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	if (destination_surface == NULL)
		return;

	glyph_destination_surface = destination_surface;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_DRAW_GLYPH || last_destination_texture != &glyph_destination_surface->texture || last_source_texture != &atlas->texture || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_destination_texture = &glyph_destination_surface->texture;
		last_source_texture = &atlas->texture;
		last_red = red;
		last_green = green;
		last_blue = blue;

		// Draw to the selected texture, instead of the screen
		GX2SetColorBuffer(&glyph_destination_surface->colour_buffer, GX2_RENDER_TARGET_0);
		GX2SetViewport(0.0f, 0.0f, (float)glyph_destination_surface->colour_buffer.surface.width, (float)glyph_destination_surface->colour_buffer.surface.height, 0.0f, 1.0f);
		GX2SetScissor(0, 0, glyph_destination_surface->colour_buffer.surface.width, glyph_destination_surface->colour_buffer.surface.height);

		// Set the colour
		const float uniform_colours[4] = {red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f};
		GX2SetPixelUniformReg(glyph_shader.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

		// Select glyph shader
		GX2SetFetchShader(&glyph_shader.fetchShader);
		GX2SetVertexShader(glyph_shader.vertexShader);
		GX2SetPixelShader(glyph_shader.pixelShader);

		// Bind misc. data
		GX2SetPixelSampler(&sampler, glyph_shader.pixelShader->samplerVars[0].location);
		GX2SetPixelTexture(&atlas->texture, glyph_shader.pixelShader->samplerVars[0].location);
		GX2RSetAttributeBuffer(&vertex_buffer, 0, sizeof(Vertex), offsetof(Vertex, position));
		GX2RSetAttributeBuffer(&vertex_buffer, 1, sizeof(Vertex), offsetof(Vertex, texture));

		// Enable blending
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, FALSE, TRUE);
	}
}

void RenderBackend_DrawGlyph(RenderBackend_GlyphAtlas *atlas, long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		// Set vertex position buffer
		const float vertex_left = x * 2.0f / glyph_destination_surface->width - 1.0f;
		const float vertex_top = y * -2.0f / glyph_destination_surface->height + 1.0f;
		const float vertex_right = (x + glyph_width) * 2.0f / glyph_destination_surface->width - 1.0f;
		const float vertex_bottom = (y + glyph_height) * -2.0f / glyph_destination_surface->height + 1.0f;

		vertex_buffer_slot->vertices[0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1].position.y = vertex_top;
		vertex_buffer_slot->vertices[2].position.x = vertex_right;
		vertex_buffer_slot->vertices[2].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[3].position.x = vertex_left;
		vertex_buffer_slot->vertices[3].position.y = vertex_bottom;

		const float texture_left = glyph_x / (float)atlas->texture.surface.width;
		const float texture_top = glyph_y / (float)atlas->texture.surface.height;
		const float texture_right = (glyph_x + glyph_width) / (float)atlas->texture.surface.width;
		const float texture_bottom = (glyph_y + glyph_height) / (float)atlas->texture.surface.height;

		// Set texture coordinate buffer
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

void RenderBackend_FlushGlyphs(void)
{
	
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
