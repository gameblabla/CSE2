// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../OpenGL.h"

#include <stddef.h>

#ifndef USE_OPENGLES2
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#include "../../../Misc.h"
#include "../../../Shared/GLFW3.h"

GLFWwindow *window;

bool WindowBackend_OpenGL_CreateWindow(const char *window_title, size_t *screen_width, size_t *screen_height, bool fullscreen)
{
#ifdef USE_OPENGLES2
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif

	GLFWmonitor *monitor = NULL;

	if (fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();

		if (monitor != NULL)
		{
			// Use current monitor resolution, because for some reason
			// 640x480 causes my laptop to completely freeze on Linux.
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			*screen_width = mode->width;
			*screen_height = mode->height;
		}
	}

	window = glfwCreateWindow(*screen_width, *screen_height, window_title, monitor, NULL);

	if (window != NULL)
	{
		glfwMakeContextCurrent(window);

	#ifndef USE_OPENGLES2
		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			// Check if the platform supports OpenGL 3.2
			if (GLAD_GL_VERSION_3_2)
			{
	#endif
				glfwSwapInterval(0);	// Disable vsync

				Backend_PostWindowCreation();

				return true;
	#ifndef USE_OPENGLES2
			}
			else
			{
				Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Your system does not support OpenGL 3.2");
			}
		}
		else
		{
			Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not initialize OpenGL context");
		}
	#endif

		glfwDestroyWindow(window);
	}
	else
	{
		Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create window");
	}

	return false;
}

void WindowBackend_OpenGL_DestroyWindow(void)
{
	glfwDestroyWindow(window);
}

void WindowBackend_OpenGL_Display(void)
{
	glfwSwapBuffers(window);
}
