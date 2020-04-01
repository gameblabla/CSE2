#include "../Window.h"

#ifdef USE_OPENGLES2
#include <GLES2/gl2.h>
#else
#include "../../../external/glad/include/glad/glad.h"
#endif

#include <GLFW/glfw3.h>

#include <stddef.h>

#include "../../WindowsWrapper.h"

#include "../Platform.h"
#include "../../Resource.h"

// Horrible hacks
GLFWwindow *window;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void WindowFocusCallback(GLFWwindow *window, int focused);
void WindowSizeCallback(GLFWwindow *window, int width, int height);

BOOL WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, BOOL fullscreen)
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
			const GLFWvidmode *mode = glfwGetVideoMode(monitor);

			*screen_width = mode->width;
			*screen_height = mode->height;
		}
	}

	window = glfwCreateWindow(*screen_width, *screen_height, window_title, monitor, NULL);

	if (window != NULL)
	{/*
	#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
		size_t resource_size;
		const unsigned char *resource_data = FindResource("ICON_MINI", "ICON", &resource_size);
		SDL_RWops *rwops = SDL_RWFromConstMem(resource_data, resource_size);
		SDL_Surface *icon_surface = SDL_LoadBMP_RW(rwops, 1);
		SDL_SetWindowIcon(window, icon_surface);
		SDL_FreeSurface(icon_surface);
	#endif
*/

		glfwMakeContextCurrent(window);

			#ifndef USE_OPENGLES2
				if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
				{
					// Check if the platform supports OpenGL 3.2
					if (GLAD_GL_VERSION_3_2)
					{
			#endif
						glfwSetKeyCallback(window, KeyCallback);
						glfwSetWindowFocusCallback(window, WindowFocusCallback);
						glfwSetWindowSizeCallback(window, WindowSizeCallback);

						return TRUE;
			#ifndef USE_OPENGLES2
					}
					else
					{
						PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Your system does not support OpenGL 3.2");
					}
				}
				else
				{
					PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not load OpenGL functions");
				}
			#endif

		glfwDestroyWindow(window);
	}
	else
	{
		PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create window");
	}

	return FALSE;
}

void WindowBackend_OpenGL_DestroyWindow(void)
{
	glfwDestroyWindow(window);
}

void WindowBackend_OpenGL_Display(void)
{
	glfwSwapBuffers(window);
}
