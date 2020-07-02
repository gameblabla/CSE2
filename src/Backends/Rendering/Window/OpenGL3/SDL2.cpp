#include "../OpenGL.h"

#include <stddef.h>
#include <string>

#ifndef USE_OPENGLES2
#include <glad/glad.h>
#endif
#include "SDL.h"

#include "../../../Misc.h"
#include "../../../Shared/SDL2.h"

SDL_Window *window;

static SDL_GLContext context;

bool WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, bool fullscreen, bool vsync)
{
#ifdef USE_OPENGLES2
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES) < 0)
		Backend_PrintError("Couldn't set OpenGL context type to ES: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0) < 0)
		Backend_PrintError("Couldn't set OpenGL context flags to 0: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2) < 0)
		Backend_PrintError("Couldn't set OpenGL major version to 2: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0) < 0)
		Backend_PrintError("Couldn't set OpenGL minor version to 0: %s", SDL_GetError());
#else
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0)
		Backend_PrintError("Couldn't set OpenGL context type to core: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) < 0)
		Backend_PrintError("Couldn't set OpenGL forward compatibility: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) < 0)
		Backend_PrintError("Couldn't set OpenGL major version to 3: %s", SDL_GetError());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) < 0)
		Backend_PrintError("Couldn't set OpenGL minor verison to 2: %s", SDL_GetError());
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
						if (vsync)
							SDL_GL_SetSwapInterval(1);

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
					Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Couldn't load OpenGL functions");
				}
			#endif
			}
			else
			{
				std::string error_message = std::string("Couldn't setup OpenGL context for rendering: ") + SDL_GetError();
				Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "SDL_GL_MakeCurrent failed");
			}

			SDL_GL_DeleteContext(context);
		}
		else
		{
			std::string error_message = std::string("Couldn't create OpenGL context: %s", SDL_GetError());
			Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", "Could not create OpenGL context");
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		std::string error_message = std::string("Could not create window: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (OpenGL rendering backend)", error_message.c_str());
	}

	return false;
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
