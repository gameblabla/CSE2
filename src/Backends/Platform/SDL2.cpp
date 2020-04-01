#include "../Platform.h"

#include "SDL.h"

#include "../Rendering.h"

#include "../../WindowsWrapper.h"

#include "../../KeyControl.h"
#include "../../Main.h"
#include "../../Organya.h"
#include "../../Profile.h"
#include "../../Resource.h"

extern SDL_Window *window;

BOOL bActive = TRUE;

void PlatformBackend_Init(void)
{
	SDL_Init(SDL_INIT_EVENTS);

#ifdef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "102");
#endif

	SDL_InitSubSystem(SDL_INIT_VIDEO);

	puts("Available SDL2 video drivers:");

	for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
		puts(SDL_GetVideoDriver(i));

	printf("Selected SDL2 video driver: %s\n", SDL_GetCurrentVideoDriver());
}

void PlatformBackend_Deinit(void)
{
	SDL_Quit();
}

void PlatformBackend_PostWindowCreation(void)
{
	// Set up window icon
#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	size_t resource_size;
	const unsigned char *resource_data = FindResource("ICON_MINI", "ICON", &resource_size);
	SDL_RWops *rwops = SDL_RWFromConstMem(resource_data, resource_size);
	SDL_Surface *icon_surface = SDL_LoadBMP_RW(rwops, 1);
	SDL_SetWindowIcon(window, icon_surface);
	SDL_FreeSurface(icon_surface);
#endif
}

BOOL PlatformBackend_GetBasePath(char *string_buffer)
{
	char *base_path = SDL_GetBasePath();
	size_t base_path_length = strlen(base_path);
	base_path[base_path_length - 1] = '\0';
	strcpy(string_buffer, base_path);
	SDL_free(base_path);

	return TRUE;
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
