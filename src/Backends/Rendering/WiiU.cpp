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

#include "../WiiU/shaders/colour_fill.gsh.h"
#include "../WiiU/shaders/glyph.gsh.h"
#include "../WiiU/shaders/texture.gsh.h"
#include "../WiiU/shaders/texture_colour_key.gsh.h"

typedef struct RenderBackend_Surface
{
	GX2Texture texture;
	GX2ColorBuffer colour_buffer;
	unsigned int width;
	unsigned int height;
	bool render_target;
	unsigned char *lock_buffer;	// TODO - Dumb
} RenderBackend_Surface;

typedef struct RenderBackend_Glyph
{
	GX2Texture texture;
	unsigned int width;
	unsigned int height;
} RenderBackend_Glyph;

typedef struct Viewport
{
	float x;
	float y;
	float width;
	float height;
} Viewport;

static WHBGfxShaderGroup texture_shader;
static WHBGfxShaderGroup texture_colour_key_shader;
static WHBGfxShaderGroup colour_fill_shader;
static WHBGfxShaderGroup glyph_shader;

static GX2RBuffer vertex_position_buffer;
static GX2RBuffer texture_coordinate_buffer;

static GX2Sampler sampler;

static RenderBackend_Surface *framebuffer_surface;

static GX2ContextState *gx2_context;

static RenderBackend_Surface *glyph_destination_surface;

static Viewport tv_viewport;
static Viewport drc_viewport;

static void CalculateViewport(unsigned int actual_screen_width, unsigned int actual_screen_height, Viewport *viewport)
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

RenderBackend_Surface* RenderBackend_Init(const char *window_title, int screen_width, int screen_height, bool fullscreen)
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

						// Initialise vertex position buffer
						vertex_position_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
																		   GX2R_RESOURCE_USAGE_CPU_READ |
																		   GX2R_RESOURCE_USAGE_CPU_WRITE |
																		   GX2R_RESOURCE_USAGE_GPU_READ);
						vertex_position_buffer.elemSize = 2 * sizeof(float);
						vertex_position_buffer.elemCount = 4;

						if (GX2RCreateBuffer(&vertex_position_buffer))
						{
							// Initialise texture coordinate buffer
							texture_coordinate_buffer.flags = (GX2RResourceFlags)(GX2R_RESOURCE_BIND_VERTEX_BUFFER |
																				  GX2R_RESOURCE_USAGE_CPU_READ |
																				  GX2R_RESOURCE_USAGE_CPU_WRITE |
																				  GX2R_RESOURCE_USAGE_GPU_READ);
							texture_coordinate_buffer.elemSize = 2 * sizeof(float);
							texture_coordinate_buffer.elemCount = 4;

							if (GX2RCreateBuffer(&texture_coordinate_buffer))
							{
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
											case GX2_TV_SCAN_MODE_NONE:	// lolwut
												break;

											case GX2_TV_SCAN_MODE_480I:
											case GX2_TV_SCAN_MODE_480P:
												CalculateViewport(854, 480, &tv_viewport);
												break;

											case GX2_TV_SCAN_MODE_720P:
											case 4:	// Why the hell doesn't WUT have an enum for this? It always returns this value for me!
												CalculateViewport(1280, 720, &tv_viewport);
												break;

											case GX2_TV_SCAN_MODE_1080I:
											case GX2_TV_SCAN_MODE_1080P:
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
								}
								else
								{
									Backend_PrintError("Couldn't create the framebuffer surface");
								}

								GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
							}
							else
							{
								Backend_PrintError("Couldn't create the texture coordinate buffer");
							}

							GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);
						}
						else
						{
							Backend_PrintError("Couldn't create the vertex position buffer");
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

	GX2RDestroyBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	GX2RDestroyBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	WHBGfxFreeShaderGroup(&colour_fill_shader);
	WHBGfxFreeShaderGroup(&texture_colour_key_shader);
	WHBGfxFreeShaderGroup(&texture_shader);

	WHBGfxShutdown();
}

void RenderBackend_DrawScreen(void)
{
	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Set buffer to (4:3) full-screen
	float *position_pointer = (float*)GX2RLockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);
	position_pointer[0] = -1.0f;
	position_pointer[1] = -1.0f;
	position_pointer[2] =  1.0f;
	position_pointer[3] = -1.0f;
	position_pointer[4] =  1.0f;
	position_pointer[5] =  1.0f;
	position_pointer[6] = -1.0f;
	position_pointer[7] =  1.0f;
	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	// Set buffer to full-texture
	float *texture_coordinate_pointer = (float*)GX2RLockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	texture_coordinate_pointer[0] = 0.0f;
	texture_coordinate_pointer[1] = 1.0f;
	texture_coordinate_pointer[2] = 1.0f;
	texture_coordinate_pointer[3] = 1.0f;
	texture_coordinate_pointer[4] = 1.0f;
	texture_coordinate_pointer[5] = 0.0f;
	texture_coordinate_pointer[6] = 0.0f;
	texture_coordinate_pointer[7] = 0.0f;
	GX2RUnlockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);

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
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

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
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);

	WHBGfxFinishRenderDRC();

	WHBGfxFinishRender();

	// Do this or else the screen will never update. I wish I understood why.
	GX2SetContextState(gx2_context);
}

RenderBackend_Surface* RenderBackend_CreateSurface(unsigned int width, unsigned int height, bool render_target)
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

unsigned char* RenderBackend_LockSurface(RenderBackend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
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

void RenderBackend_UnlockSurface(RenderBackend_Surface *surface, unsigned int width, unsigned int height)
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

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Set vertex position buffer
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
		position_pointer[i + 0] /= destination_surface->width;
		position_pointer[i + 0] *= 2.0f;
		position_pointer[i + 0] -= 1.0f;

		position_pointer[i + 1] /= destination_surface->height;
		position_pointer[i + 1] *= -2.0f;
		position_pointer[i + 1] += 1.0f;
	}

	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	// Set texture coordinate buffer
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
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Set vertex position buffer
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
		position_pointer[i + 0] /= surface->width;
		position_pointer[i + 0] *= 2.0f;
		position_pointer[i + 0] -= 1.0f;

		position_pointer[i + 1] /= surface->height;
		position_pointer[i + 1] *= -2.0f;
		position_pointer[i + 1] += 1.0f;
	}

	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

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
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
}

RenderBackend_Glyph* RenderBackend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	RenderBackend_Glyph *glyph = (RenderBackend_Glyph*)malloc(sizeof(RenderBackend_Glyph));

	if (glyph != NULL)
	{
		glyph->width = width;
		glyph->height = height;

		// Initialise texture
		memset(&glyph->texture, 0, sizeof(glyph->texture));
		glyph->texture.surface.width = width;
		glyph->texture.surface.height = height;
		glyph->texture.surface.format = GX2_SURFACE_FORMAT_UNORM_R8;
		glyph->texture.surface.depth = 1;
		glyph->texture.surface.dim = GX2_SURFACE_DIM_TEXTURE_2D;
		glyph->texture.surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
		glyph->texture.surface.mipLevels = 1;
		glyph->texture.viewNumMips = 1;
		glyph->texture.viewNumSlices = 1;
		glyph->texture.compMap = 0x00000000;
		GX2CalcSurfaceSizeAndAlignment(&glyph->texture.surface);
		GX2InitTextureRegs(&glyph->texture);

		if (GX2RCreateSurface(&glyph->texture.surface, (GX2RResourceFlags)(GX2R_RESOURCE_BIND_TEXTURE |
		                                                                   GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_CPU_READ |
		                                                                   GX2R_RESOURCE_USAGE_GPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ)))
		{
			// Convert from RGB24 to RGBA32, and upload it to the GPU texture
			unsigned char *framebuffer = (unsigned char*)GX2RLockSurfaceEx(&glyph->texture.surface, 0, (GX2RResourceFlags)0);

			const unsigned char *in_pointer = pixels;
			unsigned char *out_pointer = framebuffer;

			for (size_t y = 0; y < height; ++y)
			{
				memcpy(out_pointer, in_pointer, width);

				in_pointer += pitch;
				out_pointer += glyph->texture.surface.pitch;
			}

			GX2RUnlockSurfaceEx(&glyph->texture.surface, 0, (GX2RResourceFlags)0);

			return glyph;
		}
		else
		{
			Backend_PrintError("GX2RCreateSurface failed in RenderBackend_LoadGlyph");
		}

		free(glyph);
	}


	return NULL;
}

void RenderBackend_UnloadGlyph(RenderBackend_Glyph *glyph)
{
	if (glyph != NULL)
	{
		GX2RDestroySurfaceEx(&glyph->texture.surface, (GX2RResourceFlags)0);
		free(glyph);
	}
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_Surface *destination_surface, const unsigned char *colour_channels)
{
	if (destination_surface == NULL)
		return;

	glyph_destination_surface = destination_surface;

	// Set the colour
	const float uniform_colours[4] = {colour_channels[0] / 255.0f, colour_channels[1] / 255.0f, colour_channels[2] / 255.0f, 1.0f};
	GX2SetPixelUniformReg(glyph_shader.pixelShader->uniformVars[0].offset, 4, (uint32_t*)&uniform_colours);

	// Enable blending
	GX2SetColorControl(GX2_LOGIC_OP_COPY, 0xFF, FALSE, TRUE);
}

void RenderBackend_DrawGlyph(RenderBackend_Glyph *glyph, long x, long y)
{
	if (glyph == NULL)
		return;

	// Make sure the buffers aren't currently being used before we modify them
	GX2DrawDone();

	// Set vertex position buffer
	const float destination_left = x;
	const float destination_top = y;
	const float destination_right = x + glyph->width;
	const float destination_bottom = y + glyph->height;

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
		position_pointer[i + 0] /= glyph_destination_surface->width;
		position_pointer[i + 0] *= 2.0f;
		position_pointer[i + 0] -= 1.0f;

		position_pointer[i + 1] /= glyph_destination_surface->height;
		position_pointer[i + 1] *= -2.0f;
		position_pointer[i + 1] += 1.0f;
	}

	GX2RUnlockBufferEx(&vertex_position_buffer, (GX2RResourceFlags)0);

	// Set texture coordinate buffer
	float *texture_coordinate_pointer = (float*)GX2RLockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);
	texture_coordinate_pointer[0] = 0.0f;
	texture_coordinate_pointer[1] = 0.0f;
	texture_coordinate_pointer[2] = 1.0f;
	texture_coordinate_pointer[3] = 0.0f;
	texture_coordinate_pointer[4] = 1.0f;
	texture_coordinate_pointer[5] = 1.0f;
	texture_coordinate_pointer[6] = 0.0f;
	texture_coordinate_pointer[7] = 1.0f;
	GX2RUnlockBufferEx(&texture_coordinate_buffer, (GX2RResourceFlags)0);

	// Draw to the selected texture, instead of the screen
	GX2SetColorBuffer(&glyph_destination_surface->colour_buffer, GX2_RENDER_TARGET_0);
	GX2SetViewport(0.0f, 0.0f, (float)glyph_destination_surface->colour_buffer.surface.width, (float)glyph_destination_surface->colour_buffer.surface.height, 0.0f, 1.0f);
	GX2SetScissor(0, 0, glyph_destination_surface->colour_buffer.surface.width, glyph_destination_surface->colour_buffer.surface.height);

	// Select glyph shader
	GX2SetFetchShader(&glyph_shader.fetchShader);
	GX2SetVertexShader(glyph_shader.vertexShader);
	GX2SetPixelShader(glyph_shader.pixelShader);

	// Bind misc. data
	GX2SetPixelSampler(&sampler, glyph_shader.pixelShader->samplerVars[0].location);
	GX2SetPixelTexture(&glyph->texture, glyph_shader.pixelShader->samplerVars[0].location);
	GX2RSetAttributeBuffer(&vertex_position_buffer, 0, vertex_position_buffer.elemSize, 0);
	GX2RSetAttributeBuffer(&texture_coordinate_buffer, 1, texture_coordinate_buffer.elemSize, 0);

	// Draw
	GX2DrawEx(GX2_PRIMITIVE_MODE_QUADS, 4, 0, 1);
}

void RenderBackend_FlushGlyphs(void)
{
	// Disable blending
	GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, FALSE, TRUE);
}

void RenderBackend_HandleRenderTargetLoss(void)
{
	// Doesn't happen on the Wii U
}

void RenderBackend_HandleWindowResize(unsigned int width, unsigned int height)
{
	(void)width;
	(void)height;

	// Doesn't happen on the Wii U
}
