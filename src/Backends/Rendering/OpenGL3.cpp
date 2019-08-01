#include "../Rendering.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../../Font.h"

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
} Backend_Glyph;

typedef struct Coordinate2D
{
	GLfloat x;
	GLfloat y;
} Coordinate2D;

typedef struct VertexBuffer
{
	Coordinate2D vertexes[4];
	Coordinate2D texture_coordinates[4];
} VertexBuffer;

static SDL_Window *window;
static SDL_GLContext context;

static GLuint program_texture;
static GLuint program_texture_colour_key;
static GLuint program_colour_fill;
static GLuint program_glyph;

static GLint program_colour_fill_uniform_colour;
static GLint program_glyph_uniform_colour;

static GLuint vertex_array_id;
static GLuint vertex_buffer_id;
static GLuint framebuffer_id;

static VertexBuffer vertex_buffer;

static Backend_Surface framebuffer_surface;

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
	fragment = texture2D(tex, texture_coordinates); \
} \
";

static const GLchar *fragment_shader_texture_colour_key = " \
#version 150 core\n \
uniform sampler2D tex; \
in vec2 texture_coordinates; \
out vec4 fragment; \
void main() \
{ \
	vec4 colour = texture2D(tex, texture_coordinates); \
\
	if (colour.xyz == vec3(0.0f, 0.0f, 0.0f)) \
		discard; \
\
	fragment = colour; \
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
	fragment = colour * vec4(1.0, 1.0, 1.0, texture2D(tex, texture_coordinates).r); \
} \
";

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void* userParam)
{
	(void)source;
	(void)id;
	(void)severity;
	(void)length;
	(void)userParam;

	if (type == GL_DEBUG_TYPE_ERROR)
		printf("OpenGL error: %s\n", message);
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

SDL_Window* Backend_CreateWindow(const char *title, int width, int height)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	return SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
}

BOOL Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	int window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);

	context = SDL_GL_CreateContext(window);

	if (glewInit() != GLEW_OK)
		return FALSE;

	// Check if the platform supports OpenGL 3.2
	if (!GLEW_VERSION_3_2)
		return FALSE;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set up Vertex Array Object
	glGenVertexArrays(1, &vertex_array_id);
	glBindVertexArray(vertex_array_id);

	// Set up Vertex Buffer Object
	glGenBuffers(1, &vertex_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), NULL, GL_DYNAMIC_DRAW);

	// Set up the vertex attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offsetof(VertexBuffer, vertexes));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)offsetof(VertexBuffer, texture_coordinates));

	// Set up our shaders
	program_texture = CompileShader(vertex_shader_texture, fragment_shader_texture);
	program_texture_colour_key = CompileShader(vertex_shader_texture, fragment_shader_texture_colour_key);
	program_colour_fill = CompileShader(vertex_shader_plain, fragment_shader_colour_fill);
	program_glyph = CompileShader(vertex_shader_texture, fragment_shader_glyph);

	if (program_texture == 0 || program_texture_colour_key == 0 || program_colour_fill == 0 || program_glyph == 0)
		printf("Failed to compile shaders\n");

	// Get shader uniforms
	program_colour_fill_uniform_colour = glGetUniformLocation(program_colour_fill, "colour");
	program_glyph_uniform_colour = glGetUniformLocation(program_glyph, "colour");

	// Set up framebuffer (used for surface-to-surface blitting)
	glGenFramebuffers(1, &framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

	// Set up framebuffer screen texture (used for screen-to-surface blitting)
	glGenTextures(1, &framebuffer_surface.texture_id);
	glBindTexture(GL_TEXTURE_2D, framebuffer_surface.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	framebuffer_surface.width = window_width;
	framebuffer_surface.height = window_height;

	return TRUE;
}

void Backend_Deinit(void)
{
	glDeleteTextures(1, &framebuffer_surface.texture_id);
	glDeleteFramebuffers(1, &framebuffer_id);
	glDeleteProgram(program_glyph);
	glDeleteProgram(program_colour_fill);
	glDeleteProgram(program_texture_colour_key);
	glDeleteProgram(program_texture);
	glDeleteBuffers(1, &vertex_buffer_id);
	glDeleteVertexArrays(1, &vertex_array_id);
	SDL_GL_DeleteContext(context);
}

void Backend_DrawScreen(void)
{
	glUseProgram(program_texture);

	glDisable(GL_BLEND);

	// Enable texture coordinates, since this uses textures
	glEnableVertexAttribArray(2);

	// Target actual screen, and not our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, framebuffer_surface.width, framebuffer_surface.height);

	// Draw framebuffer to screen
	glBindTexture(GL_TEXTURE_2D, framebuffer_surface.texture_id);

	vertex_buffer.texture_coordinates[0].x = 0.0f;
	vertex_buffer.texture_coordinates[0].y = 1.0f;
	vertex_buffer.texture_coordinates[1].x = 1.0f;
	vertex_buffer.texture_coordinates[1].y = 1.0f;
	vertex_buffer.texture_coordinates[2].x = 1.0f;
	vertex_buffer.texture_coordinates[2].y = 0.0f;
	vertex_buffer.texture_coordinates[3].x = 0.0f;
	vertex_buffer.texture_coordinates[3].y = 0.0f;

	vertex_buffer.vertexes[0].x = -1.0f;
	vertex_buffer.vertexes[0].y = -1.0f;
	vertex_buffer.vertexes[1].x = 1.0f;
	vertex_buffer.vertexes[1].y = -1.0f;
	vertex_buffer.vertexes[2].x = 1.0f;
	vertex_buffer.vertexes[2].y = 1.0f;
	vertex_buffer.vertexes[3].x = -1.0f;
	vertex_buffer.vertexes[3].y = 1.0f;

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer), &vertex_buffer);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch)
{
	if (surface == NULL)
		return NULL;

	surface->pixels = (unsigned char*)malloc(surface->width * surface->height * 3);
	*pitch = surface->width * 3;
	return surface->pixels;
}

void Backend_Unlock(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	free(surface->pixels);
}

static void BlitCommon(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	// Point our framebuffer to the destination texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destination_surface->texture_id, 0);
	glViewport(0, 0, destination_surface->width, destination_surface->height);

	// Switch to colour-key shader if we have to
	glUseProgram(colour_key ? program_texture_colour_key : program_texture);

	glDisable(GL_BLEND);

	// Enable texture coordinates, since this uses textures
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, source_surface->texture_id);

	const GLfloat texture_left = (GLfloat)rect->left / (GLfloat)source_surface->width;
	const GLfloat texture_right = (GLfloat)rect->right / (GLfloat)source_surface->width;
	const GLfloat texture_top = (GLfloat)rect->top / (GLfloat)source_surface->height;
	const GLfloat texture_bottom = (GLfloat)rect->bottom / (GLfloat)source_surface->height;

	const GLfloat vertex_left = (x * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_right = ((x + (rect->right - rect->left)) * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_top = (y * (2.0f / destination_surface->height)) - 1.0f;
	const GLfloat vertex_bottom = ((y + (rect->bottom - rect->top)) * (2.0f / destination_surface->height)) - 1.0f;

	vertex_buffer.texture_coordinates[0].x = texture_left;
	vertex_buffer.texture_coordinates[0].y = texture_top;
	vertex_buffer.texture_coordinates[1].x = texture_right;
	vertex_buffer.texture_coordinates[1].y = texture_top;
	vertex_buffer.texture_coordinates[2].x = texture_right;
	vertex_buffer.texture_coordinates[2].y = texture_bottom;
	vertex_buffer.texture_coordinates[3].x = texture_left;
	vertex_buffer.texture_coordinates[3].y = texture_bottom;

	vertex_buffer.vertexes[0].x = vertex_left;
	vertex_buffer.vertexes[0].y = vertex_top;
	vertex_buffer.vertexes[1].x = vertex_right;
	vertex_buffer.vertexes[1].y = vertex_top;
	vertex_buffer.vertexes[2].x = vertex_right;
	vertex_buffer.vertexes[2].y = vertex_bottom;
	vertex_buffer.vertexes[3].x = vertex_left;
	vertex_buffer.vertexes[3].y = vertex_bottom;

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer), &vertex_buffer);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	BlitCommon(source_surface, rect, destination_surface, x, y, colour_key);
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	BlitCommon(source_surface, rect, &framebuffer_surface, x, y, colour_key);
}

static void ColourFillCommon(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	if (surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	// Point our framebuffer to the destination texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture_id, 0);
	glViewport(0, 0, surface->width, surface->height);

	glUseProgram(program_colour_fill);

	glDisable(GL_BLEND);

	// Disable texture coordinate array, since this doesn't use textures
	glDisableVertexAttribArray(2);

	glUniform4f(program_colour_fill_uniform_colour, red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);

	const GLfloat vertex_left = (rect->left * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_right = (rect->right * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_top = (rect->top * (2.0f / surface->height)) - 1.0f;
	const GLfloat vertex_bottom = (rect->bottom * (2.0f / surface->height)) - 1.0f;

	vertex_buffer.vertexes[0].x = vertex_left;
	vertex_buffer.vertexes[0].y = vertex_top;
	vertex_buffer.vertexes[1].x = vertex_right;
	vertex_buffer.vertexes[1].y = vertex_top;
	vertex_buffer.vertexes[2].x = vertex_right;
	vertex_buffer.vertexes[2].y = vertex_bottom;
	vertex_buffer.vertexes[3].x = vertex_left;
	vertex_buffer.vertexes[3].y = vertex_bottom;

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer.vertexes), &vertex_buffer);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	ColourFillCommon(surface, rect, red, green, blue);
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	ColourFillCommon(&framebuffer_surface, rect, red, green, blue);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	BlitCommon(&framebuffer_surface, rect, surface, rect->left, rect->top, FALSE);
}

BOOL Backend_SupportsSubpixelGlyph(void)
{
	return FALSE;	// Per-component alpha is available as an extension, but I haven't looked into it yet
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch, unsigned short total_greys, unsigned char pixel_mode)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph == NULL)
		return NULL;

	const int destination_pitch = (width + 3) & ~3;	// Round up to the nearest 4 (OpenGL needs this)

	unsigned char *buffer = (unsigned char*)malloc(destination_pitch * height);

	if (buffer == NULL)
	{
		free(glyph);
		return NULL;
	}

	switch (pixel_mode)
	{
		// FONT_PIXEL_MODE_LCD is unsupported

		case FONT_PIXEL_MODE_GRAY:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = buffer + y * destination_pitch;

				for (unsigned int x = 0; x < width; ++x)
				{
					*destination_pointer++ = (unsigned char)(pow((double)*source_pointer++ / (total_greys - 1), 1.0 / 1.8) * 255.0);
				}
			}

			break;

		case FONT_PIXEL_MODE_MONO:
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = pixels + y * pitch;
				unsigned char *destination_pointer = buffer + y * destination_pitch;

				for (unsigned int x = 0; x < width; ++x)
				{
					*destination_pointer++ = *source_pointer++ ? 0xFF : 0;
				}
			}

			break;
	}

	glGenTextures(1, &glyph->texture_id);
	glBindTexture(GL_TEXTURE_2D, glyph->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0,  GL_RED, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glyph->width = width;
	glyph->height = height;

	free(buffer);

	return glyph;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	glDeleteTextures(1, &glyph->texture_id);
	free(glyph);
}

static void DrawGlyphCommon(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	if (glyph == NULL || surface == NULL)
		return;

	// Point our framebuffer to the destination texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture_id, 0);
	glViewport(0, 0, surface->width, surface->height);

	glUseProgram(program_glyph);

	glEnable(GL_BLEND);

	// Enable texture coordinates, since this uses textures
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, glyph->texture_id);

	const GLfloat vertex_left = (x * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_right = ((x + glyph->width) * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_top = (y * (2.0f / surface->height)) - 1.0f;
	const GLfloat vertex_bottom = ((y + glyph->height) * (2.0f / surface->height)) - 1.0f;

	glUniform4f(program_glyph_uniform_colour, colours[0] / 255.0f, colours[1] / 255.0f, colours[2] / 255.0f, 1.0f);

	vertex_buffer.texture_coordinates[0].x = 0.0f;
	vertex_buffer.texture_coordinates[0].y = 0.0f;
	vertex_buffer.texture_coordinates[1].x = 1.0f;
	vertex_buffer.texture_coordinates[1].y = 0.0f;
	vertex_buffer.texture_coordinates[2].x = 1.0f;
	vertex_buffer.texture_coordinates[2].y = 1.0f;
	vertex_buffer.texture_coordinates[3].x = 0.0f;
	vertex_buffer.texture_coordinates[3].y = 1.0f;

	vertex_buffer.vertexes[0].x = vertex_left;
	vertex_buffer.vertexes[0].y = vertex_top;
	vertex_buffer.vertexes[1].x = vertex_right;
	vertex_buffer.vertexes[1].y = vertex_top;
	vertex_buffer.vertexes[2].x = vertex_right;
	vertex_buffer.vertexes[2].y = vertex_bottom;
	vertex_buffer.vertexes[3].x = vertex_left;
	vertex_buffer.vertexes[3].y = vertex_bottom;

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer), &vertex_buffer);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Backend_DrawGlyph(Backend_Surface *surface, Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	DrawGlyphCommon(surface, glyph, x, y, colours);
}

void Backend_DrawGlyphToScreen(Backend_Glyph *glyph, long x, long y, const unsigned char *colours)
{
	DrawGlyphCommon(&framebuffer_surface, glyph, x, y, colours);
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
