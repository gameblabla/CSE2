#include "../Software.h"

#include <stddef.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "../../../Misc.h"
#include "../../../Shared/GLFW3.h"

GLFWwindow *window;

static unsigned char *framebuffer;
static int framebuffer_width;
static int framebuffer_height;

static float framebuffer_x_ratio;
static float framebuffer_y_ratio;

static GLuint screen_texture_id;

bool WindowBackend_Software_CreateWindow(const char *window_title, int screen_width, int screen_height, bool fullscreen, bool *vsync)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	framebuffer_width = screen_width;
	framebuffer_height = screen_height;

	GLFWmonitor *monitor = NULL;

	if (fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();

		if (monitor != NULL)
		{
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			screen_width = mode->width;
			screen_height = mode->height;
		}
	}

	window = glfwCreateWindow(screen_width, screen_height, window_title, monitor, NULL);

	if (window != NULL)
	{
		glfwMakeContextCurrent(window);

		if (*vsync)
			glfwSwapInterval(1);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glEnable(GL_TEXTURE_2D);

		WindowBackend_Software_HandleWindowResize(screen_width, screen_height);

		// Create screen texture
		glGenTextures(1, &screen_texture_id);
		glBindTexture(GL_TEXTURE_2D, screen_texture_id);

		int framebuffer_texture_width = 1;
		while (framebuffer_texture_width < framebuffer_width)
			framebuffer_texture_width <<= 1;

		int framebuffer_texture_height = 1;
		while (framebuffer_texture_height < framebuffer_height)
			framebuffer_texture_height <<= 1;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer_texture_width, framebuffer_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		framebuffer_x_ratio = (float)framebuffer_width / framebuffer_texture_width;
		framebuffer_y_ratio = (float)framebuffer_height / framebuffer_texture_height;

		framebuffer = (unsigned char*)malloc(framebuffer_width * framebuffer_height * 4);

		Backend_PostWindowCreation();

		return true;
	}
	else
	{
		Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create window");
	}

	return false;
}

void WindowBackend_Software_DestroyWindow(void)
{
	free(framebuffer);
	glDeleteTextures(1, &screen_texture_id);
	glfwDestroyWindow(window);
}

unsigned char* WindowBackend_Software_GetFramebuffer(size_t *pitch)
{
	*pitch = framebuffer_width * 4;

	return framebuffer;
}

void WindowBackend_Software_Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, framebuffer_width, framebuffer_height, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, framebuffer_y_ratio);
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(framebuffer_x_ratio, framebuffer_y_ratio);
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(framebuffer_x_ratio, 0.0f);
		glVertex2f(1.0f, 1.0f);
	glEnd();

	glfwSwapBuffers(window);
}

void WindowBackend_Software_HandleWindowResize(unsigned int width, unsigned int height)
{
	// Do some viewport trickery, to fit the framebuffer in the center of the screen
	GLint viewport_x;
	GLint viewport_y;
	GLsizei viewport_width;
	GLsizei viewport_height;

	if (width * framebuffer_height > framebuffer_width * height) // Fancy way to do `if (width / height > framebuffer_width / framebuffer_height)` without floats
	{
		viewport_y = 0;
		viewport_height = height;

		viewport_width = (framebuffer_width * height) / framebuffer_height;
		viewport_x = (width - viewport_width) / 2;
	}
	else
	{
		viewport_x = 0;
		viewport_width = width;

		viewport_height = (framebuffer_height * width) / framebuffer_width;
		viewport_y = (height - viewport_height) / 2;
	}

	glViewport(viewport_x, viewport_y, viewport_width, viewport_height);
}
