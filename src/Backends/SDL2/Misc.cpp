#include "../Platform.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "../Rendering.h"

#include "../../WindowsWrapper.h"

#include "../SDL2.h"
#include "../../KeyControl.h"
#include "../../Main.h"
#include "../../Organya.h"
#include "../../Profile.h"
#include "../../Resource.h"

SDL_Window *window;

BOOL bActive = TRUE;

static SDL_Surface *cursor_surface;
static SDL_Cursor *cursor;

void PlatformBackend_Init(void)
{
	SDL_Init(SDL_INIT_EVENTS);

	SDL_InitSubSystem(SDL_INIT_VIDEO);

	puts("Available SDL2 video drivers:");

	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
		puts(SDL_GetVideoDriver(i));

	printf("Selected SDL2 video driver: %s\n", SDL_GetCurrentVideoDriver());
}

void PlatformBackend_Deinit(void)
{
	if (cursor != NULL)
		SDL_FreeCursor(cursor);

	if (cursor_surface != NULL)
		SDL_FreeSurface(cursor_surface);

	SDL_Quit();
}

void PlatformBackend_PostWindowCreation(void)
{
	
}

BOOL PlatformBackend_GetBasePath(char *string_buffer)
{
	char *base_path = SDL_GetBasePath();
	// Trim the trailing '/'
	size_t base_path_length = strlen(base_path);
	base_path[base_path_length - 1] = '\0';
	strcpy(string_buffer, base_path);
	SDL_free(base_path);

	return TRUE;
}

void PlatformBackend_HideMouse(void)
{
	SDL_ShowCursor(SDL_DISABLE);
}

void PlatformBackend_SetWindowIcon(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)rgb_pixels, width, height, 0, width * 3, SDL_PIXELFORMAT_RGB24);
	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);
}

void PlatformBackend_SetCursor(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	cursor_surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)rgb_pixels, width, height, 0, width * 3, SDL_PIXELFORMAT_RGB24);
	SDL_SetColorKey(cursor_surface, SDL_TRUE, SDL_MapRGB(cursor_surface->format, 0xFF, 0, 0xFF));
	cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);
	SDL_SetCursor(cursor);
}

void PlaybackBackend_EnableDragAndDrop(void)
{
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

BOOL PlatformBackend_SystemTask(void)
{
	while (SDL_PollEvent(NULL) || !bActive)
	{
		SDL_Event event;

		if (!SDL_WaitEvent(&event))
			return FALSE;

		switch (event.type)
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gKey |= KEY_ESCAPE;
						break;

					case SDLK_w:
						gKey |= KEY_MAP;
						break;

					case SDLK_LEFT:
						gKey |= KEY_LEFT;
						break;

					case SDLK_RIGHT:
						gKey |= KEY_RIGHT;
						break;

					case SDLK_UP:
						gKey |= KEY_UP;
						break;

					case SDLK_DOWN:
						gKey |= KEY_DOWN;
						break;

					case SDLK_x:
						gKey |= KEY_X;
						break;

					case SDLK_z:
						gKey |= KEY_Z;
						break;

					case SDLK_s:
						gKey |= KEY_ARMS;
						break;

					case SDLK_a:
						gKey |= KEY_ARMSREV;
						break;

					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
						gKey |= KEY_SHIFT;
						break;

					case SDLK_F1:
						gKey |= KEY_F1;
						break;

					case SDLK_F2:
						gKey |= KEY_F2;
						break;

					case SDLK_q:
						gKey |= KEY_ITEM;
						break;

					case SDLK_COMMA:
						gKey |= KEY_ALT_LEFT;
						break;

					case SDLK_PERIOD:
						gKey |= KEY_ALT_DOWN;
						break;

					case SDLK_SLASH:
						gKey |= KEY_ALT_RIGHT;
						break;

					case SDLK_l:
						gKey |= KEY_L;
						break;

					case SDLK_PLUS:
						gKey |= KEY_PLUS;
						break;

					case SDLK_F5:
						gbUseJoystick = FALSE;
						break;
				}

				break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						gKey &= ~KEY_ESCAPE;
						break;

					case SDLK_w:
						gKey &= ~KEY_MAP;
						break;

					case SDLK_LEFT:
						gKey &= ~KEY_LEFT;
						break;

					case SDLK_RIGHT:
						gKey &= ~KEY_RIGHT;
						break;

					case SDLK_UP:
						gKey &= ~KEY_UP;
						break;

					case SDLK_DOWN:
						gKey &= ~KEY_DOWN;
						break;

					case SDLK_x:
						gKey &= ~KEY_X;
						break;

					case SDLK_z:
						gKey &= ~KEY_Z;
						break;

					case SDLK_s:
						gKey &= ~KEY_ARMS;
						break;

					case SDLK_a:
						gKey &= ~KEY_ARMSREV;
						break;

					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
						gKey &= ~KEY_SHIFT;
						break;

					case SDLK_F1:
						gKey &= ~KEY_F1;
						break;

					case SDLK_F2:
						gKey &= ~KEY_F2;
						break;

					case SDLK_q:
						gKey &= ~KEY_ITEM;
						break;

					case SDLK_COMMA:
						gKey &= ~KEY_ALT_LEFT;
						break;

					case SDLK_PERIOD:
						gKey &= ~KEY_ALT_DOWN;
						break;

					case SDLK_SLASH:
						gKey &= ~KEY_ALT_RIGHT;
						break;

					case SDLK_l:
						gKey &= ~KEY_L;
						break;

					case SDLK_PLUS:
						gKey &= ~KEY_PLUS;
						break;
				}

				break;

			case SDL_DROPFILE:
				LoadProfile(event.drop.file);
				SDL_free(event.drop.file);
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_LOST:
						InactiveWindow();
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:
						ActiveWindow();
						break;

					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						Backend_HandleWindowResize(event.window.data1, event.window.data2);
						break;
				}

				break;

			case SDL_QUIT:
				StopOrganyaMusic();
				return FALSE;

			case SDL_RENDER_TARGETS_RESET:
				Backend_HandleRenderTargetLoss();
				break;

		}
	}

	return TRUE;
}

void PlatformBackend_ShowMessageBox(const char *title, const char *message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window);
}

unsigned long PlatformBackend_GetTicks(void)
{
	return SDL_GetTicks();
}

void PlatformBackend_Delay(unsigned int ticks)
{
	SDL_Delay(ticks);
}
