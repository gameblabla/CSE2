// Released under the MIT licence.
// See LICENCE.txt for details.

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

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

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

static struct
{
	GLuint id;
	struct
	{
		GLint vertex_transform;
		GLint texture_coordinate_transform;
	} uniforms;
} program_texture;

static struct
{
	GLuint id;
	struct
	{
		GLint vertex_transform;
		GLint texture_coordinate_transform;
	} uniforms;
} program_texture_colour_key;

static struct
{
	GLuint id;
	struct
	{
		GLint vertex_transform;
		GLint colour;
	} uniforms;
} program_colour_fill;

static struct
{
	GLuint id;
	struct
	{
		GLint vertex_transform;
		GLint texture_coordinate_transform;
		GLint colour;
	} uniforms;
} program_glyph;


#ifndef USE_OPENGLES2
static GLuint vertex_array_id;
#endif
static GLuint vertex_buffer_id;
static GLuint framebuffer_id;

static VertexBufferSlot *local_vertex_buffer;
static size_t local_vertex_buffer_size;
static size_t current_vertex_buffer_slot;

static RenderMode last_render_mode;
static GLuint last_source_texture;
static GLuint last_destination_texture;

static RenderBackend_Surface *framebuffer_surface;
static RenderBackend_Surface *upscaled_framebuffer_surface;
static RenderBackend_Surface window_surface;

static RenderBackend_Rect window_rect;

#ifdef USE_OPENGLES2
static const GLchar *vertex_shader_plain = " \
#version 100\n \
uniform mat4 vertex_transform; \
attribute vec2 input_vertex_coordinates; \
void main() \
{ \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0) * vertex_transform; \
} \
";

static const GLchar *vertex_shader_texture = " \
#version 100\n \
uniform mat4 vertex_transform; \
uniform vec2 texture_coordinate_transform; \
attribute vec2 input_vertex_coordinates; \
attribute vec2 input_texture_coordinates; \
varying vec2 texture_coordinates; \
void main() \
{ \
	texture_coordinates = input_texture_coordinates * texture_coordinate_transform; \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0) * vertex_transform; \
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
uniform mat4 vertex_transform; \
in vec2 input_vertex_coordinates; \
void main() \
{ \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0) * vertex_transform; \
} \
";

static const GLchar *vertex_shader_texture = " \
#version 150 core\n \
uniform mat4 vertex_transform; \
uniform vec2 texture_coordinate_transform; \
in vec2 input_vertex_coordinates; \
in vec2 input_texture_coordinates; \
out vec2 texture_coordinates; \
void main() \
{ \
	texture_coordinates = input_texture_coordinates * texture_coordinate_transform; \
	gl_Position = vec4(input_vertex_coordinates.xy, 0.0, 1.0) * vertex_transform; \
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

// A little forward-declaration for some internal functions
static void Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *source_rect, RenderBackend_Surface *destination_surface, const RenderBackend_Rect *destination_rect, bool colour_key);
static RenderBackend_Surface* CreateSurface(size_t width, size_t height, bool linear_filter);

// This was used back when CSE2 used GLEW instead of glad
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

static void SetTextureUploadAlignment(size_t pitch)
{
	const GLint alignments[8] = {8, 1, 2, 1, 4, 1, 2, 1};

	glPixelStorei(GL_UNPACK_ALIGNMENT, alignments[pitch & 7]);
}

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
	if (current_vertex_buffer_slot == 0)
		return;

	glBufferData(GL_ARRAY_BUFFER, current_vertex_buffer_slot * sizeof(VertexBufferSlot), local_vertex_buffer, GL_STREAM_DRAW);

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

	size_t actual_screen_width = screen_width;
	size_t actual_screen_height = screen_height;

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

		// Set up Vertex Buffer Object
		glGenBuffers(1, &vertex_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
		glVertexAttribPointer(ATTRIBUTE_INPUT_VERTEX_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
		glVertexAttribPointer(ATTRIBUTE_INPUT_TEXTURE_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texture));

		// Set up the vertex attributes
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_VERTEX_COORDINATES);

		// Set up our shaders
		program_texture.id = CompileShader(vertex_shader_texture, fragment_shader_texture);
		program_texture_colour_key.id = CompileShader(vertex_shader_texture, fragment_shader_texture_colour_key);
		program_colour_fill.id = CompileShader(vertex_shader_plain, fragment_shader_colour_fill);
		program_glyph.id = CompileShader(vertex_shader_texture, fragment_shader_glyph);

		if (program_texture.id != 0 && program_texture_colour_key.id != 0 && program_colour_fill.id != 0 && program_glyph.id != 0)
		{
			// Get shader uniforms
			program_texture.uniforms.texture_coordinate_transform = glGetUniformLocation(program_texture.id, "texture_coordinate_transform");
			program_texture.uniforms.vertex_transform = glGetUniformLocation(program_texture.id, "vertex_transform");

			program_texture_colour_key.uniforms.texture_coordinate_transform = glGetUniformLocation(program_texture_colour_key.id, "texture_coordinate_transform");
			program_texture_colour_key.uniforms.vertex_transform = glGetUniformLocation(program_texture_colour_key.id, "vertex_transform");

			program_colour_fill.uniforms.vertex_transform = glGetUniformLocation(program_colour_fill.id, "vertex_transform");
			program_colour_fill.uniforms.colour = glGetUniformLocation(program_colour_fill.id, "colour");

			program_glyph.uniforms.texture_coordinate_transform = glGetUniformLocation(program_glyph.id, "texture_coordinate_transform");
			program_glyph.uniforms.vertex_transform = glGetUniformLocation(program_glyph.id, "vertex_transform");
			program_glyph.uniforms.colour = glGetUniformLocation(program_glyph.id, "colour");

			// Set up framebuffer (used for surface-to-surface blitting)
			glGenFramebuffers(1, &framebuffer_id);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);

			// Set up framebuffer screen texture (used for screen-to-surface blitting)
			framebuffer_surface = RenderBackend_CreateSurface(screen_width, screen_height, true);

			// Set up window surface
			window_surface.texture_id = 0;

			RenderBackend_HandleWindowResize(actual_screen_width, actual_screen_height);

			return framebuffer_surface;
		}

		if (program_glyph.id != 0)
			glDeleteProgram(program_glyph.id);

		if (program_colour_fill.id != 0)
			glDeleteProgram(program_colour_fill.id);

		if (program_texture_colour_key.id != 0)
			glDeleteProgram(program_texture_colour_key.id);

		if (program_texture.id != 0)
			glDeleteProgram(program_texture.id);

		glDeleteBuffers(1, &vertex_buffer_id);
	#ifndef USE_OPENGLES2
		glDeleteVertexArrays(1, &vertex_array_id);
	#endif
	}

	return NULL;
}

void RenderBackend_Deinit(void)
{
	free(local_vertex_buffer);

	if (upscaled_framebuffer_surface != NULL)
		RenderBackend_FreeSurface(upscaled_framebuffer_surface);
	RenderBackend_FreeSurface(framebuffer_surface);
	glDeleteFramebuffers(1, &framebuffer_id);
	glDeleteProgram(program_glyph.id);
	glDeleteProgram(program_colour_fill.id);
	glDeleteProgram(program_texture_colour_key.id);
	glDeleteProgram(program_texture.id);
	glDeleteBuffers(1, &vertex_buffer_id);
#ifndef USE_OPENGLES2
	glDeleteVertexArrays(1, &vertex_array_id);
#endif

	WindowBackend_OpenGL_DestroyWindow();
}

void RenderBackend_DrawScreen(void)
{
	RenderBackend_Rect framebuffer_rect;
	framebuffer_rect.left = 0;
	framebuffer_rect.top = 0;
	framebuffer_rect.right = framebuffer_surface->width;
	framebuffer_rect.bottom = framebuffer_surface->height;

	if (upscaled_framebuffer_surface == NULL)
	{
		Blit(framebuffer_surface, &framebuffer_rect, &window_surface, &window_rect, false);
	}
	else
	{
		RenderBackend_Rect upscaled_framebuffer_rect;
		upscaled_framebuffer_rect.left = 0;
		upscaled_framebuffer_rect.top = 0;
		upscaled_framebuffer_rect.right = upscaled_framebuffer_surface->width;
		upscaled_framebuffer_rect.bottom = upscaled_framebuffer_surface->height;

		Blit(framebuffer_surface, &framebuffer_rect, upscaled_framebuffer_surface, &upscaled_framebuffer_rect, false);
		Blit(upscaled_framebuffer_surface, &upscaled_framebuffer_rect, &window_surface, &window_rect, false);
	}

	// Target actual screen, and not our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// According to https://www.khronos.org/opengl/wiki/Common_Mistakes#Swap_Buffers
	// the buffer should always be cleared, even if it seems unnecessary
	glClear(GL_COLOR_BUFFER_BIT);

	// Flush the vertex buffer, which will render to the screen
	FlushVertexBuffer();

	WindowBackend_OpenGL_Display();

	// Switch back to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
}

////////////////////////
// Surface management //
////////////////////////

static RenderBackend_Surface* CreateSurface(size_t width, size_t height, bool linear_filter)
{
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear_filter ? GL_LINEAR : GL_NEAREST);
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

RenderBackend_Surface* RenderBackend_CreateSurface(size_t width, size_t height, bool render_target)
{
	(void)render_target;

	return CreateSurface(width, height, false);
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

	SetTextureUploadAlignment(width * 3);
	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_source_texture);
}

/////////////
// Drawing //
/////////////

static void Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *source_rect, RenderBackend_Surface *destination_surface, const RenderBackend_Rect *destination_rect, bool colour_key)
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

		GLfloat vertex_transform[4 * 4] = {
			2.0f / destination_surface->width, 0.0f, 0.0f, -1.0f,
			0.0f, 2.0f / destination_surface->height, 0.0f, -1.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		// Invert the Y-axis when drawing to the screen, since everything is upside-down in OpenGL for some reason
		if (destination_surface->texture_id == 0)
		{
			vertex_transform[4 + 1] = -vertex_transform[4 + 1];
			vertex_transform[4 + 3] = -vertex_transform[4 + 3];
		}

		// Switch to colour-key shader if we have to
		glUseProgram(colour_key ? program_texture_colour_key.id : program_texture.id);
		glUniform2f(colour_key ? program_texture_colour_key.uniforms.texture_coordinate_transform : program_texture.uniforms.texture_coordinate_transform, 1.0f / source_surface->width, 1.0f / source_surface->height);
		glUniformMatrix4fv(colour_key ? program_texture_colour_key.uniforms.vertex_transform : program_texture.uniforms.vertex_transform, 1, GL_FALSE, vertex_transform);

		glDisable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glBindTexture(GL_TEXTURE_2D, source_surface->texture_id);
	}

	// Add data to the vertex queue
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = destination_rect->left;
		const GLfloat vertex_top = destination_rect->top;
		const GLfloat vertex_right = destination_rect->right;
		const GLfloat vertex_bottom = destination_rect->bottom;

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

		const GLfloat texture_left = source_rect->left;
		const GLfloat texture_top = source_rect->top;
		const GLfloat texture_right = source_rect->right;
		const GLfloat texture_bottom = source_rect->bottom;

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

void RenderBackend_Blit(RenderBackend_Surface *source_surface, const RenderBackend_Rect *rect, RenderBackend_Surface *destination_surface, long x, long y, bool colour_key)
{
	const RenderBackend_Rect destination_rect = {x, y, x + (rect->right - rect->left), y + (rect->bottom - rect->top)};

	Blit(source_surface, rect, destination_surface, &destination_rect, colour_key);
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

		const GLfloat vertex_transform[4 * 4] = {
			2.0f / surface->width, 0.0f, 0.0f, -1.0f,
			0.0f, 2.0f / surface->height, 0.0f, -1.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		glUseProgram(program_colour_fill.id);
		glUniformMatrix4fv(program_colour_fill.uniforms.vertex_transform, 1, GL_FALSE, vertex_transform);

		glDisable(GL_BLEND);

		// Disable texture coordinate array, since this doesn't use textures
		glDisableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glUniform4f(program_colour_fill.uniforms.colour, red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
	}

	// Add data to the vertex queue
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = rect->left;
		const GLfloat vertex_top = rect->top;
		const GLfloat vertex_right = rect->right;
		const GLfloat vertex_bottom = rect->bottom;

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

void RenderBackend_UploadGlyph(RenderBackend_GlyphAtlas *atlas, size_t x, size_t y, const unsigned char *pixels, size_t width, size_t height, size_t pitch)
{
#ifdef USE_OPENGLES2
	unsigned char *buffer = (unsigned char*)malloc(width * height);

	if (buffer == NULL)
		return;

	for (size_t y = 0; y < height; ++y)
		memcpy (&buffer[y * width], &pixels[y * pitch], width);
#else
	const unsigned char *buffer = pixels;

	glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch);
#endif

	SetTextureUploadAlignment(width);
	glBindTexture(GL_TEXTURE_2D, atlas->texture_id);
#ifdef USE_OPENGLES2
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, buffer);
#endif
	glBindTexture(GL_TEXTURE_2D, last_source_texture);

#ifdef USE_OPENGLES2
	free(buffer);
#else
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
}

void RenderBackend_PrepareToDrawGlyphs(RenderBackend_GlyphAtlas *atlas, RenderBackend_Surface *destination_surface, unsigned char red, unsigned char green, unsigned char blue)
{
	static unsigned char last_red;
	static unsigned char last_green;
	static unsigned char last_blue;

	// Flush vertex data if a context-change is needed
	if (last_render_mode != MODE_DRAW_GLYPH || last_source_texture != atlas->texture_id || last_destination_texture != destination_surface->texture_id || last_red != red || last_green != green || last_blue != blue)
	{
		FlushVertexBuffer();

		last_render_mode = MODE_DRAW_GLYPH;
		last_source_texture = atlas->texture_id;
		last_destination_texture = destination_surface->texture_id;
		last_red = red;
		last_green = green;
		last_blue = blue;

		const GLfloat vertex_transform[4 * 4] = {
			2.0f / destination_surface->width, 0.0f, 0.0f, -1.0f,
			0.0f, 2.0f / destination_surface->height, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		glUseProgram(program_glyph.id);
		glUniform2f(program_glyph.uniforms.texture_coordinate_transform, 1.0f / atlas->width, 1.0f / atlas->height);
		glUniform4f(program_glyph.uniforms.colour, red / 255.0f, green / 255.0f, blue / 255.0f, 1.0f);
		glUniformMatrix4fv(program_glyph.uniforms.vertex_transform, 1, GL_FALSE, vertex_transform);

		// Point our framebuffer to the destination texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destination_surface->texture_id, 0);
		glViewport(0, 0, destination_surface->width, destination_surface->height);

		glEnable(GL_BLEND);

		// Enable texture coordinates, since this uses textures
		glEnableVertexAttribArray(ATTRIBUTE_INPUT_TEXTURE_COORDINATES);

		glBindTexture(GL_TEXTURE_2D, atlas->texture_id);
	}
}

void RenderBackend_DrawGlyph(long x, long y, size_t glyph_x, size_t glyph_y, size_t glyph_width, size_t glyph_height)
{
	// Add data to the vertex queue
	VertexBufferSlot *vertex_buffer_slot = GetVertexBufferSlot();

	if (vertex_buffer_slot != NULL)
	{
		const GLfloat vertex_left = x;
		const GLfloat vertex_top = y;
		const GLfloat vertex_right = x + glyph_width;
		const GLfloat vertex_bottom = y + glyph_height;

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

		const GLfloat texture_left = glyph_x;
		const GLfloat texture_top = glyph_y;
		const GLfloat texture_right = glyph_x + glyph_width;
		const GLfloat texture_bottom = glyph_y + glyph_height;

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
	size_t upscale_factor = MAX(1, MIN((width + framebuffer_surface->width / 2) / framebuffer_surface->width, (height + framebuffer_surface->height / 2) / framebuffer_surface->height));

	size_t upscaled_framebuffer_width = framebuffer_surface->width * upscale_factor;
	size_t upscaled_framebuffer_height = framebuffer_surface->height * upscale_factor;

	if (upscaled_framebuffer_surface != NULL)
	{
		RenderBackend_FreeSurface(upscaled_framebuffer_surface);
		upscaled_framebuffer_surface = NULL;
	}

	// Create rect that forces 4:3 no matter what size the window is
	if (width * upscaled_framebuffer_height >= upscaled_framebuffer_width * height) // Fancy way to do `if (width / height >= upscaled_framebuffer->width / upscaled_framebuffer->height)` without floats
	{
		window_rect.right = (height * upscaled_framebuffer_width) / upscaled_framebuffer_height;
		window_rect.bottom = height;
	}
	else
	{
		window_rect.right = width;
		window_rect.bottom = (width * upscaled_framebuffer_height) / upscaled_framebuffer_width;
	}

	window_rect.left = (width - window_rect.right) / 2;
	window_rect.top = (height - window_rect.bottom) / 2;
	window_rect.right += window_rect.left;
	window_rect.bottom += window_rect.top;

	window_surface.width = width;
	window_surface.height = height;

	if ((window_rect.right - window_rect.left) % framebuffer_surface->width != 0 || (window_rect.bottom - window_rect.top) % framebuffer_surface->height != 0)
	{
		upscaled_framebuffer_surface = CreateSurface(upscaled_framebuffer_width, upscaled_framebuffer_height, true);

		if (upscaled_framebuffer_surface == NULL)
			Backend_PrintError("Couldn't regenerate upscaled framebuffer");
	}
}
