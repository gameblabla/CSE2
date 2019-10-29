#include "../Rendering.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include "SDL.h"

#include "../../WindowsWrapper.h"

typedef enum RenderMode
{
	MODE_BLANK,
	MODE_DRAW_SURFACE,
	MODE_DRAW_SURFACE_WITH_TRANSPARENCY,
	MODE_COLOUR_FILL,
	MODE_DRAW_GLYPH
} RenderMode;

typedef struct Backend_Surface
{
	GLuint texture_id;
	unsigned int width;
	unsigned int height;
	unsigned char *pixels;
} Backend_Surface;

typedef struct Backend_Glyph
{
	GLuint texture_id;
	unsigned int width;
	unsigned int height;
	unsigned char pixel_mode;
} Backend_Glyph;

typedef struct Coordinate2D
{
	GLfloat x;
	GLfloat y;
} Coordinate2D;

typedef struct Vertex
{
	Coordinate2D vertex_coordinate;
	Coordinate2D texture_coordinate;
} Vertex;

typedef struct VertexBufferSlot
{
	Vertex vertices[2][3];
} VertexBufferSlot;

static SDL_Window *window;
static SDL_GLContext context;

static GLuint program_texture;
static GLuint program_colour_fill;
static GLuint program_glyph;

static GLint program_colour_fill_uniform_colour;
static GLint program_glyph_uniform_colour;

static GLuint vertex_array_id;
static GLuint vertex_buffer_id;
static GLuint framebuffer_id;

static VertexBufferSlot *vertex_buffer;
static unsigned long current_vertex_buffer_slot;

static RenderMode last_render_mode;

static Backend_Surface framebuffer;

static const GLchar *vertex_shader_plain = " \
#version 150 core\n \
in vec2 input_vertex_coordinates; \
void main() \
{ \
	gl_Position = vec4(input_vertex_coordinates.x, input_vertex_coordinates.y, 0.0, 1.0); \
} \
";

static const GLchar *vertex_shader_texture = " \
#version 150 core\n \
in vec2 input_vertex_coordinates; \
in vec2 input_texture_coordinates; \
out vec2 texture_coordinates; \
void main() \
{ \
	texture_coordinates = input_texture_coordinates; \
	gl_Position = vec4(input_vertex_coordinates.x, input_vertex_coordinates.y, 0.0, 1.0); \
} \
";

static const GLchar *fragment_shader_texture = " \
#version 150 core\n \
uniform sampler2D tex; \
in vec2 texture_coordinates; \
out vec4 fragment; \
void main() \
{ \
	fragment = texture(tex, texture_coordinates); \
} \
";

static const GLchar *fragment_shader_colour_fill = " \
#version 150 core\n \
uniform vec4 colour; \
out vec4 fragment; \
void main() \
{ \
	fragment = colour; \
} \
";

static const GLchar *fragment_shader_glyph = " \
#version 150 core\n \
uniform sampler2D tex; \
uniform vec4 colour; \
in vec2 texture_coordinates; \
out vec4 fragment; \
void main() \
{ \
	float alpha = texture(tex, texture_coordinates).r; \
	fragment = colour * vec4(alpha, alpha, alpha, alpha); \
} \
";

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void* userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)length;
	(void)userParam;

	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
		printf("OpenGL debug: %s\n", message);
}

static GLuint CompileShader(const char *vertex_shader_source, const char *fragment_shader_source)
{
	GLint shader_status;

	GLuint program_id = glCreateProgram();

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
		return 0;

	glAttachShader(program_id, vertex_shader);

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
		return 0;

	glAttachShader(program_id, fragment_shader);

	glBindAttribLocation(program_id, 1, "input_vertex_coordinates");
	glBindAttribLocation(program_id, 2, "input_texture_coordinates");

	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
		return 0;

	return program_id;
}

static VertexBufferSlot* GetVertexBufferSlot(void)
{
	static unsigned long max_slots = 0;

	if (current_vertex_buffer_slot >= max_slots)
	{
		if (max_slots == 0)
			max_slots = 1;
		else
			max_slots <<= 1;

		vertex_buffer = (VertexBufferSlot*)realloc(vertex_buffer, max_slots * sizeof(VertexBufferSlot));
		glBufferData(GL_ARRAY_BUFFER, max_slots * sizeof(VertexBufferSlot), NULL, GL_STREAM_DRAW);
	}

	return &vertex_buffer[current_vertex_buffer_slot++];
}

static void FlushVertexBuffer(void)
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VertexBufferSlot) * current_vertex_buffer_slot, vertex_buffer);
	glDrawArrays(GL_TRIANGLES, 0, 6 * current_vertex_buffer_slot);

	current_vertex_buffer_slot = 0;
}

SDL_Window* Backend_CreateWindow(const char *title, int width, int height)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
}

Backend_Surface* Backend_Init(SDL_Window *p_window, unsigned int internal_screen_width, unsigned int internal_screen_height, BOOL vsync)
{
	window = p_window;

	context = SDL_GL_CreateContext(window);

	SDL_GL_SetSwapInterval(vsync);	// TODO - Handle this failing

	if (glewInit() != GLEW_OK)
		return NULL;

	// Check if the platform supports OpenGL 3.2
	if (!GLEW_VERSION_3_2)
		return NULL;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	// We're using pre-multiplied alpha so we can blend onto textures that have their own alpha
	// http://apoorvaj.io/alpha-compositing-opengl-blending-and-premultiplied-alpha.html
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set up Vertex Array Object
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Set up Vertex Buffer Object
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(VertexBufferSlot), NULL, GL_STREAM_DRAW);

	// Set up the vertex attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, vertex_coordinate));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture_coordinate));

	// Set up our shaders
	program_texture = CompileShader(vertex_shader_texture, fragment_shader_texture);
	program_colour_fill = CompileShader(vertex_shader_plain, fragment_shader_colour_fill);
	program_glyph = CompileShader(vertex_shader_texture, fragment_shader_glyph);

	if (program_texture == 0 || program_colour_fill == 0 || program_glyph == 0)
		printf("Failed to compile shaders\n");

	// Get shader uniforms
	program_colour_fill_uniform_colour = glGetUniformLocation(program_colour_fill, "colour");
	program_glyph_uniform_colour = glGetUniformLocation(program_glyph, "colour");

	// Set up framebuffer (used for surface-to-surface blitting)
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	// Set up framebuffer screen texture (used for screen-to-surface blitting)
	glGenTextures(1, &framebuffer.texture_id);
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, internal_screen_width, internal_screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	framebuffer.width = internal_screen_width;
	framebuffer.height = internal_screen_height;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture_id, 0);
	glViewport(0, 0, framebuffer.width, framebuffer.height);

	return &framebuffer;
}

void Backend_Deinit(void)
{
	glDeleteTextures(1, &framebuffer.texture_id);
	glDeleteFramebuffers(1, &framebuffer_id);
	glDeleteProgram(program_glyph);
	glDeleteProgram(program_colour_fill);
	glDeleteProgram(program_texture);
	glDeleteBuffers(1, &vertex_buffer_id);
	glDeleteVertexArrays(1, &vertex_array_id);
	SDL_GL_DeleteContext(context);
}

void Backend_DrawScreen(void)
{
	FlushVertexBuffer();
	last_render_mode = MODE_BLANK;

	// This would be a good time to use a custom shader to divide the pixels by
	// their alpha, to undo the premultiplied alpha stuff, but the framebuffer
	// is pretty much guaranteed to be fully opaque, and X / 1 == X, so it'd be
	// a waste of processing power.

	// Fit the screen to the window
	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);

	float fit_width, fit_height;

	if ((float)framebuffer.width / framebuffer.height > (float)window_width / window_height)
	{
		fit_width = 1.0f;
		fit_height = ((float)framebuffer.height / framebuffer.width) / ((float)window_height / window_width);
	}
	else
	{
		fit_width = ((float)framebuffer.width / framebuffer.height) / ((float)window_width / window_height);
		fit_height = 1.0f;
	}

	glUseProgram(program_texture);

	glDisable(GL_BLEND);

	// Enable texture coordinates, since this uses textures
	glEnableVertexAttribArray(2);

	// Target actual screen, and not our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, window_width, window_height);

	// Draw framebuffer to screen
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	vertex_buffer_slot->vertices[0][0].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[0][0].texture_coordinate.y = 1.0f;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.y = 1.0f;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.y = 0.0f;

	vertex_buffer_slot->vertices[1][0].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[1][0].texture_coordinate.y = 1.0f;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.y = 0.0f;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.y = 0.0f;

	vertex_buffer_slot->vertices[0][0].vertex_coordinate.x = -fit_width;
	vertex_buffer_slot->vertices[0][0].vertex_coordinate.y = -fit_height;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.x = fit_width;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.y = -fit_height;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.x = fit_width;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.y = fit_height;

	vertex_buffer_slot->vertices[1][0].vertex_coordinate.x = -fit_width;
	vertex_buffer_slot->vertices[1][0].vertex_coordinate.y = -fit_height;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.x = fit_width;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.y = fit_height;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.x = -fit_width;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.y = fit_height;

	FlushVertexBuffer();

	SDL_GL_SwapWindow(window);

	// According to https://www.khronos.org/opengl/wiki/Common_Mistakes#Swap_Buffers
	// the buffer should always be cleared, even if it seems unnecessary
	glClear(GL_COLOR_BUFFER_BIT);

	// Switch back to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	glGenTextures(1, &surface->texture_id);
	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	surface->width = width;
	surface->height = height;

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	glDeleteTextures(1, &surface->texture_id);
	free(surface);
}

BOOL Backend_IsSurfaceLost(Backend_Surface *surface)
{
	(void)surface;

	return FALSE;
}

void Backend_RestoreSurface(Backend_Surface *surface)
{
	(void)surface;
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	surface->pixels = (unsigned char*)malloc(surface->width * surface->height * 4);
	*pitch = surface->width * 4;

	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	// Pre-multiply the colour channels with the alpha, so blending works correctly
	unsigned char *pixels = surface->pixels;

	for (unsigned int y = 0; y < surface->height; ++y)
	{
		for (unsigned int x = 0; x < surface->width; ++x)
		{
			pixels[0] = (pixels[0] * pixels[3]) / 0xFF;
			pixels[1] = (pixels[1] * pixels[3]) / 0xFF;
			pixels[2] = (pixels[2] * pixels[3]) / 0xFF;
			pixels += 4;
		}
	}

	GLint previously_bound_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previously_bound_texture);

	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	free(surface->pixels);

	glBindTexture(GL_TEXTURE_2D, previously_bound_texture);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL alpha_blend)
{
	static Backend_Surface *last_source_surface;
	static Backend_Surface *last_destination_surface;

	if (source_surface == NULL || destination_surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	const RenderMode render_mode = (alpha_blend ? MODE_DRAW_SURFACE_WITH_TRANSPARENCY : MODE_DRAW_SURFACE);

	if (last_render_mode != render_mode || last_source_surface != source_surface || last_destination_surface != destination_surface)
	{
		FlushVertexBuffer();

		last_render_mode = render_mode;
		last_source_surface = source_surface;
		last_destination_surface = destination_surface;

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destination_surface->texture_id, 0);
		glViewport(0, 0, destination_surface->width, destination_surface->height);

		glUseProgram(program_texture);

		if (alpha_blend)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(2);

		glBindTexture(GL_TEXTURE_2D, source_surface->texture_id);
	}

	const GLfloat texture_left = (GLfloat)rect->left / (GLfloat)source_surface->width;
	const GLfloat texture_right = (GLfloat)rect->right / (GLfloat)source_surface->width;
	const GLfloat texture_top = (GLfloat)rect->top / (GLfloat)source_surface->height;
	const GLfloat texture_bottom = (GLfloat)rect->bottom / (GLfloat)source_surface->height;

	const GLfloat vertex_left = (x * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_right = ((x + (rect->right - rect->left)) * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_top = (y * (2.0f / destination_surface->height)) - 1.0f;
	const GLfloat vertex_bottom = ((y + (rect->bottom - rect->top)) * (2.0f / destination_surface->height)) - 1.0f;

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	vertex_buffer_slot->vertices[0][0].texture_coordinate.x = texture_left;
	vertex_buffer_slot->vertices[0][0].texture_coordinate.y = texture_top;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.x = texture_right;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.y = texture_top;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.x = texture_right;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.y = texture_bottom;

	vertex_buffer_slot->vertices[1][0].texture_coordinate.x = texture_left;
	vertex_buffer_slot->vertices[1][0].texture_coordinate.y = texture_top;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.x = texture_right;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.y = texture_bottom;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.x = texture_left;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.y = texture_bottom;

	vertex_buffer_slot->vertices[0][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[0][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.y = vertex_bottom;

	vertex_buffer_slot->vertices[1][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.y = vertex_bottom;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.y = vertex_bottom;
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
{
	static Backend_Surface *last_surface;
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	if (surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	if (last_render_mode != MODE_COLOUR_FILL || last_surface != surface || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_COLOUR_FILL;
		last_surface = surface;
		last_red = red;
		last_green = green;
		last_blue = blue;

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture_id, 0);
		glViewport(0, 0, surface->width, surface->height);

		glUseProgram(program_colour_fill);

		glDisable(GL_BLEND);

		// Disable texture coordinate array, since this doesn't use textures
		glDisableVertexAttribArray(2);

		glUniform4f(program_colour_fill_uniform_colour, red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f);

		current_vertex_buffer_slot = 0;
	}

	const GLfloat vertex_left = (rect->left * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_right = (rect->right * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_top = (rect->top * (2.0f / surface->height)) - 1.0f;
	const GLfloat vertex_bottom = (rect->bottom * (2.0f / surface->height)) - 1.0f;

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	vertex_buffer_slot->vertices[0][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[0][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.y = vertex_bottom;

	vertex_buffer_slot->vertices[1][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.y = vertex_bottom;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.y = vertex_bottom;
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, FontPixelMode pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	const unsigned int destination_pitch = (width + 3) & ~3;	// Round up to the nearest 4 (OpenGL needs this)

	unsigned char *buffer = (unsigned char*)malloc(destination_pitch * height);

	switch (pixel_mode)
	{
		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = buffer + y * destination_pitch;
				memcpy(destination_pointer, source_pointer, width);
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = buffer + y * destination_pitch;

				for (unsigned int x = 0; x < width; ++x)
					*destination_pointer++ = (*source_pointer++ ? 0xFF : 0);
			}

			break;
	}

	GLint previously_bound_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previously_bound_texture);

	glGenTextures(1, &glyph->texture_id);
	glBindTexture(GL_TEXTURE_2D, glyph->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glyph->width = width;
	glyph->height = height;
	glyph->pixel_mode = pixel_mode;

	free(buffer);

	glBindTexture(GL_TEXTURE_2D, previously_bound_texture);

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	glDeleteTextures(1, &glyph->texture_id);
	free(glyph);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	static Backend_Surface *last_surface;
	static Backend_Glyph *last_glyph;
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	if (glyph == NULL || surface == NULL)
		return;

	if (last_render_mode != MODE_DRAW_GLYPH || last_surface != surface || last_glyph != glyph || last_red != colours[0] || last_green != colours[1] || last_blue != colours[2])
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_surface = surface;
		last_glyph = glyph;
		last_red = colours[0];
		last_green = colours[1];
		last_blue = colours[2];

		glUseProgram(program_glyph);
		glUniform4f(program_glyph_uniform_colour, colours[0] / 255.0f, colours[1] / 255.0f, colours[2] / 255.0f, 1.0f);

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture_id, 0);
		glViewport(0, 0, surface->width, surface->height);

		glEnable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(2);

		glBindTexture(GL_TEXTURE_2D, glyph->texture_id);
	}

	const GLfloat vertex_left = (x * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_right = ((x + glyph->width) * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_top = (y * (2.0f / surface->height)) - 1.0f;
	const GLfloat vertex_bottom = ((y + glyph->height) * (2.0f / surface->height)) - 1.0f;

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	vertex_buffer_slot->vertices[0][0].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[0][0].texture_coordinate.y = 0.0f;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][1].texture_coordinate.y = 0.0f;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][2].texture_coordinate.y = 1.0f;

	vertex_buffer_slot->vertices[1][0].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[1][0].texture_coordinate.y = 0.0f;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[1][1].texture_coordinate.y = 1.0f;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.x = 0.0f;
	vertex_buffer_slot->vertices[1][2].texture_coordinate.y = 1.0f;

	vertex_buffer_slot->vertices[0][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[0][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.y = vertex_bottom;

	vertex_buffer_slot->vertices[1][0].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][0].vertex_coordinate.y = vertex_top;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.x = vertex_right;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.y = vertex_bottom;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.x = vertex_left;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.y = vertex_bottom;
}

void Backend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
