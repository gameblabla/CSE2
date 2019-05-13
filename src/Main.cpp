#include "Main.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>

#include "lodepng/lodepng.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "Game.h"
#include "Generic.h"
#include "Input.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "Organya.h"
#ifdef EXTRA_SOUND_FORMATS
#include "ExtraSoundFormats.h"
#endif
#include "Profile.h"
#include "Resource.h"
#include "Sound.h"
#include "Triangle.h"
#include "Types.h"

// These two are defined in Draw.cpp. This is a bit of a hack.
extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

char gModulePath[PATH_LENGTH];
char gDataPath[PATH_LENGTH];

int gJoystickButtonTable[8];

int ghWnd;	// Placeholder until we restore the WinAPI code
bool gbUseJoystick = false;
bool bFps = false;

bool bActive = true;

#ifdef JAPANESE
const char *lpWindowName = "洞窟物語エンジン2";
#else
const char *lpWindowName = "Cave Story Engine 2 ~ Doukutsu Monogatari Enjin 2";
#endif

// A replication of MSVC's rand algorithm
static unsigned long int next = 1;

int rep_rand()
{
	next = ((next) * 214013 + 2531011);
	return ((next) >> 16) & 0x7FFF;
}

void rep_srand(unsigned int seed)
{
	next = seed;
}

// Framerate stuff
void PutFramePerSecound()
{
	if (bFps)
		PutNumber4(WINDOW_WIDTH - 40, 8, GetFramePerSecound(), false);
}

int GetFramePerSecound()
{
	unsigned int current_tick;
	static bool need_new_base_tick = true;
	static int frames_this_second;
	static int current_frame;
	static int base_tick;

	if (need_new_base_tick)
	{
		base_tick = SDL_GetTicks();
		need_new_base_tick = false;
	}

	current_tick = SDL_GetTicks();
	++current_frame;

	if (base_tick + 1000 <= current_tick)
	{
		base_tick += 1000;
		frames_this_second = current_frame;
		current_frame = 0;
	}

	return frames_this_second;
}

int main(int argc, char *argv[])
{
	// Get executable's path
	strcpy(gModulePath, SDL_GetBasePath());
	if (gModulePath[strlen(gModulePath) - 1] == '/' || gModulePath[strlen(gModulePath) - 1] == '\\')
		gModulePath[strlen(gModulePath) - 1] = '\0'; // String cannot end in slash or stuff will probably break (original does this through a windows.h provided function)

	// Get path of the data folder
	strcpy(gDataPath, gModulePath);
	strcat(gDataPath, "/data");

#ifdef WINDOWS
	// Set the window icons. See res/ICON/ICON.rc.
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "101");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "102");
#endif

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) >= 0)
	{
		// Load configuration
		CONFIG config;

		if (!LoadConfigData(&config))
			DefaultConfigData(&config);

		// Apply keybinds
		// Swap X and Z buttons
		if (config.attack_button_mode)
		{
			if (config.attack_button_mode == 1)
			{
				gKeyJump = KEY_X;
				gKeyShot = KEY_Z;
			}
		}
		else
		{
			gKeyJump = KEY_Z;
			gKeyShot = KEY_X;
		}

		// Swap Okay and Cancel buttons
		if (config.ok_button_mode)
		{
			if (config.ok_button_mode == 1)
			{
				gKeyOk = gKeyShot;
				gKeyCancel = gKeyJump;
			}
		}
		else
		{
			gKeyOk = gKeyJump;
			gKeyCancel = gKeyShot;
		}

		// Swap left and right weapon switch keys
		if (CheckFileExists("s_reverse"))
		{
			gKeyArms = KEY_ARMSREV;
			gKeyArmsRev = KEY_ARMS;
		}

		// Alternate movement keys
		if (config.move_button_mode)
		{
			if (config.move_button_mode == 1)
			{
				gKeyLeft = KEY_ALT_LEFT;
				gKeyUp = KEY_ALT_UP;
				gKeyRight = KEY_ALT_RIGHT;
				gKeyDown = KEY_ALT_DOWN;
			}
		}
		else
		{
			gKeyLeft = KEY_LEFT;
			gKeyUp = KEY_UP;
			gKeyRight = KEY_RIGHT;
			gKeyDown = KEY_DOWN;
		}

		// Set gamepad inputs
		for (int i = 0; i < 8; i++)
		{
			switch (config.joystick_button[i])
			{
				case 1:
					gJoystickButtonTable[i] = gKeyJump;
					break;

				case 2:
					gJoystickButtonTable[i] = gKeyShot;
					break;

				case 3:
					gJoystickButtonTable[i] = gKeyArms;
					break;

				case 6:
					gJoystickButtonTable[i] = gKeyArmsRev;
					break;

				case 4:
					gJoystickButtonTable[i] = gKeyItem;
					break;

				case 5:
					gJoystickButtonTable[i] = gKeyMap;
					break;

				default:
					continue;
			}
		}

		RECT unused_rect = {0, 0, 320, 240};

		// Load cursor
		char path[PATH_LENGTH];
		sprintf(path, "%s/Resource/CURSOR/CURSOR_NORMAL.png", gDataPath);
		unsigned int bitmap_width, bitmap_height;
		unsigned char *bitmap_pixels;
		lodepng_decode32_file(&bitmap_pixels, &bitmap_width, &bitmap_height, path);

		if (bitmap_pixels)
		{
			SDL_Surface *cursor_surface = SDL_CreateRGBSurfaceWithFormatFrom(bitmap_pixels, bitmap_width, bitmap_height, 0, bitmap_width * 4, SDL_PIXELFORMAT_RGBA32);
			SDL_Cursor *cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);	// Don't worry, the hotspots are accurate to the original files

			if (cursor)
				SDL_SetCursor(cursor);
			else
				printf("Failed to load cursor\n");

			SDL_FreeSurface(cursor_surface);
			free(bitmap_pixels);
		}
		else
		{
			printf("Failed to load cursor\n");
		}

		// Get window dimensions and colour depth
		int windowWidth;
		int windowHeight;
		int colourDepth;

		switch (config.display_mode)
		{
			case 1:
			case 2:
				// Set window dimensions
				if (config.display_mode == 1)
				{
					windowWidth = WINDOW_WIDTH;
					windowHeight = WINDOW_HEIGHT;
				}
				else
				{
					windowWidth = WINDOW_WIDTH * 2;
					windowHeight = WINDOW_HEIGHT * 2;
				}

				// Create window
				gWindow = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);

				if (gWindow)
				{
					if (config.display_mode == 1)
						StartDirectDraw(0, 0);
					else
						StartDirectDraw(1, 0);
					break;
				}

				break;

			case 0:
			case 3:
			case 4:
				// Set window dimensions
				windowWidth = WINDOW_WIDTH * 2;
				windowHeight = WINDOW_HEIGHT * 2;

				// Create window
				gWindow = SDL_CreateWindow(lpWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);

				if (gWindow)
				{
					// Set colour depth
					switch (config.display_mode)
					{
						case 0:
							colourDepth = 16;
							break;
						case 3:
							colourDepth = 24;
							break;
						case 4:
							colourDepth = 32;
							break;
					}

					StartDirectDraw(2, colourDepth);

					fullscreen = true;
					SDL_ShowCursor(0);
					break;
				}
				break;
		}

		// Create window


		if (gWindow)
		{
			// Check debug things
			if (CheckFileExists("fps"))
				bFps = true;

#ifndef WINDOWS
			// Load icon
			char path[PATH_LENGTH];
			sprintf(path, "%s/Resource/ICON/ICON_MINI.png", gDataPath);
			unsigned int bitmap_width, bitmap_height;
			unsigned char *bitmap_pixels;
			lodepng_decode32_file(&bitmap_pixels, &bitmap_width, &bitmap_height, path);

			if (bitmap_pixels)
			{
				SDL_Surface *iconSurf = SDL_CreateRGBSurfaceWithFormatFrom(bitmap_pixels, bitmap_width, bitmap_height, 0, bitmap_width * 4, SDL_PIXELFORMAT_RGBA32);
				SDL_Surface *iconConverted = SDL_ConvertSurfaceFormat(iconSurf, SDL_PIXELFORMAT_RGB888, 0);
				SDL_FreeSurface(iconSurf);
				free(bitmap_pixels);
				SDL_Surface *iconSurfUpscaled = SDL_CreateRGBSurfaceWithFormat(0, 256, 256, 0, SDL_PIXELFORMAT_RGB888);
				SDL_LowerBlitScaled(iconConverted, NULL, iconSurfUpscaled, NULL);
				SDL_FreeSurface(iconConverted);
				SDL_SetWindowIcon(gWindow, iconSurfUpscaled);
				SDL_FreeSurface(iconSurfUpscaled);
			}
			else
			{
				printf("Failed to load icon\n");
			}
#endif

			// Set rects
			RECT loading_rect = {0, 0, 64, 8};
			RECT clip_rect = {0, 0, windowWidth, windowHeight};

			// Load the "LOADING" text
			MakeSurface_File("Loading", SURFACE_ID_LOADING);

			// Draw loading screen
			CortBox(&clip_rect, 0x000000);
			PutBitmap3(&clip_rect, PixelToScreenCoord((WINDOW_WIDTH - 64) / 2), PixelToScreenCoord((WINDOW_HEIGHT - 8) / 2), &loading_rect, SURFACE_ID_LOADING);

			// Draw to screen
			if (Flip_SystemTask(ghWnd))
			{
				// Initialize sound
				InitDirectSound();

				// Initialize joystick
				if (config.bJoystick && InitDirectInput())
				{
					ResetJoystickStatus();
					gbUseJoystick = true;
				}

				// Initialize stuff
				InitTextObject(config.font_name);
				InitTriangleTable();

				// Run game code
				Game();

				// End stuff
				EndDirectSound();
				EndTextObject();
				EndDirectDraw();
			}
		}
	}
	else
	{
		SDL_Quit();
		return -1;
	}

	SDL_Quit();
	return 0;
}

void InactiveWindow()
{
	if (bActive)
	{
		bActive = false;
		StopOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Stop();
#endif
		SleepNoise();
	}

	PlaySoundObject(7, 0);
}

void ActiveWindow()
{
	if (!bActive)
	{
		bActive = true;
		StopOrganyaMusic();
		PlayOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Play();
#endif
		ResetNoise();
	}

	PlaySoundObject(7, -1);
}

void JoystickProc()
{
	JOYSTICK_STATUS status;

	if (GetJoystickStatus(&status))
	{
		// Clear held buttons
		gKey &= (KEY_ESCAPE | KEY_F2 | KEY_F1);

		// Set movement buttons
		if (status.bLeft)
			gKey |= gKeyLeft;
		if (status.bRight)
			gKey |= gKeyRight;
		if (status.bUp)
			gKey |= gKeyUp;
		if (status.bDown)
			gKey |= gKeyDown;

		// Set held buttons
		for (int i = 0; i < 8; i++)
		{
			if (status.bButton[i])
				gKey |= gJoystickButtonTable[i];
		}
	}
}

#define DO_KEY_PRESS(key) \
	if (event.type == SDL_KEYDOWN) \
		gKey |= key; \
	else \
		gKey &= ~key; \
	break;

bool SystemTask()
{
	// Handle window events
	bool focusGained = true;

	while (SDL_PollEvent(NULL) || !focusGained)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				return false;
				break;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focusGained = true;
						ActiveWindow();
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
						focusGained = false;
						InactiveWindow();
						break;

					default:
						break;
				}
				break;

			case SDL_DROPFILE:
				LoadProfile(event.drop.file);
				SDL_free(event.drop.file);
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
#ifdef FIX_BUGS
				// BUG FIX: Pixel relied on key codes for input, but these differ based on keyboard layout.
				// This would break the alternate movement keys on typical English keyboards, since the '=' key is in a completely different place to where it is on a Japanese keyboard.
				// To solve this, we use scancodes instead, which are based on the physical location of keys, rather than their meaning.
				switch (event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						DO_KEY_PRESS(KEY_ESCAPE)

					case SDL_SCANCODE_W:
						DO_KEY_PRESS(KEY_MAP)

					case SDL_SCANCODE_LEFT:
						DO_KEY_PRESS(KEY_LEFT)

					case SDL_SCANCODE_RIGHT:
						DO_KEY_PRESS(KEY_RIGHT)

					case SDL_SCANCODE_UP:
						DO_KEY_PRESS(KEY_UP)

					case SDL_SCANCODE_DOWN:
						DO_KEY_PRESS(KEY_DOWN)

					case SDL_SCANCODE_X:
						DO_KEY_PRESS(KEY_X)

					case SDL_SCANCODE_Z:
						DO_KEY_PRESS(KEY_Z)

					case SDL_SCANCODE_S:
						DO_KEY_PRESS(KEY_ARMS)

					case SDL_SCANCODE_A:
						DO_KEY_PRESS(KEY_ARMSREV)

					case SDL_SCANCODE_RSHIFT:
					case SDL_SCANCODE_LSHIFT:
						DO_KEY_PRESS(KEY_SHIFT)

					case SDL_SCANCODE_F1:
						DO_KEY_PRESS(KEY_F1)

					case SDL_SCANCODE_F2:
						DO_KEY_PRESS(KEY_F2)

					case SDL_SCANCODE_Q:
						DO_KEY_PRESS(KEY_ITEM)

					case SDL_SCANCODE_COMMA:
						DO_KEY_PRESS(KEY_ALT_LEFT)

					case SDL_SCANCODE_PERIOD:
						DO_KEY_PRESS(KEY_ALT_DOWN)

					case SDL_SCANCODE_SLASH:
						DO_KEY_PRESS(KEY_ALT_RIGHT)

					case SDL_SCANCODE_L:
						DO_KEY_PRESS(KEY_ALT_UP)

					case SDL_SCANCODE_SEMICOLON:
						DO_KEY_PRESS(KEY_PLUS)

					case SDL_SCANCODE_F5:
						gbUseJoystick = false;
						break;

					default:
						break;
				}
				break;
#else
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						DO_KEY_PRESS(KEY_ESCAPE)

					case SDLK_w:
						DO_KEY_PRESS(KEY_MAP)

					case SDLK_LEFT:
						DO_KEY_PRESS(KEY_LEFT)

					case SDLK_RIGHT:
						DO_KEY_PRESS(KEY_RIGHT)

					case SDLK_UP:
						DO_KEY_PRESS(KEY_UP)

					case SDLK_DOWN:
						DO_KEY_PRESS(KEY_DOWN)

					case SDLK_x:
						DO_KEY_PRESS(KEY_X)

					case SDLK_z:
						DO_KEY_PRESS(KEY_Z)

					case SDLK_s:
						DO_KEY_PRESS(KEY_ARMS)

					case SDLK_a:
						DO_KEY_PRESS(KEY_ARMSREV)

					case SDLK_RSHIFT:
					case SDLK_LSHIFT:
						DO_KEY_PRESS(KEY_SHIFT)

					case SDLK_F1:
						DO_KEY_PRESS(KEY_F1)

					case SDLK_F2:
						DO_KEY_PRESS(KEY_F2)

					case SDLK_q:
						DO_KEY_PRESS(KEY_ITEM)

					case SDLK_COMMA:
						DO_KEY_PRESS(KEY_ALT_LEFT)

					case SDLK_PERIOD:
						DO_KEY_PRESS(KEY_ALT_DOWN)

					case SDLK_SLASH:
						DO_KEY_PRESS(KEY_ALT_RIGHT)

					case SDLK_l:
						DO_KEY_PRESS(KEY_ALT_UP)

					case SDLK_SEMICOLON:
						DO_KEY_PRESS(KEY_PLUS)

					case SDLK_F5:
						gbUseJoystick = false;
						break;
				}
				break;
#endif
		}
	}

	// Run joystick code
	if (gbUseJoystick)
		JoystickProc();

	return true;
}
