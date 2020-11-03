// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../OpenGL.h"

#include <stddef.h>
#include <string>

#include <glad/glad.h>
#include "SDL.h"

#include "../../../Misc.h"

bool WindowBackend_OpenGL_CreateWindow(const char *window_title, size_t *screen_width, size_t *screen_height, bool fullscreen)
{
#ifdef SDL_GL_SWAP_CONTROL
	if (SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0) < 0)	// Disable vsync
		Backend_PrintError("Couldn't set OpenGL swap interval: %s", SDL_GetError());
#endif

	if (SDL_SetVideoMode(*screen_width, *screen_height, 0, SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0)) != NULL)
	{
		SDL_WM_SetCaption(window_title, NULL);

		if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			// Check if the platform supports OpenGL 3.2
			if (GLAD_GL_VERSION_3_2)
			{
				Backend_PostWindowCreation();

				return true;
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
}

void WindowBackend_OpenGL_Display(void)
{
	SDL_GL_SwapBuffers();
}
