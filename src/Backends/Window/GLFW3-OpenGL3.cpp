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

Backend_Surface* Backend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen)
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

	window = glfwCreateWindow(screen_width, screen_height, window_title, NULL, NULL);

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
		if (fullscreen)
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screen_width, screen_height, GLFW_DONT_CARE);

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

						return RenderBackend_Init(screen_width, screen_height);
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

	return NULL;
}

void Backend_Deinit(void)
{
	RenderBackend_Deinit();

	glfwDestroyWindow(window);
}

void Backend_DrawScreen(void)
{
	RenderBackend_DrawScreen();

	glfwSwapBuffers(window);

	RenderBackend_ClearScreen();
}

Backend_Surface* Backend_CreateSurface(unsigned int width, unsigned int height)
{
	return RenderBackend_CreateSurface(width, height);

}

void Backend_FreeSurface(Backend_Surface *surface)
{
	RenderBackend_FreeSurface(surface);
}

BOOL Backend_IsSurfaceLost(Backend_Surface *surface)
{
	return RenderBackend_IsSurfaceLost(surface);
}

void Backend_RestoreSurface(Backend_Surface *surface)
{
	RenderBackend_RestoreSurface(surface);
}

unsigned char* Backend_LockSurface(Backend_Surface *surface, unsigned int *pitch, unsigned int width, unsigned int height)
{
	return RenderBackend_LockSurface(surface, pitch, width, height);
}

void Backend_UnlockSurface(Backend_Surface *surface, unsigned int width, unsigned int height)
{
	RenderBackend_UnlockSurface(surface, width, height);
}

void Backend_Blit(Backend_Surface *source_surface, const RECT *rect, Backend_Surface *destination_surface, long x, long y, BOOL colour_key)
{
	RenderBackend_Blit(source_surface, rect, destination_surface, x, y, colour_key);
}

void Backend_ColourFill(Backend_Surface *surface, const RECT *rect, unsigned char red, unsigned char green, unsigned char blue)
{
	RenderBackend_ColourFill(surface, rect, red, green, blue);
}

Backend_Glyph* Backend_LoadGlyph(const unsigned char *pixels, unsigned int width, unsigned int height, int pitch)
{
	return RenderBackend_LoadGlyph(pixels, width, height, pitch);
}

void Backend_UnloadGlyph(Backend_Glyph *glyph)
{
	RenderBackend_UnloadGlyph(glyph);
}

void Backend_PrepareToDrawGlyphs(Backend_Surface *destination_surface, const unsigned char *colour_channels)
{
	RenderBackend_PrepareToDrawGlyphs(destination_surface, colour_channels);
}

void Backend_DrawGlyph(Backend_Glyph *glyph, long x, long y)
{
	RenderBackend_DrawGlyph(glyph, x, y);
}

void Backend_FlushGlyphs(void)
{
	RenderBackend_FlushGlyphs();
}

void Backend_HandleRenderTargetLoss(void)
{
	RenderBackend_HandleRenderTargetLoss();
}

void Backend_HandleWindowResize(void)
{
	RenderBackend_HandleWindowResize();
}
