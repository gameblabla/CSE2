#include "../Platform.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "../Rendering.h"

#include "../../WindowsWrapper.h"

#include "Platform.h"
#include "../../Main.h"
#include "../../Organya.h"
#include "../../Profile.h"
#include "../../Resource.h"

#define DO_KEY(SDL_KEY, BACKEND_KEY) \
	case SDL_KEY: \
		backend_keyboard_state[BACKEND_KEY] = event.key.type == SDL_KEYDOWN; \
		break;

SDL_Window *window;

BOOL bActive = TRUE;
BOOL backend_keyboard_state[BACKEND_KEYBOARD_TOTAL];
BOOL backend_previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];

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
	memcpy(backend_previous_keyboard_state, backend_keyboard_state, sizeof(backend_keyboard_state));

	while (SDL_PollEvent(NULL) || !bActive)
	{
		SDL_Event event;

		if (!SDL_WaitEvent(&event))
			return FALSE;

		switch (event.type)
		{
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					DO_KEY(SDLK_a, BACKEND_KEYBOARD_A)
					DO_KEY(SDLK_b, BACKEND_KEYBOARD_B)
					DO_KEY(SDLK_c, BACKEND_KEYBOARD_C)
					DO_KEY(SDLK_d, BACKEND_KEYBOARD_D)
					DO_KEY(SDLK_e, BACKEND_KEYBOARD_E)
					DO_KEY(SDLK_f, BACKEND_KEYBOARD_F)
					DO_KEY(SDLK_g, BACKEND_KEYBOARD_G)
					DO_KEY(SDLK_h, BACKEND_KEYBOARD_H)
					DO_KEY(SDLK_i, BACKEND_KEYBOARD_I)
					DO_KEY(SDLK_j, BACKEND_KEYBOARD_J)
					DO_KEY(SDLK_k, BACKEND_KEYBOARD_K)
					DO_KEY(SDLK_l, BACKEND_KEYBOARD_L)
					DO_KEY(SDLK_m, BACKEND_KEYBOARD_M)
					DO_KEY(SDLK_n, BACKEND_KEYBOARD_N)
					DO_KEY(SDLK_o, BACKEND_KEYBOARD_O)
					DO_KEY(SDLK_p, BACKEND_KEYBOARD_P)
					DO_KEY(SDLK_q, BACKEND_KEYBOARD_Q)
					DO_KEY(SDLK_r, BACKEND_KEYBOARD_R)
					DO_KEY(SDLK_s, BACKEND_KEYBOARD_S)
					DO_KEY(SDLK_t, BACKEND_KEYBOARD_T)
					DO_KEY(SDLK_u, BACKEND_KEYBOARD_U)
					DO_KEY(SDLK_v, BACKEND_KEYBOARD_V)
					DO_KEY(SDLK_w, BACKEND_KEYBOARD_W)
					DO_KEY(SDLK_x, BACKEND_KEYBOARD_X)
					DO_KEY(SDLK_y, BACKEND_KEYBOARD_Y)
					DO_KEY(SDLK_z, BACKEND_KEYBOARD_Z)
					DO_KEY(SDLK_0, BACKEND_KEYBOARD_0)
					DO_KEY(SDLK_1, BACKEND_KEYBOARD_1)
					DO_KEY(SDLK_2, BACKEND_KEYBOARD_2)
					DO_KEY(SDLK_3, BACKEND_KEYBOARD_3)
					DO_KEY(SDLK_4, BACKEND_KEYBOARD_4)
					DO_KEY(SDLK_5, BACKEND_KEYBOARD_5)
					DO_KEY(SDLK_6, BACKEND_KEYBOARD_6)
					DO_KEY(SDLK_7, BACKEND_KEYBOARD_7)
					DO_KEY(SDLK_8, BACKEND_KEYBOARD_8)
					DO_KEY(SDLK_9, BACKEND_KEYBOARD_9)
					DO_KEY(SDLK_F1, BACKEND_KEYBOARD_F1)
					DO_KEY(SDLK_F2, BACKEND_KEYBOARD_F2)
					DO_KEY(SDLK_F3, BACKEND_KEYBOARD_F3)
					DO_KEY(SDLK_F4, BACKEND_KEYBOARD_F4)
					DO_KEY(SDLK_F5, BACKEND_KEYBOARD_F5)
					DO_KEY(SDLK_F6, BACKEND_KEYBOARD_F6)
					DO_KEY(SDLK_F7, BACKEND_KEYBOARD_F7)
					DO_KEY(SDLK_F8, BACKEND_KEYBOARD_F8)
					DO_KEY(SDLK_F9, BACKEND_KEYBOARD_F9)
					DO_KEY(SDLK_F10, BACKEND_KEYBOARD_F10)
					DO_KEY(SDLK_F11, BACKEND_KEYBOARD_F11)
					DO_KEY(SDLK_F12, BACKEND_KEYBOARD_F12)
					DO_KEY(SDLK_UP, BACKEND_KEYBOARD_UP)
					DO_KEY(SDLK_DOWN, BACKEND_KEYBOARD_DOWN)
					DO_KEY(SDLK_LEFT, BACKEND_KEYBOARD_LEFT)
					DO_KEY(SDLK_RIGHT, BACKEND_KEYBOARD_RIGHT)
					DO_KEY(SDLK_ESCAPE, BACKEND_KEYBOARD_ESCAPE)
					DO_KEY(SDLK_BACKQUOTE, BACKEND_KEYBOARD_BACK_QUOTE)
					DO_KEY(SDLK_TAB, BACKEND_KEYBOARD_TAB)
					DO_KEY(SDLK_CAPSLOCK, BACKEND_KEYBOARD_CAPS_LOCK)
					DO_KEY(SDLK_LSHIFT, BACKEND_KEYBOARD_LEFT_SHIFT)
					DO_KEY(SDLK_LCTRL, BACKEND_KEYBOARD_LEFT_CTRL)
					DO_KEY(SDLK_LALT, BACKEND_KEYBOARD_LEFT_ALT)
					DO_KEY(SDLK_SPACE, BACKEND_KEYBOARD_SPACE)
					DO_KEY(SDLK_RALT, BACKEND_KEYBOARD_RIGHT_ALT)
					DO_KEY(SDLK_RCTRL, BACKEND_KEYBOARD_RIGHT_CTRL)
					DO_KEY(SDLK_RSHIFT, BACKEND_KEYBOARD_RIGHT_SHIFT)
					DO_KEY(SDLK_RETURN, BACKEND_KEYBOARD_ENTER)
					DO_KEY(SDLK_BACKSPACE, BACKEND_KEYBOARD_BACKSPACE)
					DO_KEY(SDLK_MINUS, BACKEND_KEYBOARD_MINUS)
					DO_KEY(SDLK_EQUALS, BACKEND_KEYBOARD_EQUALS)
					DO_KEY(SDLK_LEFTBRACKET, BACKEND_KEYBOARD_LEFT_BRACKET)
					DO_KEY(SDLK_RIGHTBRACKET, BACKEND_KEYBOARD_RIGHT_BRACKET)
					DO_KEY(SDLK_BACKSLASH, BACKEND_KEYBOARD_BACK_SLASH)
					DO_KEY(SDLK_SEMICOLON, BACKEND_KEYBOARD_SEMICOLON)
					DO_KEY(SDLK_QUOTE, BACKEND_KEYBOARD_APOSTROPHE)
					DO_KEY(SDLK_COMMA, BACKEND_KEYBOARD_COMMA)
					DO_KEY(SDLK_PERIOD, BACKEND_KEYBOARD_PERIOD)
					DO_KEY(SDLK_SLASH, BACKEND_KEYBOARD_FORWARD_SLASH)

					default:
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
