// Dual OpenGL 3.2 and OpenGL ES 2.0 renderer

#include "../Rendering.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_OPENGLES2
#include <GLES2/gl2.h>
#else
#include "../../../external/glad/include/glad/glad.h"
#endif

#define SPRITEBATCH_IMPLEMENTATION
#include "../../../external/cute_spritebatch.h"

#include "../../WindowsWrapper.h"

#include "../Platform.h"
#include "../Window.h"
#include "../../Resource.h"

#define TOTAL_VBOS 8

#define ATTRIBUTE_INPUT_VERTEX_COORDINATES 1
#define ATTRIBUTE_INPUT_TEXTURE_COORDINATES 2

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
	unsigned char *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;
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

static GLuint program_texture;
static GLuint program_texture_colour_key;
static GLuint program_colour_fill;
static GLuint program_glyph;

static GLint program_colour_fill_uniform_colour;
static GLint program_glyph_uniform_colour;

#ifndef USE_OPENGLES2
static GLuint vertex_array_id;
#endif
static GLuint vertex_buffer_ids[TOTAL_VBOS];
static GLuint framebuffer_id;

static VertexBufferSlot *local_vertex_buffer;
static unsigned long local_vertex_buffer_size;
static unsigned long current_vertex_buffer_slot;

static RenderMode last_render_mode;
static GLuint last_source_texture;
static GLuint last_destination_texture;

static Backend_Surface framebuffer;

static unsigned char glyph_colour_channels[3];
static Backend_Surface *glyph_destination_surface;

static spritebatch_t glyph_batcher;

static int actual_screen_width;
static int actual_screen_height;

#ifdef USE_OPENGLES2
static const GLchar *vertex_shader_plain = " \
#version 100\n \
attribute vec2 input_vertex_coordinates; \
void main() \
{ \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0); \
} \
";

static const GLchar *vertex_shader_texture = " \
#version 100\n \
attribute vec2 input_vertex_coordinates; \
attribute vec2 input_texture_coordinates; \
varying vec2 texture_coordinates; \
void main() \
{ \
	texture_coordinates = input_texture_coordinates; \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0); \
} \
";

static const GLchar *fragment_shader_texture = " \
#version 100\n \
precision mediump float; \
uniform sampler2D tex; \
varying vec2 texture_coordinates; \
void main() \
{ \
	gl_FragColor = texture2D(tex, texture_coordinates); \
} \
";

static const GLchar *fragment_shader_texture_colour_key = " \
#version 100\n \
precision mediump float; \
uniform sampler2D tex; \
varying vec2 texture_coordinates; \
void main() \
{ \
	vec4 colour = texture2D(tex, texture_coordinates); \
\
	if (colour.xyz == vec3(0.0f, 0.0f, 0.0f)) \
		discard; \
\
	gl_FragColor = colour; \
} \
";

static const GLchar *fragment_shader_colour_fill = " \
#version 100\n \
precision mediump float; \
uniform vec4 colour; \
void main() \
{ \
	gl_FragColor = colour; \
} \
";

static const GLchar *fragment_shader_glyph = " \
#version 100\n \
precision mediump float; \
uniform sampler2D tex; \
uniform vec4 colour; \
varying vec2 texture_coordinates; \
void main() \
{ \
	gl_FragColor = colour * vec4(1.0, 1.0, 1.0, texture2D(tex, texture_coordinates).r); \
} \
";
#else
static const GLchar *vertex_shader_plain = " \
#version 150 core\n \
in vec2 input_vertex_coordinates; \
void main() \
{ \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0); \
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
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0); \
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

static const GLchar *fragment_shader_texture_colour_key = " \
#version 150 core\n \
uniform sampler2D tex; \
in vec2 texture_coordinates; \
out vec4 fragment; \
void main() \
{ \
	vec4 colour = texture(tex, texture_coordinates); \
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
	fragment = colour * vec4(1.0, 1.0, 1.0, texture(tex, texture_coordinates).r); \
} \
";
#endif
/*
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
*/
// ====================
// Shader compilation
// ====================

static GLuint CompileShader(const char *vertex_shader_source, const char *fragment_shader_source)
{
	GLint shader_status;

	GLuint program_id = glCreateProgram();

	// Compile vertex shader
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetShaderInfoLog(vertex_shader, sizeof(buffer), NULL, buffer);
		PlatformBackend_ShowMessageBox("Vertex shader error", buffer);
		return 0;
	}

	glAttachShader(program_id, vertex_shader);

	// Compile fragment shader
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetShaderInfoLog(fragment_shader, sizeof(buffer), NULL, buffer);
		PlatformBackend_ShowMessageBox("Fragment shader error", buffer);
		return 0;
	}

	glAttachShader(program_id, fragment_shader);

	// Link shaders
	glBindAttribLocation(program_id, ATTRIBUTE_INPUT_VERTEX_COORDINATES, "input_vertex_coordinates");
	glBindAttribLocation(program_id, ATTRIBUTE_INPUT_TEXTURE_COORDINATES, "input_texture_coordinates");

	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
	{
		char buffer[0x200];
		glGetProgramInfoLog(program_id, sizeof(buffer), NULL, buffer);
		PlatformBackend_ShowMessageBox("Shader linker error", buffer);
		return 0;
	}

	return program_id;
}

// ====================
// Vertex buffer management
// ====================

static VertexBufferSlot* GetVertexBufferSlot(unsigned int slots_needed)
{
	// Check if buffer needs expanding
	if (current_vertex_buffer_slot + slots_needed > local_vertex_buffer_size)
	{
		local_vertex_buffer_size = 1;

		while (current_vertex_buffer_slot + slots_needed > local_vertex_buffer_size)
			local_vertex_buffer_size <<= 1;

		local_vertex_buffer = (VertexBufferSlot*)realloc(local_vertex_buffer, local_vertex_buffer_size * sizeof(VertexBufferSlot));
	}

	current_vertex_buffer_slot += slots_needed;

	return &local_vertex_buffer[current_vertex_buffer_slot - slots_needed];
}

static void FlushVertexBuffer(void)
{
	static unsigned long vertex_buffer_size[TOTAL_VBOS];
	static unsigned int current_vertex_buffer = 0;

	if (current_vertex_buffer_slot == 0)
		return;

	// Select new VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[current_vertex_buffer]);
	glVertexAttribPointer(ATTRIBUTE_INPUT_VERTEX_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, vertex_coordinate));
	glVertexAttribPointer(ATTRIBUTE_INPUT_TEXTURE_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture_coordinate));

	// Upload vertex buffer to VBO, growing it if necessary
	if (local_vertex_buffer_size > vertex_buffer_size[current_vertex_buffer])
	{
		vertex_buffer_size[current_vertex_buffer] = local_vertex_buffer_size;
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size[current_vertex_buffer] * sizeof(VertexBufferSlot), local_vertex_buffer, GL_STREAM_DRAW);
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, 0, current_vertex_buffer_slot * sizeof(VertexBufferSlot), local_vertex_buffer);
	}

	if (++current_vertex_buffer >= TOTAL_VBOS)
		current_vertex_buffer = 0;

	glDrawArrays(GL_TRIANGLES, 0, 6 * current_vertex_buffer_slot);

	current_vertex_buffer_slot = 0;
}

// ====================
// Glyph-batching
// ====================

// Blit the glyphs in the batch
static void GlyphBatch_Draw(spritebatch_sprite_t *sprites, int count, int texture_w, int texture_h, void *udata)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	(void)udata;

	if (glyph_destination_surface == NULL)
		return;

	GLuint texture_id = (GLuint)sprites[0].texture_id;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_DRAW_GLYPH || last_destination_texture != glyph_destination_surface->texture_id || last_source_texture != texture_id || last_red != glyph_colour_channels[0] || last_green != glyph_colour_channels[1] || last_blue != glyph_colour_channels[2])
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_destination_texture = glyph_destination_surface->texture_id;
		last_source_texture = texture_id;
		last_red = glyph_colour_channels[0];
		last_green = glyph_colour_channels[1];
		last_blue = glyph_colour_channels[2];

		glUseProgram(program_glyph);
		glUniform4f(program_glyph_uniform_colour, glyph_colour_channels[0] / 255.0f, glyph_colour_channels[1] / 255.0f, glyph_colour_channels[2] / 255.0f, 1.0f);

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glyph_destination_surface->texture_id, 0);
		glViewport(0, 0, glyph_destination_surface->width, glyph_destination_surface->height);

		glEnable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	// Add data to the vertex queue
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(count);

	for (int i = 0; i < count; ++i)
	{
		Backend_Glyph *glyph = (Backend_Glyph*)sprites[i].image_id;

		const GLfloat texture_left = sprites[i].minx;
		const GLfloat texture_right = texture_left + ((GLfloat)glyph->width / (GLfloat)texture_w);	// Account for width not matching pitch
		const GLfloat texture_top = sprites[i].maxy;
		const GLfloat texture_bottom = sprites[i].miny;

		const GLfloat vertex_left = (sprites[i].x * (2.0f / glyph_destination_surface->width)) - 1.0f;
		const GLfloat vertex_right = ((sprites[i].x + glyph->width) * (2.0f / glyph_destination_surface->width)) - 1.0f;
		const GLfloat vertex_top = (sprites[i].y * (2.0f / glyph_destination_surface->height)) - 1.0f;
		const GLfloat vertex_bottom = ((sprites[i].y + glyph->height) * (2.0f / glyph_destination_surface->height)) - 1.0f;

		vertex_buffer_slot[i].vertices[0][0].texture_coordinate.x = texture_left;
		vertex_buffer_slot[i].vertices[0][0].texture_coordinate.y = texture_top;
		vertex_buffer_slot[i].vertices[0][1].texture_coordinate.x = texture_right;
		vertex_buffer_slot[i].vertices[0][1].texture_coordinate.y = texture_top;
		vertex_buffer_slot[i].vertices[0][2].texture_coordinate.x = texture_right;
		vertex_buffer_slot[i].vertices[0][2].texture_coordinate.y = texture_bottom;

		vertex_buffer_slot[i].vertices[1][0].texture_coordinate.x = texture_left;
		vertex_buffer_slot[i].vertices[1][0].texture_coordinate.y = texture_top;
		vertex_buffer_slot[i].vertices[1][1].texture_coordinate.x = texture_right;
		vertex_buffer_slot[i].vertices[1][1].texture_coordinate.y = texture_bottom;
		vertex_buffer_slot[i].vertices[1][2].texture_coordinate.x = texture_left;
		vertex_buffer_slot[i].vertices[1][2].texture_coordinate.y = texture_bottom;

		vertex_buffer_slot[i].vertices[0][0].vertex_coordinate.x = vertex_left;
		vertex_buffer_slot[i].vertices[0][0].vertex_coordinate.y = vertex_top;
		vertex_buffer_slot[i].vertices[0][1].vertex_coordinate.x = vertex_right;
		vertex_buffer_slot[i].vertices[0][1].vertex_coordinate.y = vertex_top;
		vertex_buffer_slot[i].vertices[0][2].vertex_coordinate.x = vertex_right;
		vertex_buffer_slot[i].vertices[0][2].vertex_coordinate.y = vertex_bottom;

		vertex_buffer_slot[i].vertices[1][0].vertex_coordinate.x = vertex_left;
		vertex_buffer_slot[i].vertices[1][0].vertex_coordinate.y = vertex_top;
		vertex_buffer_slot[i].vertices[1][1].vertex_coordinate.x = vertex_right;
		vertex_buffer_slot[i].vertices[1][1].vertex_coordinate.y = vertex_bottom;
		vertex_buffer_slot[i].vertices[1][2].vertex_coordinate.x = vertex_left;
		vertex_buffer_slot[i].vertices[1][2].vertex_coordinate.y = vertex_bottom;
	}
}

// Upload the glyph's pixels
static void GlyphBatch_GetPixels(SPRITEBATCH_U64 image_id, void *buffer, int bytes_to_fill, void *udata)
{
	(void)udata;

	Backend_Glyph *glyph = (Backend_Glyph*)image_id;

	memcpy(buffer, glyph->pixels, bytes_to_fill);
}

// Create a texture atlas, and upload pixels to it
static SPRITEBATCH_U64 GlyphBatch_CreateTexture(void *pixels, int w, int h, void *udata)
{
	(void)udata;

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
#ifdef USE_OPENGLES2
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef USE_OPENGLES2
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif

	glBindTexture(GL_TEXTURE_2D, last_source_texture);

	return (SPRITEBATCH_U64)texture_id;
}

// Destroy texture atlas
static void GlyphBatch_DestroyTexture(SPRITEBATCH_U64 texture_id, void *udata)
{
	(void)udata;

	GLuint gl_texture_id = (GLuint)texture_id;

	// Flush the vertex buffer if we're about to destroy its texture
	if (gl_texture_id == last_source_texture || gl_texture_id == last_destination_texture)
		FlushVertexBuffer();

	glDeleteTextures(1, &gl_texture_id);
}

// ====================
// Render-backend initialisation
// ====================

Backend_Surface* Backend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen)
{
	actual_screen_width = screen_width;
	actual_screen_height = screen_height;

	if (WindowBackend_OpenGL_CreateWindow(window_title, &actual_screen_width, &actual_screen_height, fullscreen))
	{
		printf("GL_VENDOR = %s\n", glGetString(GL_VENDOR));
		printf("GL_RENDERER = %s\n", glGetString(GL_RENDERER));
		printf("GL_VERSION = %s\n", glGetString(GL_VERSION));

		// Set up blending (only used for font-rendering)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback(MessageCallback, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

	#ifndef USE_OPENGLES2
		// Set up Vertex Array Object
		glGenVertexArrays(1, &vertex_array_id);
		glBindVertexArray(vertex_array_id);
	#endif

		// Set up Vertex Buffer Objects
		glGenBuffers(TOTAL_VBOS, vertex_buffer_ids);

		// Set up the vertex attributes
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_VERTEX_COORDINATES);

		// Set up our shaders
		program_texture = CompileShader(vertex_shader_texture, fragment_shader_texture);
		program_texture_colour_key = CompileShader(vertex_shader_texture, fragment_shader_texture_colour_key);
		program_colour_fill = CompileShader(vertex_shader_plain, fragment_shader_colour_fill);
		program_glyph = CompileShader(vertex_shader_texture, fragment_shader_glyph);

		if (program_texture != 0 && program_texture_colour_key != 0 && program_colour_fill != 0 && program_glyph != 0)
		{
			// Get shader uniforms
			program_colour_fill_uniform_colour = glGetUniformLocation(program_colour_fill, "colour");
			program_glyph_uniform_colour = glGetUniformLocation(program_glyph, "colour");

			// Set up framebuffer (used for surface-to-surface blitting)
			glGenFramebuffers(1, &framebuffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

			// Set up framebuffer screen texture (used for screen-to-surface blitting)
			glGenTextures(1, &framebuffer.texture_id);
			glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);
		#ifdef USE_OPENGLES2
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		#endif
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		#ifndef USE_OPENGLES2
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		#endif

			framebuffer.width = screen_width;
			framebuffer.height = screen_height;

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.texture_id, 0);
			glViewport(0, 0, framebuffer.width, framebuffer.height);

			// Set-up glyph-batcher
			spritebatch_config_t config;
			spritebatch_set_default_config(&config);
			config.pixel_stride = 1;
			config.atlas_width_in_pixels = 256;
			config.atlas_height_in_pixels = 256;
			config.lonely_buffer_count_till_flush = 4; // Start making atlases immediately
			config.batch_callback = GlyphBatch_Draw;
			config.get_pixels_callback = GlyphBatch_GetPixels;
			config.generate_texture_callback = GlyphBatch_CreateTexture;
			config.delete_texture_callback = GlyphBatch_DestroyTexture;
			spritebatch_init(&glyph_batcher, &config, NULL);

			return &framebuffer;
		}

		if (program_glyph != 0)
			glDeleteProgram(program_glyph);

		if (program_colour_fill != 0)
			glDeleteProgram(program_colour_fill);

		if (program_texture_colour_key != 0)
			glDeleteProgram(program_texture_colour_key);

		if (program_texture != 0)
			glDeleteProgram(program_texture);

		glDeleteBuffers(TOTAL_VBOS, vertex_buffer_ids);
	#ifndef USE_OPENGLES2
		glDeleteVertexArrays(1, &vertex_array_id);
	#endif
	}

	return NULL;
}

void Backend_Deinit(void)
{
	free(local_vertex_buffer);

	spritebatch_term(&glyph_batcher);

	glDeleteTextures(1, &framebuffer.texture_id);
	glDeleteFramebuffers(1, &framebuffer_id);
	glDeleteProgram(program_glyph);
	glDeleteProgram(program_colour_fill);
	glDeleteProgram(program_texture_colour_key);
	glDeleteProgram(program_texture);
	glDeleteBuffers(TOTAL_VBOS, vertex_buffer_ids);
#ifndef USE_OPENGLES2
	glDeleteVertexArrays(1, &vertex_array_id);
#endif

	WindowBackend_OpenGL_DestroyWindow();
}

void Backend_DrawScreen(void)
{
	spritebatch_tick(&glyph_batcher);

	FlushVertexBuffer();
	last_render_mode = MODE_BLANK;
	last_source_texture = 0;
	last_destination_texture = 0;

	glUseProgram(program_texture);

	glDisable(GL_BLEND);

	// Enable texture coordinates, since this uses textures
	glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

	// Target actual screen, and not our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Do some viewport trickery, to fit the framebuffer in the center of the screen
	GLint x;
	GLint y;
	GLsizei width;
	GLsizei height;

	if (actual_screen_width > actual_screen_height)
	{
		y = 0;
		height = actual_screen_height;

		width = framebuffer.width * ((float)actual_screen_height / (float)framebuffer.height);
		x = (actual_screen_width - width) / 2;
	}
	else
	{
		x = 0;
		width = actual_screen_width;

		height = framebuffer.height * ((float)actual_screen_width / (float)framebuffer.width);
		y = (actual_screen_height - height) / 2;
	}

	glViewport(x, y, width, height);

	// Draw framebuffer to screen
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

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

	vertex_buffer_slot->vertices[0][0].vertex_coordinate.x = -1.0f;
	vertex_buffer_slot->vertices[0][0].vertex_coordinate.y = -1.0f;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][1].vertex_coordinate.y = -1.0f;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[0][2].vertex_coordinate.y = 1.0f;

	vertex_buffer_slot->vertices[1][0].vertex_coordinate.x = -1.0f;
	vertex_buffer_slot->vertices[1][0].vertex_coordinate.y = -1.0f;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.x = 1.0f;
	vertex_buffer_slot->vertices[1][1].vertex_coordinate.y = 1.0f;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.x = -1.0f;
	vertex_buffer_slot->vertices[1][2].vertex_coordinate.y = 1.0f;

	FlushVertexBuffer();

	WindowBackend_OpenGL_Display();

	// According to https://www.khronos.org/opengl/wiki/Common_Mistakes#Swap_Buffers
	// the buffer should always be cleared, even if it seems unnecessary
	glClear(GL_COLOR_BUFFER_BIT);

	// Switch back to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

// ====================
// Surface management
// ====================

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	Backend_Surface *surface = (Backend_Surface*)malloc(sizeof(Backend_Surface));

	if (surface == NULL)
		return NULL;

	glGenTextures(1, &surface->texture_id);
	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
#ifdef USE_OPENGLES2
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef USE_OPENGLES2
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif

	glBindTexture(GL_TEXTURE_2D, last_source_texture);

	surface->width = width;
	surface->height = height;

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	if (surface == NULL)
		return;

	// Flush the vertex buffer if we're about to destroy its texture
	if (surface->texture_id == last_source_texture || surface->texture_id == last_destination_texture)
		FlushVertexBuffer();

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

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return NULL;

	surface->pixels = (unsigned char*)malloc(width * height * 3);
	*pitch = width * 3;
	return surface->pixels;
}

void Backend_UnlockSurface(Backend_Surface *surface, unsigned int width, unsigned int height)
{
	if (surface == NULL)
		return;

	// Flush the vertex buffer if we're about to modify its texture
	if (surface->texture_id == last_source_texture || surface->texture_id == last_destination_texture)
		FlushVertexBuffer();

	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	free(surface->pixels);

	glBindTexture(GL_TEXTURE_2D, last_source_texture);
}

// ====================
// Drawing
// ====================

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	if (source_surface == NULL || destination_surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	const RenderMode render_mode = (colour_key ? MODE_DRAW_SURFACE_WITH_TRANSPARENCY : MODE_DRAW_SURFACE);

	// Flush vertex data if a context-change is needed
	if (last_render_mode != render_mode || last_source_texture != source_surface->texture_id || last_destination_texture != destination_surface->texture_id)
	{
		FlushVertexBuffer();

		last_render_mode = render_mode;
		last_source_texture = source_surface->texture_id;
		last_destination_texture = destination_surface->texture_id;

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destination_surface->texture_id, 0);
		glViewport(0, 0, destination_surface->width, destination_surface->height);

		// Switch to colour-key shader if we have to
		glUseProgram(colour_key ? program_texture_colour_key : program_texture);

		glDisable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glBindTexture(GL_TEXTURE_2D, source_surface->texture_id);
	}

	// Add data to the vertex queue
	const GLfloat texture_left = (GLfloat)rect->left / (GLfloat)source_surface->width;
	const GLfloat texture_right = (GLfloat)rect->right / (GLfloat)source_surface->width;
	const GLfloat texture_top = (GLfloat)rect->top / (GLfloat)source_surface->height;
	const GLfloat texture_bottom = (GLfloat)rect->bottom / (GLfloat)source_surface->height;

	const GLfloat vertex_left = (x * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_right = ((x + (rect->right - rect->left)) * (2.0f / destination_surface->width)) - 1.0f;
	const GLfloat vertex_top = (y * (2.0f / destination_surface->height)) - 1.0f;
	const GLfloat vertex_bottom = ((y + (rect->bottom - rect->top)) * (2.0f / destination_surface->height)) - 1.0f;

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

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

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	if (surface == NULL)
		return;

	if (rect->right - rect->left < 0 || rect->bottom - rect->top < 0)
		return;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_COLOUR_FILL || last_destination_texture != surface->texture_id || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_COLOUR_FILL;
		last_source_texture = 0;
		last_destination_texture = surface->texture_id;
		last_red = red;
		last_green = green;
		last_blue = blue;

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, surface->texture_id, 0);
		glViewport(0, 0, surface->width, surface->height);

		glUseProgram(program_colour_fill);

		glDisable(GL_BLEND);

		// Disable texture coordinate array, since this doesn't use textures
		glDisableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glUniform4f(program_colour_fill_uniform_colour, red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
	}

	// Add data to the vertex queue
	const GLfloat vertex_left = (rect->left * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_right = (rect->right * (2.0f / surface->width)) - 1.0f;
	const GLfloat vertex_top = (rect->top * (2.0f / surface->height)) - 1.0f;
	const GLfloat vertex_bottom = (rect->bottom * (2.0f / surface->height)) - 1.0f;

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

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

// ====================
// Glyph management
// ====================

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	Backend_Glyph *glyph = (Backend_Glyph*)malloc(sizeof(Backend_Glyph));

	if (glyph != NULL)
	{
		glyph->pitch = (width + 3) & ~3;	// Round up to the nearest 4 (OpenGL needs this)

		glyph->pixels = (unsigned char*)malloc(glyph->pitch * height);

		if (glyph->pixels != NULL)
		{
			for (unsigned int y = 0; y < height; ++y)
			{
				const unsigned char *source_pointer = &pixels[y * pitch];
				unsigned char *destination_pointer = &glyph->pixels[y * glyph->pitch];
				memcpy(destination_pointer, source_pointer, width);
			}

			glyph->width = width;
			glyph->height = height;

			return glyph;
		}

		free(glyph);
	}

	return NULL;
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	if (glyph == NULL)
		return;

	free(glyph->pixels);
	free(glyph);
}

void Backend_PrepareToDrawGlyphs(Backend_Surface *destination_surface, const unsigned char *colour_channels)
{
	glyph_destination_surface = destination_surface;

	memcpy(glyph_colour_channels, colour_channels, sizeof(glyph_colour_channels));
}

void Backend_DrawGlyph(Backend_Glyph *glyph, long x, long y)
{
	spritebatch_push(&glyph_batcher, (SPRITEBATCH_U64)glyph, glyph->pitch, glyph->height, x, y, 1.0f, 1.0f, 0.0f, 0.0f, 0);
}

void Backend_FlushGlyphs(void)
{
	spritebatch_defrag(&glyph_batcher);
	spritebatch_flush(&glyph_batcher);
}

// ====================
// Misc.
// ====================

void Backend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(unsigned int width, unsigned int height)
{
	actual_screen_width = width;
	actual_screen_height = height;
}
