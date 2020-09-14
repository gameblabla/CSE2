// Dual OpenGL 3.2 and OpenGL ES 2.0 renderer

#include "../Rendering.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_OPENGLES2
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif

#include "../Misc.h"
#include "Window/OpenGL.h"

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

typedef struct RenderBackend_Surface
{
	GLuint texture_id;
	size_t width;
	size_t height;
} RenderBackend_Surface;

typedef struct RenderBackend_GlyphAtlas
{
	GLuint texture_id;
	size_t width;
	size_t height;
} RenderBackend_GlyphAtlas;

typedef struct Coordinate2D
{
	GLfloat x;
	GLfloat y;
} Coordinate2D;

typedef struct Vertex
{
	Coordinate2D position;
	Coordinate2D texture;
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
static size_t local_vertex_buffer_size;
static size_t current_vertex_buffer_slot;

static RenderMode last_render_mode;
static GLuint last_source_texture;
static GLuint last_destination_texture;

static RenderBackend_Surface framebuffer;

static RenderBackend_Surface *glyph_destination_surface;

static size_t actual_screen_width;
static size_t actual_screen_height;

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
		Backend_PrintInfo("OpenGL debug: %s", message);
}
*/
////////////////////////
// Shader compilation //
////////////////////////

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
		char buffer[0x400];
		glGetShaderInfoLog(vertex_shader, sizeof(buffer), NULL, buffer);
		Backend_PrintError("Vertex shader error: %s", buffer);
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
		char buffer[0x400];
		glGetShaderInfoLog(fragment_shader, sizeof(buffer), NULL, buffer);
		Backend_PrintError("Fragment shader error: %s", buffer);
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
		char buffer[0x400];
		glGetProgramInfoLog(program_id, sizeof(buffer), NULL, buffer);
		Backend_PrintError("Shader linker error: %s", buffer);
		return 0;
	}

	return program_id;
}

//////////////////////////////
// Vertex buffer management //
//////////////////////////////

static VertexBufferSlot* GetVertexBufferSlot(unsigned int slots_needed)
{
	// Check if buffer needs expanding
	if (current_vertex_buffer_slot + slots_needed > local_vertex_buffer_size)
	{
		local_vertex_buffer_size = 1;

		while (current_vertex_buffer_slot + slots_needed > local_vertex_buffer_size)
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

	current_vertex_buffer_slot += slots_needed;

	return &local_vertex_buffer[current_vertex_buffer_slot - slots_needed];
}

static void FlushVertexBuffer(void)
{
	static size_t vertex_buffer_size[TOTAL_VBOS];
	static size_t current_vertex_buffer = 0;

	if (current_vertex_buffer_slot == 0)
		return;

	// Select new VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ids[current_vertex_buffer]);
	glVertexAttribPointer(ATTRIBUTE_INPUT_VERTEX_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glVertexAttribPointer(ATTRIBUTE_INPUT_TEXTURE_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture));

	// Upload vertex buffer to VBO, growing it if necessary
	if (local_vertex_buffer_size > vertex_buffer_size[current_vertex_buffer])
	{
		vertex_buffer_size[current_vertex_buffer] = local_vertex_buffer_size;
		glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size[current_vertex_buffer] * sizeof(VertexBufferSlot), local_vertex_buffer, GL_DYNAMIC_DRAW);
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

#ifndef USE_OPENGLES2

static const char* GetOpenGLErrorCodeDescription(GLenum error_code)
{
	switch (error_code)
	{
		case GL_NO_ERROR:
			return "No error";

		case GL_INVALID_ENUM:
			return "An unacceptable value was specified for enumerated argument";

		case GL_INVALID_VALUE:
			return "A numeric argument is out of range";

		case GL_INVALID_OPERATION:
			return "The specified operation is not allowed in the current state";

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "The framebuffer object is not complete";

		case GL_OUT_OF_MEMORY:
			return "There is not enough memory left to execute the command";

		/*
		 * For some reason glad does not define these even though they are there in OpenGL 3.2
		 */

/*
		case GL_STACK_UNDERFLOW:
			return "An attempt has been made to perform an operation that would cause an internal stack to underflow";

		case GL_STACK_OVERFLOW:
			return "An attempt has been made to perform an operation that would cause an internal stack to overflow";
*/

		default:
			return "Unknown error";
	}
}

static void PostGLCallCallback(const char *name, void *function_pointer, int length_arguments, ...)
{
	(void)function_pointer;
	(void)length_arguments;

	GLenum error_code = glad_glGetError();	// Manually use glad_glGetError. Otherwise, glad_debug_glGetError would be called and we'd get infinite recursion into this function

	if (error_code != GL_NO_ERROR)
		Backend_PrintError("Error %d in %s: %s", error_code, name, GetOpenGLErrorCodeDescription(error_code));
}

#endif

///////////////////////////////////
// Render-backend initialisation //
///////////////////////////////////

RenderBackend_Surface* RenderBackend_Init(const char *window_title, size_t screen_width, size_t screen_height, bool fullscreen)
{
#ifndef USE_OPENGLES2
	glad_set_post_callback(PostGLCallCallback);
#endif

	actual_screen_width = screen_width;
	actual_screen_height = screen_height;

	if (WindowBackend_OpenGL_CreateWindow(window_title, &actual_screen_width, &actual_screen_height, fullscreen))
	{
		Backend_PrintInfo("GL_VENDOR = %s", glGetString(GL_VENDOR));
		Backend_PrintInfo("GL_RENDERER = %s", glGetString(GL_RENDERER));
		Backend_PrintInfo("GL_VERSION = %s", glGetString(GL_VERSION));
		Backend_PrintInfo("GL_SHADING_LANGUAGE_VERSION = %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

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

void RenderBackend_Deinit(void)
{
	free(local_vertex_buffer);

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

void RenderBackend_DrawScreen(void)
{
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

	if (actual_screen_width * framebuffer.height > framebuffer.width * actual_screen_height)	// Fancy way to do `if (actual_screen_width / actual_screen_height > framebuffer.width / framebuffer.height)` without floats
	{
		y = 0;
		height = actual_screen_height;

		width = (framebuffer.width * actual_screen_height) / framebuffer.height;
		x = (actual_screen_width - width) / 2;
	}
	else
	{
		x = 0;
		width = actual_screen_width;

		height = (framebuffer.height * actual_screen_width) / framebuffer.width;
		y = (actual_screen_height - height) / 2;
	}

	glViewport(x, y, width, height);

	// Draw framebuffer to screen
	glBindTexture(GL_TEXTURE_2D, framebuffer.texture_id);

	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

	if (vertex_buffer_slot != NULL)
	{
		vertex_buffer_slot->vertices[0][0].position.x = -1.0f;
		vertex_buffer_slot->vertices[0][0].position.y = -1.0f;
		vertex_buffer_slot->vertices[0][1].position.x = 1.0f;
		vertex_buffer_slot->vertices[0][1].position.y = -1.0f;
		vertex_buffer_slot->vertices[0][2].position.x = 1.0f;
		vertex_buffer_slot->vertices[0][2].position.y = 1.0f;

		vertex_buffer_slot->vertices[1][0].position.x = -1.0f;
		vertex_buffer_slot->vertices[1][0].position.y = -1.0f;
		vertex_buffer_slot->vertices[1][1].position.x = 1.0f;
		vertex_buffer_slot->vertices[1][1].position.y = 1.0f;
		vertex_buffer_slot->vertices[1][2].position.x = -1.0f;
		vertex_buffer_slot->vertices[1][2].position.y = 1.0f;

		vertex_buffer_slot->vertices[0][0].texture.x = 0.0f;
		vertex_buffer_slot->vertices[0][0].texture.y = 1.0f;
		vertex_buffer_slot->vertices[0][1].texture.x = 1.0f;
		vertex_buffer_slot->vertices[0][1].texture.y = 1.0f;
		vertex_buffer_slot->vertices[0][2].texture.x = 1.0f;
		vertex_buffer_slot->vertices[0][2].texture.y = 0.0f;

		vertex_buffer_slot->vertices[1][0].texture.x = 0.0f;
		vertex_buffer_slot->vertices[1][0].texture.y = 1.0f;
		vertex_buffer_slot->vertices[1][1].texture.x = 1.0f;
		vertex_buffer_slot->vertices[1][1].texture.y = 0.0f;
		vertex_buffer_slot->vertices[1][2].texture.x = 0.0f;
		vertex_buffer_slot->vertices[1][2].texture.y = 0.0f;

	}

	FlushVertexBuffer();

	WindowBackend_OpenGL_Display();

	// According to https://www.khronos.org/opengl/wiki/Common_Mistakes#Swap_Buffers
	// the buffer should always be cleared, even if it seems unnecessary
	glClear(GL_COLOR_BUFFER_BIT);

	// Switch back to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

////////////////////////
// Surface management //
////////////////////////

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	(void)render_target;

	RenderBackend_Surface *surface = (RenderBackend_Surface*)malloc(sizeof(RenderBackend_Surface));

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

void RenderBackend_FreeSurface(RenderBackend_Surface *surface)
{
	// Flush the vertex buffer if we're about to destroy its texture
	if (surface->texture_id == last_source_texture)
	{
		FlushVertexBuffer();
		last_source_texture = 0;
	}

	if (surface->texture_id == last_destination_texture)
	{
		FlushVertexBuffer();
		last_destination_texture = 0;
	}

	glDeleteTextures(1, &surface->texture_id);
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
	if (surface->texture_id == last_source_texture || surface->texture_id == last_destination_texture)
		FlushVertexBuffer();

	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_source_texture);
}

/////////////
// Drawing //
/////////////

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
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
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = x * 2.0f / destination_surface->width - 1.0f;
		const GLfloat vertex_top = y * 2.0f / destination_surface->height - 1.0f;
		const GLfloat vertex_right = (x + (rect->right - rect->left)) * 2.0f / destination_surface->width - 1.0f;
		const GLfloat vertex_bottom = (y + (rect->bottom - rect->top)) * 2.0f / destination_surface->height - 1.0f;

		vertex_buffer_slot->vertices[0][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][1].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][2].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][2].position.y = vertex_bottom;

		vertex_buffer_slot->vertices[1][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1][1].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[1][2].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][2].position.y = vertex_bottom;

		const GLfloat texture_left = rect->left / (GLfloat)source_surface->width;
		const GLfloat texture_top = rect->top / (GLfloat)source_surface->height;
		const GLfloat texture_right = rect->right / (GLfloat)source_surface->width;
		const GLfloat texture_bottom = rect->bottom / (GLfloat)source_surface->height;

		vertex_buffer_slot->vertices[0][0].texture.x = texture_left;
		vertex_buffer_slot->vertices[0][0].texture.y = texture_top;
		vertex_buffer_slot->vertices[0][1].texture.x = texture_right;
		vertex_buffer_slot->vertices[0][1].texture.y = texture_top;
		vertex_buffer_slot->vertices[0][2].texture.x = texture_right;
		vertex_buffer_slot->vertices[0][2].texture.y = texture_bottom;

		vertex_buffer_slot->vertices[1][0].texture.x = texture_left;
		vertex_buffer_slot->vertices[1][0].texture.y = texture_top;
		vertex_buffer_slot->vertices[1][1].texture.x = texture_right;
		vertex_buffer_slot->vertices[1][1].texture.y = texture_bottom;
		vertex_buffer_slot->vertices[1][2].texture.x = texture_left;
		vertex_buffer_slot->vertices[1][2].texture.y = texture_bottom;
	}
}

void RenderBackend_ColourFill(RenderBackend_Surface *surface, const RenderBackend_Rect *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

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
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = rect->left * 2.0f / surface->width - 1.0f;
		const GLfloat vertex_top = rect->top * 2.0f / surface->height - 1.0f;
		const GLfloat vertex_right = rect->right * 2.0f / surface->width - 1.0f;
		const GLfloat vertex_bottom = rect->bottom * 2.0f / surface->height - 1.0f;

		vertex_buffer_slot->vertices[0][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][1].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][2].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][2].position.y = vertex_bottom;

		vertex_buffer_slot->vertices[1][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1][1].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[1][2].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][2].position.y = vertex_bottom;
	}
}

//////////////////////
// Glyph management //
//////////////////////

RenderBackend_GlyphAtlas* RenderBackend_CreateGlyphAtlas(size_t width, size_t height)
{
	RenderBackend_GlyphAtlas *atlas = (RenderBackend_GlyphAtlas*)malloc(sizeof(RenderBackend_GlyphAtlas));

	if (atlas != NULL)
	{
		atlas->width = width;
		atlas->height = height;

		glGenTextures(1, &atlas->texture_id);
		glBindTexture(GL_TEXTURE_2D, atlas->texture_id);

	#ifdef USE_OPENGLES2
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
	#else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	#endif

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#ifndef USE_OPENGLES2
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	#endif

		glBindTexture(GL_TEXTURE_2D, last_source_texture);
	}

	return atlas;
}

void RenderBackend_DestroyGlyphAtlas(RenderBackend_GlyphAtlas *atlas)
{
	glDeleteTextures(1, &atlas->texture_id);
	free(atlas);
}

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, atlas->texture_id);
#ifdef USE_OPENGLES2
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, pixels);
#endif
	glBindTexture(GL_TEXTURE_2D, last_source_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	(void)atlas;

	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	glyph_destination_surface = destination_surface;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_DRAW_GLYPH || last_destination_texture != glyph_destination_surface->texture_id || last_source_texture != atlas->texture_id || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_destination_texture = glyph_destination_surface->texture_id;
		last_source_texture = atlas->texture_id;
		last_red = red;
		last_green = green;
		last_blue = blue;

		glUseProgram(program_glyph);
		glUniform4f(program_glyph_uniform_colour, red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glyph_destination_surface->texture_id, 0);
		glViewport(0, 0, glyph_destination_surface->width, glyph_destination_surface->height);

		glEnable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glBindTexture(GL_TEXTURE_2D, atlas->texture_id);
	}
}

void RenderBackend_DrawGlyph(RenderBackend_GlyphAtlas *atlas, long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	// Add data to the vertex queue
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot(1);

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = x * 2.0f / glyph_destination_surface->width - 1.0f;
		const GLfloat vertex_top = y * 2.0f / glyph_destination_surface->height - 1.0f;
		const GLfloat vertex_right = (x + glyph_width) * 2.0f / glyph_destination_surface->width - 1.0f;
		const GLfloat vertex_bottom = (y + glyph_height) * 2.0f / glyph_destination_surface->height - 1.0f;

		vertex_buffer_slot->vertices[0][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[0][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][1].position.y = vertex_top;
		vertex_buffer_slot->vertices[0][2].position.x = vertex_right;
		vertex_buffer_slot->vertices[0][2].position.y = vertex_bottom;

		vertex_buffer_slot->vertices[1][0].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][0].position.y = vertex_top;
		vertex_buffer_slot->vertices[1][1].position.x = vertex_right;
		vertex_buffer_slot->vertices[1][1].position.y = vertex_bottom;
		vertex_buffer_slot->vertices[1][2].position.x = vertex_left;
		vertex_buffer_slot->vertices[1][2].position.y = vertex_bottom;

		const GLfloat texture_left = glyph_x / (GLfloat)atlas->width;
		const GLfloat texture_top = glyph_y / (GLfloat)atlas->height;
		const GLfloat texture_right = (glyph_x + glyph_width) / (GLfloat)atlas->width;
		const GLfloat texture_bottom = (glyph_y + glyph_height) / (GLfloat)atlas->height;

		vertex_buffer_slot->vertices[0][0].texture.x = texture_left;
		vertex_buffer_slot->vertices[0][0].texture.y = texture_top;
		vertex_buffer_slot->vertices[0][1].texture.x = texture_right;
		vertex_buffer_slot->vertices[0][1].texture.y = texture_top;
		vertex_buffer_slot->vertices[0][2].texture.x = texture_right;
		vertex_buffer_slot->vertices[0][2].texture.y = texture_bottom;

		vertex_buffer_slot->vertices[1][0].texture.x = texture_left;
		vertex_buffer_slot->vertices[1][0].texture.y = texture_top;
		vertex_buffer_slot->vertices[1][1].texture.x = texture_right;
		vertex_buffer_slot->vertices[1][1].texture.y = texture_bottom;
		vertex_buffer_slot->vertices[1][2].texture.x = texture_left;
		vertex_buffer_slot->vertices[1][2].texture.y = texture_bottom;
	}
}

///////////
// Misc. //
///////////

void RenderBackend_HandleRenderTargetLoss(void)
{
	// No problem for us
}

void RenderBackend_HandleWindowResize(size_t width, size_t height)
{
	actual_screen_width = width;
	actual_screen_height = height;
}
