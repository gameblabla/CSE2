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

Backend_Surface* Backend_Init(const char *window_title, int screen_width, int screen_height, BOOL fullscreen)
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

	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_OPENGL);

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
						return RenderBackend_Init(screen_width, screen_height);
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

	return NULL;
}

void Backend_Deinit(void)
{
	RenderBackend_Deinit();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
}

void Backend_DrawScreen(void)
{
	RenderBackend_DrawScreen();

	SDL_GL_SwapWindow(window);

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
