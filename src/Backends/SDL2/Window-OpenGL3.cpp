#include "../Window-OpenGL.h"

#include <stddef.h>

#ifdef USE_OPENGLES2
#include <GLES2/gl2.h>
#else
#include <glad/glad.h>
#endif
#include "SDL.h"

#include "../../WindowsWrapper.h"

#include "../Platform.h"
#include "Platform.h"
#include "../../Resource.h"

static SDL_GLContext context;

BOOL WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, BOOL fullscreen)
{
#ifdef USE_OPENGLES2
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, *screen_width, *screen_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));

	if (window != NULL)
	{
		context = SDL_GL_CreateContext(window);

		if (context != NULL)
		{
			if (SDL_GL_MakeCurrent(window, context) == 0)
			{
			#ifndef USE_OPENGLES2
				if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
				{
					// Check if the platform supports OpenGL 3.2
					if (GLAD_GL_VERSION_3_2)
					{
			#endif
						PlatformBackend_PostWindowCreation();

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
			}
			else
			{
				PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "SDL_GL_MakeCurrent failed");
			}

			SDL_GL_DeleteContext(context);
		}
		else
		{
			PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create OpenGL context");
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		PlatformBackend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create window");
	}

	return FALSE;
}

void WindowBackend_OpenGL_DestroyWindow(void)
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
}

void WindowBackend_OpenGL_Display(void)
{
	SDL_GL_SwapWindow(window);
}
