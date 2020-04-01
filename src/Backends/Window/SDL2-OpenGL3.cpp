#include "../Window.h"

#ifdef USE_OPENGLES2
#include <GLES2/gl2.h>
#else
#include "../../../external/glad/include/glad/glad.h"
#endif

#include "SDL.h"

#include <stddef.h>

#include "../../WindowsWrapper.h"

#include "../../Resource.h"

static SDL_Window *window;
static SDL_GLContext context;

BOOL WindowBackend_OpenGL_CreateWindow(const char *window_title, int *screen_width, int *screen_height, BOOL fullscreen)
{
	puts("Available SDL2 video drivers:");

	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
		puts(SDL_GetVideoDriver(i));

	printf("Selected SDL2 video driver: %s\n", SDL_GetCurrentVideoDriver());

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

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, *screen_width, *screen_height, SDL_WINDOW_OPENGL);

	if (window != NULL)
	{
	#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
		size_t resource_size;
		const unsigned char *resource_data = FindResource("ICON_MINI", "ICON", &resource_size);
		SDL_RWops *rwops = SDL_RWFromConstMem(resource_data, resource_size);
		SDL_Surface *icon_surface = SDL_LoadBMP_RW(rwops, 1);
		SDL_SetWindowIcon(window, icon_surface);
		SDL_FreeSurface(icon_surface);
	#endif

		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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
						return TRUE;
			#ifndef USE_OPENGLES2
					}
					else
					{
						SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (OpenGL rendering backend)", "Your system does not support OpenGL 3.2", window);
					}
				}
				else
				{
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (OpenGL rendering backend)", "Could not load OpenGL functions", window);
				}
			#endif
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (OpenGL rendering backend)", "SDL_GL_MakeCurrent failed", window);
			}

			SDL_GL_DeleteContext(context);
		}
		else
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (OpenGL rendering backend)", "Could not create OpenGL context", window);
		}

		SDL_DestroyWindow(window);
	}
	else
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal error (OpenGL rendering backend)", "Could not create window", NULL);
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
