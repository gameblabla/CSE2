#include "../Rendering.h"

#include <stddef.h>
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

static SDL_Window *window;
static SDL_GLContext context;
static GLuint colour_key_program_id;
static GLuint framebuffer_id;

static Backend_Surface framebuffer_surface;

static const GLchar *fragment_shader_source = " \
#version 120\n \
uniform sampler2D tex; \
void main() \
{ \
	vec4 colour = gl_Color * texture2D(tex, gl_TexCoord[0].st); \
\
	if (colour.xyz == vec3(0.0f, 0.0f, 0.0f)) \
		discard; \
\
	gl_FragColor = colour; \
} \
";

static GLuint CompileShader(const char *fragment_shader_source)
{
	GLint shader_status;

	GLuint program_id = glCreateProgram();

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
		return 0;

	glAttachShader(program_id, fragment_shader);

	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &shader_status);
	if (shader_status != GL_TRUE)
		return 0;

	return program_id;
}

BOOL Backend_Init(SDL_Window *p_window)
{
	window = p_window;

	int screen_width, screen_height;
	SDL_GetWindowSize(window, &screen_width, &screen_height);

	context = SDL_GL_CreateContext(window);

	if (glewInit() != GLEW_OK)
		return FALSE;

	// Check for framebuffer object extension (is part of the core spec in OpenGL 3.0, but not 2.1)
	if (!GLEW_EXT_framebuffer_object)
		return FALSE;

//	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0.0, screen_width, 0.0, screen_height, 1.0, -1.0);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set up blank default texture (it seems to be black by default, which sucks for colour modulation)
	const unsigned char white_pixel[3] = {0xFF, 0xFF, 0xFF};
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white_pixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Set up our colour-key-enabled fragment shader
	colour_key_program_id = CompileShader(fragment_shader_source);

	// Set up framebuffer (used for surface-to-surface blitting)
	glGenFramebuffersEXT(1, &framebuffer_id);

	// Set up framebuffer screen texture (used for screen-to-surface blitting)
	glGenTextures(1, &framebuffer_surface.texture_id);
	glBindTexture(GL_TEXTURE_2D, framebuffer_surface.texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	framebuffer_surface.width = screen_width;
	framebuffer_surface.height = screen_height;

	return TRUE;
}

void Backend_Deinit(void)
{
	glDeleteTextures(1, &framebuffer_surface.texture_id);
	glDeleteFramebuffersEXT(1, &framebuffer_id);
	glDeleteProgram(colour_key_program_id);
	SDL_GL_DeleteContext(context);
}

void Backend_DrawScreen(void)
{
	// Disable colour-keying
	glUseProgram(0);

	// Target actual screen, and not our framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// Draw framebuffer to screen
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, framebuffer_surface.texture_id);

	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
	glEnd();

	glPopMatrix();

	SDL_GL_SwapWindow(window);

	// According to https://www.khronos.org/opengl/wiki/Common_Mistakes#Swap_Buffers
	// the buffer should always be cleared
	glClear(GL_COLOR_BUFFER_BIT);

	// Switch back to our framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
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

	surface->width = width;
	surface->height = height;

	return surface;
}

void Backend_FreeSurface(Backend_Surface *surface)
{
	glDeleteTextures(1, &surface->texture_id);
	free(surface);
}

unsigned char* Backend_Lock(Backend_Surface *surface, unsigned int *pitch)
{
	surface->pixels = (unsigned char*)malloc(surface->width * surface->height * 3);
	*pitch = surface->width * 3;
	return surface->pixels;
}

void Backend_Unlock(Backend_Surface *surface)
{
	glBindTexture(GL_TEXTURE_2D, surface->texture_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface->width, surface->height, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	free(surface->pixels);
}

static void BlitCommon(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	// Switch to colour-key shader if we have to
	glUseProgram(colour_key ? colour_key_program_id : 0);

	glBindTexture(GL_TEXTURE_2D, source_surface->texture_id);

	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f((GLfloat)rect->left / (GLfloat)source_surface->width, (GLfloat)rect->top / (GLfloat)source_surface->height);
		glVertex2f((GLfloat)x, (GLfloat)y);
		glTexCoord2f((GLfloat)rect->right / (GLfloat)source_surface->width, (GLfloat)rect->top / (GLfloat)source_surface->height);
		glVertex2f((GLfloat)x + (rect->right - rect->left), (GLfloat)y);
		glTexCoord2f((GLfloat)rect->right / (GLfloat)source_surface->width, (GLfloat)rect->bottom / (GLfloat)source_surface->height);
		glVertex2f((GLfloat)x + (rect->right - rect->left), (GLfloat)y + (rect->bottom - rect->top));
		glTexCoord2f((GLfloat)rect->left / (GLfloat)source_surface->width, (GLfloat)rect->bottom / (GLfloat)source_surface->height);
		glVertex2f((GLfloat)x, (GLfloat)y + (rect->bottom - rect->top));
	glEnd();
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	// Point our framebuffer to the destination texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, destination_surface->texture_id, 0);

	// The matrix needs resetting to fit the dimensions of the destination texture
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, destination_surface->width, 0.0, destination_surface->height, 1.0, -1.0);

	BlitCommon(source_surface, rect, x, y, colour_key);

	glPopMatrix();
}

void Backend_BlitToScreen(Backend_Surface *source_surface, const RECT *rect, long x, long y, BOOL colour_key)
{
	// Point our framebuffer to the screen texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, framebuffer_surface.texture_id, 0);

	BlitCommon(source_surface, rect, x, y, colour_key);
}

static void ColourFillCommon(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	// Disable colour-keying
	glUseProgram(0);

	// Use blank default texture, for a solid colour-fill
	glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_QUADS);
		glColor3f(red / 255.0f, green / 255.0f, blue / 255.0f);
		glVertex2f((GLfloat)rect->left, (GLfloat)rect->top);
		glVertex2f((GLfloat)rect->right, (GLfloat)rect->top);
		glVertex2f((GLfloat)rect->right, (GLfloat)rect->bottom);
		glVertex2f((GLfloat)rect->left, (GLfloat)rect->bottom);
	glEnd();
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{	
	// Point our framebuffer to the destination texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, surface->texture_id, 0);

	// The matrix needs resetting to fit the dimensions of the destination texture
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, surface->width, 0.0, surface->height, 1.0, -1.0);

	ColourFillCommon(rect, red, green, blue);

	glPopMatrix();
}

void Backend_ColourFillToScreen(const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	// Point our framebuffer to the screen texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, framebuffer_surface.texture_id, 0);

	ColourFillCommon(rect, red, green, blue);
}

void Backend_ScreenToSurface(Backend_Surface *surface, const RECT *rect)
{
	// Point our framebuffer to the destination texture
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, surface->texture_id, 0);

	// The matrix needs resetting to fit the dimensions of the destination texture
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, surface->width, 0.0, surface->height, 1.0, -1.0);

	BlitCommon(&framebuffer_surface, rect, rect->left, rect->top, FALSE);

	glPopMatrix();
}

void Backend_DrawText(Backend_Surface *surface, FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	// Soon
}

void Backend_DrawTextToScreen(FontObject *font, int x, int y, const char *text, unsigned long colour)
{
	// Soon
}

void Backend_HandleDeviceLoss(void)
{
	// No problem for us
}

void Backend_HandleWindowResize(void)
{
	// No problem for us
}
