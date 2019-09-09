#include "Main.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "lodepng/lodepng.h"

#include "WindowsWrapper.h"

#include "Backends/Rendering.h"
#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "File.h"
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

char gModulePath[MAX_PATH];
char gDataPath[MAX_PATH];

int gJoystickButtonTable[8];

BOOL bFullscreen;
BOOL gbUseJoystick = FALSE;

static BOOL bFps = FALSE;
static BOOL bActive = TRUE;

static int windowWidth;
static int windowHeight;

static const char *mutex_name = "Doukutsu";

#ifdef JAPANESE
static const char *lpWindowName = "洞窟物語";	// "Cave Story"
#else
static const char *lpWindowName = "Cave Story ~ Doukutsu Monogatari";
#endif

// Framerate stuff
void PutFramePerSecound(void)
{
	if (bFps)
	{
		const unsigned long fps = GetFramePerSecound();
		PutNumber4(WINDOW_WIDTH - 40, 8, fps, FALSE);
	}
}

unsigned long GetFramePerSecound(void)
{
	unsigned long current_tick;
	static BOOL need_new_base_tick = TRUE;
	static unsigned long frames_this_second;
	static unsigned long current_frame;
	static unsigned long base_tick;

	if (need_new_base_tick)
	{
		base_tick = SDL_GetTicks();
		need_new_base_tick = FALSE;
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
	(void)argc;
	(void)argv;

	int i;

	// Get executable's path
	char *base_path = SDL_GetBasePath();
	size_t base_path_length = strlen(base_path);
	base_path[base_path_length - 1] = '\0';
	strcpy(gModulePath, base_path);
	SDL_free(base_path);

	// Get path of the data folder
	strcpy(gDataPath, gModulePath);
	strcat(gDataPath, "/data");

	CONFIG conf;
	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	// Apply keybinds
	// Swap X and Z buttons
	switch (conf.attack_button_mode)
	{
		case 0:
			gKeyJump = KEY_Z;
			gKeyShot = KEY_X;
			break;

		case 1:
			gKeyJump = KEY_X;
			gKeyShot = KEY_Z;
			break;
	}

	// Swap Okay and Cancel buttons
	switch (conf.ok_button_mode)
	{
		case 0:
			gKeyOk = gKeyJump;
			gKeyCancel = gKeyShot;
			break;

		case 1:
			gKeyOk = gKeyShot;
			gKeyCancel = gKeyJump;
			break;
	}

	// Swap left and right weapon switch keys
	if (CheckFileExists("s_reverse"))
	{
		gKeyArms = KEY_ARMSREV;
		gKeyArmsRev = KEY_ARMS;
	}

	// Alternate movement keys
	switch (conf.move_button_mode)
	{
		case 0:
			gKeyLeft = KEY_LEFT;
			gKeyUp = KEY_UP;
			gKeyRight = KEY_RIGHT;
			gKeyDown = KEY_DOWN;
			break;

		case 1:
			gKeyLeft = KEY_ALT_LEFT;
			gKeyUp = KEY_ALT_UP;
			gKeyRight = KEY_ALT_RIGHT;
			gKeyDown = KEY_ALT_DOWN;
			break;
	}

	// Set gamepad inputs
	for (i = 0; i < 8; i++)
	{
		switch (conf.joystick_button[i])
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
		}
	}

	RECT unused_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	SDL_Init(SDL_INIT_EVENTS);

	SDL_Window *window;

	switch (conf.display_mode)
	{
		default:
			// Windowed
			windowWidth = WINDOW_WIDTH * conf.display_mode;
			windowHeight = WINDOW_HEIGHT * conf.display_mode;

			window = CreateWindow(lpWindowName, windowWidth, windowHeight);

			if (window == NULL)
				return 0;

			StartDirectDraw(window, conf.display_mode);

			break;

		case 0:
			// Fullscreen
			windowWidth = WINDOW_WIDTH * 2;
			windowHeight = WINDOW_HEIGHT * 2;

			window = CreateWindow(lpWindowName, windowWidth, windowHeight);

			if (window == NULL)
				return 0;

			StartDirectDraw(window, 0);
			bFullscreen = TRUE;

			SDL_ShowCursor(SDL_DISABLE);
			break;
	}

#ifdef DEBUG_SAVE
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
#endif

	// Set up the window icon and cursor
	unsigned char *image_buffer;
	unsigned int image_width;
	unsigned int image_height;
	char image_path[MAX_PATH];

	sprintf(image_path, "%s/Resource/ICON/ICON_MINI.png", gDataPath);
	lodepng_decode32_file(&image_buffer, &image_width, &image_height, image_path);
	SDL_Surface *icon_surface = SDL_CreateRGBSurfaceWithFormatFrom(image_buffer, image_width, image_height, 32, image_width * 4, SDL_PIXELFORMAT_RGBA32);
	SDL_SetWindowIcon(window, icon_surface);
	SDL_FreeSurface(icon_surface);
	free(image_buffer);

	sprintf(image_path, "%s/Resource/CURSOR/CURSOR_NORMAL.png", gDataPath);
	lodepng_decode32_file(&image_buffer, &image_width, &image_height, image_path);
	SDL_Surface *cursor_surface = SDL_CreateRGBSurfaceWithFormatFrom(image_buffer, image_width, image_height, 32, image_width * 4, SDL_PIXELFORMAT_RGBA32);
	SDL_SetColorKey(cursor_surface, SDL_TRUE, SDL_MapRGB(cursor_surface->format, 0xFF, 0, 0xFF));
	SDL_Cursor *cursor = SDL_CreateColorCursor(cursor_surface, 0, 0);
	SDL_SetCursor(cursor);

	if (CheckFileExists("fps"))
		bFps = TRUE;

	// Set rects
	RECT rcLoading = {0, 0, 64, 8};
	RECT rcFull = {0, 0, 0, 0};
	rcFull.right = WINDOW_WIDTH;
	rcFull.bottom = WINDOW_HEIGHT;

	// Load the "LOADING" text
	BOOL b = MakeSurface_File("Loading", SURFACE_ID_LOADING);

	// Draw loading screen
	CortBox(&rcFull, 0x000000);
	PutBitmap3(&rcFull, PixelToScreenCoord((WINDOW_WIDTH - 64) / 2), PixelToScreenCoord((WINDOW_HEIGHT - 8) / 2), &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask())
	{
        SDL_FreeCursor(cursor);
        SDL_FreeSurface(cursor_surface);
		free(image_buffer);
		SDL_DestroyWindow(window);
		return 1;
	}
	else
	{
		// Initialize sound
		InitDirectSound();

		// Initialize joystick
		if (conf.bJoystick && InitDirectInput())
		{
			ResetJoystickStatus();
			gbUseJoystick = TRUE;
		}

		// Initialize stuff
		InitTextObject(conf.font_name);
		InitTriangleTable();

		// Run game code
		Game();

		// End stuff
		EndDirectSound();
		EndTextObject();
		EndDirectDraw();

        SDL_FreeCursor(cursor);
        SDL_FreeSurface(cursor_surface);
		free(image_buffer);
		SDL_DestroyWindow(window);
	}

	return 1;
}

void InactiveWindow(void)
{
	if (bActive)
	{
		bActive = FALSE;
		StopOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Stop();
#endif
		SleepNoise();
	}

	PlaySoundObject(7, 0);
}

void ActiveWindow(void)
{
	if (!bActive)
	{
		bActive = TRUE;
		StopOrganyaMusic();
		PlayOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Play();
#endif
		ResetNoise();
	}

	PlaySoundObject(7, -1);
}

void JoystickProc(void);

BOOL SystemTask(void)
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
						Backend_HandleWindowResize();
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

	// Run joystick code
	if (gbUseJoystick)
		JoystickProc();

	return TRUE;
}

void JoystickProc(void)
{
	int i;
	JOYSTICK_STATUS status;

	if (!GetJoystickStatus(&status))
		return;

	gKey &= (KEY_ESCAPE | KEY_F2 | KEY_F1);

	// Set movement buttons
	if (status.bLeft)
		gKey |= gKeyLeft;
	else
		gKey &= ~gKeyLeft;

	if (status.bRight)
		gKey |= gKeyRight;
	else
		gKey &= ~gKeyRight;

	if (status.bUp)
		gKey |= gKeyUp;
	else
		gKey &= ~gKeyUp;

	if (status.bDown)
		gKey |= gKeyDown;
	else
		gKey &= ~gKeyDown;

	// Clear held buttons
	for (i = 0; i < 8; i++)
		gKey &= ~gJoystickButtonTable[i];

	// Set held buttons
	for (i = 0; i < 8; i++)
	{
		if (status.bButton[i])
			gKey |= gJoystickButtonTable[i];
	}
}
