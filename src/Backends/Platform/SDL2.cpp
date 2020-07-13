#include "../Misc.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "SDL.h"

#include "../Rendering.h"
#include "../Shared/SDL2.h"
#include "../../Attributes.h"
#include "../../Main.h"
#include "../../Organya.h"
#include "../../Profile.h"

#define DO_KEY(SDL_KEY, BACKEND_KEY) \
	case SDL_KEY: \
		keyboard_state[BACKEND_KEY] = event.key.type == SDL_KEYDOWN; \
		break;

static bool keyboard_state[BACKEND_KEYBOARD_TOTAL];

static unsigned char *cursor_surface_pixels;
static SDL_Surface *cursor_surface;
static SDL_Cursor *cursor;

bool Backend_Init(void)
{
	if (SDL_Init(SDL_INIT_EVENTS) == 0)
	{
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0)
		{
			Backend_PrintInfo("Available SDL video drivers:");

			for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
				Backend_PrintInfo("%s", SDL_GetVideoDriver(i));

			const char *driver = SDL_GetCurrentVideoDriver();

			if (driver != NULL)
			{
				Backend_PrintInfo("Selected SDL video driver: %s", driver);

				return true;
			}
			else
			{
				Backend_PrintError("No SDL video driver initialized!");
			}
		}
		else
		{
			std::string error_message = std::string("Could not initialise SDL video subsystem: ") + SDL_GetError();
			Backend_ShowMessageBox("Fatal error", error_message.c_str());
		}

		SDL_Quit();
	}
	else
	{
		std::string error_message = std::string("Could not initialise SDL: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error", error_message.c_str());
	}

	return false;
}

void Backend_Deinit(void)
{
	if (cursor != NULL)
		SDL_FreeCursor(cursor);

	if (cursor_surface != NULL)
		SDL_FreeSurface(cursor_surface);

	free(cursor_surface_pixels);

	SDL_Quit();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(std::string *string_buffer)
{
#ifdef _WIN32
	// SDL_GetBasePath returns a UTF-8 string, but Windows' fopen uses (extended?) ASCII.
	// This is apparently a problem for accented characters, as they will make fopen fail.
	// So, instead, we rely on argv[0], as that will put the accented characters in a
	// format Windows will understand.
	(void)string_buffer;

	return false;
#else
	char *base_path = SDL_GetBasePath();
	if (base_path == NULL)
		return false;

	// Trim the trailing '/'
	size_t base_path_length = strlen(base_path);
	base_path[base_path_length - 1] = '\0';
	*string_buffer = base_path;
	SDL_free(base_path);

	return true;
#endif
}

void Backend_HideMouse(void)
{
	SDL_ShowCursor(SDL_DISABLE);
}

void Backend_SetWindowIcon(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)rgb_pixels, width, height, 0, width * 3, SDL_PIXELFORMAT_RGB24);

	if (surface != NULL)
	{
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
	}
	else
	{
		Backend_PrintError("Couldn't create RGB surface for window icon: %s", SDL_GetError());
	}
}

void Backend_SetCursor(const unsigned char *rgba_pixels, unsigned int width, unsigned int height)
{
	cursor_surface_pixels = (unsigned char*)malloc(width * height * 4);

	if (cursor_surface_pixels != NULL)
	{
		memcpy(cursor_surface_pixels, rgba_pixels, width * height * 4);

		cursor_surface = SDL_CreateRGBSurfaceWithFormatFrom(cursor_surface_pixels, width, height, 0, width * 4, SDL_PIXELFORMAT_RGBA32);

		if (cursor_surface != NULL)
		{
			cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);

			if (cursor != NULL)
				SDL_SetCursor(cursor);
		}
	}
	else
	{
		Backend_PrintError("Failed to allocate memory for cursor surface");
	}
}

void PlaybackBackend_EnableDragAndDrop(void)
{
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
}

bool Backend_SystemTask(bool active)
{
	if (!active)
		if (!SDL_WaitEvent(NULL))
			return false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
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

			case SDL_JOYDEVICEADDED:
				ControllerBackend_JoystickConnect(event.jdevice.which);
				break;

			case SDL_JOYDEVICEREMOVED:
				ControllerBackend_JoystickDisconnect(event.jdevice.which);
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
						RenderBackend_HandleWindowResize(event.window.data1, event.window.data2);
						break;
				}

				break;

			case SDL_QUIT:
				StopOrganyaMusic();
				return false;

			case SDL_RENDER_TARGETS_RESET:
				RenderBackend_HandleRenderTargetLoss();
				break;

		}
	}

	return true;
}

void Backend_GetKeyboardState(bool *out_keyboard_state)
{
	memcpy(out_keyboard_state, keyboard_state, sizeof(keyboard_state));
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	fprintf(stderr, "ShowMessageBox - '%s' - '%s'\n", title, message);

	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window) != 0)
		Backend_PrintError("Was also unable to display a message box containing the error: %s", SDL_GetError());
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char *format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("ERROR: ", stderr);
	vfprintf(stderr, format, argumentList);
	fputc('\n', stderr);
	va_end(argumentList);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char *format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("INFO: ", stdout);
	vprintf(format, argumentList);
	putchar('\n');
	va_end(argumentList);
}

unsigned long Backend_GetTicks(void)
{
	return SDL_GetTicks();
}

void Backend_Delay(unsigned int ticks)
{
	SDL_Delay(ticks);
}

void Backend_GetDisplayMode(Backend_DisplayMode *display_mode)
{
	SDL_DisplayMode sdl_display_mode;
	SDL_GetDesktopDisplayMode(0, &sdl_display_mode);

	display_mode->width = sdl_display_mode.w;
	display_mode->height = sdl_display_mode.h;
	display_mode->refresh_rate = sdl_display_mode.refresh_rate;
}

bool Backend_IsConsole(void)
{
	return false;
}
