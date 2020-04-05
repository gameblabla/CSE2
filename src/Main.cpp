#include "Main.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Backends/Misc.h"
#include "Backends/Rendering.h"
#include "Bitmap.h"
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

BOOL bFullscreen;

CONFIG_BINDING bindings[BINDING_TOTAL];

static BOOL bFps = FALSE;

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
		base_tick = Backend_GetTicks();
		need_new_base_tick = FALSE;
	}

	current_tick = Backend_GetTicks();
	++current_frame;

	if (base_tick + 1000 <= current_tick)
	{
		base_tick += 1000;
		frames_this_second = current_frame;
		current_frame = 0;
	}

	return frames_this_second;
}

// TODO - Inaccurate stack frame
int main(int argc, char *argv[])
{
	(void)argc;

	Backend_Init();

	// Get executable's path
	if (!Backend_GetBasePath(gModulePath))
	{
		// Fall back on argv[0] if the backend cannot provide a path
		strcpy(gModulePath, argv[0]);

		for (size_t i = strlen(gModulePath);; --i)
		{
			if (i == 0 || gModulePath[i] == '\\' || gModulePath[i] == '/')
			{
				gModulePath[i] = '\0';
				break;
			}
		}
	}

	// Get path of the data folder
	strcpy(gDataPath, gModulePath);
	strcat(gDataPath, "/data");

	CONFIG conf;
	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	memcpy(bindings, conf.bindings, sizeof(bindings));

	RECT unused_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	switch (conf.display_mode)
	{
		default:
			// Windowed

		#ifdef FIX_BUGS
			if (!StartDirectDraw(lpWindowName, conf.display_mode, conf.b60fps, conf.bSmoothScrolling, conf.bVsync))
			{
				Backend_Deinit();
				return EXIT_FAILURE;
			}
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, conf.display_mode, conf.b60fps, conf.bSmoothScrolling, conf.bVsync);
		#endif

			break;

		case 0:
			// Fullscreen

		#ifdef FIX_BUGS
			if (!StartDirectDraw(lpWindowName, 0, conf.b60fps, conf.bSmoothScrolling, conf.bVsync))
			{
				Backend_Deinit();
				return EXIT_FAILURE;
			}
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, 0, conf.b60fps, conf.bSmoothScrolling, conf.bVsync);
		#endif

			bFullscreen = TRUE;

			Backend_HideMouse();
			break;
	}

#ifdef DEBUG_SAVE
	PlaybackBackend_EnableDragAndDrop();
#endif

	// Set up window icon
#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	size_t window_icon_resource_size;
	const unsigned char *window_icon_resource_data = FindResource("ICON_MINI", "ICON", &window_icon_resource_size);

	if (window_icon_resource_data != NULL)
	{
		unsigned int window_icon_width, window_icon_height;
		unsigned char *window_icon_rgb_pixels = DecodeBitmap(window_icon_resource_data, window_icon_resource_size, &window_icon_width, &window_icon_height);

		if (window_icon_rgb_pixels != NULL)
		{
			Backend_SetWindowIcon(window_icon_rgb_pixels, window_icon_width, window_icon_height);
			FreeBitmap(window_icon_rgb_pixels);
		}
	}
#endif

	// Set up the cursor
	char cursor_path[MAX_PATH];
	sprintf(cursor_path, "%s/Resource/CURSOR/CURSOR_NORMAL.png", gDataPath);

	unsigned int cursor_width, cursor_height;
	unsigned char *cursor_rgba_pixels = DecodeBitmapWithAlphaFromFile(cursor_path, &cursor_width, &cursor_height, FALSE);

	if (cursor_rgba_pixels != NULL)
	{
		Backend_SetCursor(cursor_rgba_pixels, cursor_width, cursor_height);
		FreeBitmap(cursor_rgba_pixels);
	}

	if (IsKeyFile("fps"))
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
	PutBitmap3(&rcFull, PixelToScreenCoord((WINDOW_WIDTH / 2) - 32), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 4), &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask())
	{
		Backend_Deinit();
		return EXIT_SUCCESS;
	}

	// Initialize sound
	InitDirectSound();

	// Initialize joystick
	InitDirectInput();

	// Initialize stuff
	InitTextObject(conf.font_name);
	InitTriangleTable();

	// Run game code
	Game();

	// End stuff
	EndTextObject();
	EndDirectSound();
	EndDirectDraw();

	Backend_Deinit();

	return EXIT_SUCCESS;
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
	static BOOL previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];

	if (!Backend_SystemTask())
		return FALSE;

	Backend_GetKeyboardState(gKeyboardState);

	for (unsigned int i = 0; i < BACKEND_KEYBOARD_TOTAL; ++i)
	{
		if (gKeyboardState[i] && !previous_keyboard_state[i])
		{
			if (i == BACKEND_KEYBOARD_ESCAPE)
				gKey |= KEY_ESCAPE;
			else if (i == BACKEND_KEYBOARD_F1)
				gKey |= KEY_F1;
			else if (i == BACKEND_KEYBOARD_F2)
				gKey |= KEY_F2;

			if (i == bindings[BINDING_MAP].keyboard)
				gKey |= KEY_MAP;
			if (i == bindings[BINDING_LEFT].keyboard)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].keyboard)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_UP].keyboard)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_DOWN].keyboard)
				gKey |= KEY_DOWN;
			if (i == bindings[BINDING_SHOT].keyboard)
				gKey |= KEY_SHOT;
			if (i == bindings[BINDING_JUMP].keyboard)
				gKey |= KEY_JUMP;
			if (i == bindings[BINDING_ARMS].keyboard)
				gKey |= KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].keyboard)
				gKey |= KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].keyboard)
				gKey |= KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].keyboard)
				gKey |= KEY_CANCEL;
			if (i == bindings[BINDING_OK].keyboard)
				gKey |= KEY_OK;
			if (i == bindings[BINDING_PAUSE].keyboard)
				gKey |= KEY_PAUSE;
		}
		else if (!gKeyboardState[i] && previous_keyboard_state[i])
		{
			if (i == BACKEND_KEYBOARD_ESCAPE)
				gKey &= ~KEY_ESCAPE;
			else if (i == BACKEND_KEYBOARD_F1)
				gKey &= ~KEY_F1;
			else if (i == BACKEND_KEYBOARD_F2)
				gKey &= ~KEY_F2;

			if (i == bindings[BINDING_MAP].keyboard)
				gKey &= ~KEY_MAP;
			if (i == bindings[BINDING_LEFT].keyboard)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].keyboard)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_UP].keyboard)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_DOWN].keyboard)
				gKey &= ~KEY_DOWN;
			if (i == bindings[BINDING_SHOT].keyboard)
				gKey &= ~KEY_SHOT;
			if (i == bindings[BINDING_JUMP].keyboard)
				gKey &= ~KEY_JUMP;
			if (i == bindings[BINDING_ARMS].keyboard)
				gKey &= ~KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].keyboard)
				gKey &= ~KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].keyboard)
				gKey &= ~KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].keyboard)
				gKey &= ~KEY_CANCEL;
			if (i == bindings[BINDING_OK].keyboard)
				gKey &= ~KEY_OK;
			if (i == bindings[BINDING_PAUSE].keyboard)
				gKey &= ~KEY_PAUSE;
		}
	}

	memcpy(previous_keyboard_state, gKeyboardState, sizeof(gKeyboardState));

	// Run joystick code
	JoystickProc();

	return TRUE;
}

void JoystickProc(void)
{
	int i;
	static JOYSTICK_STATUS old_status;

	if (!GetJoystickStatus(&gJoystickState))
		memset(&gJoystickState, 0, sizeof(gJoystickState));

	// Set held buttons
	for (i = 0; i < sizeof(gJoystickState.bButton) / sizeof(gJoystickState.bButton[0]); ++i)
	{
		if (gJoystickState.bButton[i] && !old_status.bButton[i])
		{
			if (i == bindings[BINDING_MAP].controller)
				gKey |= KEY_MAP;
			if (i == bindings[BINDING_LEFT].controller)
				gKey |= KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].controller)
				gKey |= KEY_RIGHT;
			if (i == bindings[BINDING_UP].controller)
				gKey |= KEY_UP;
			if (i == bindings[BINDING_DOWN].controller)
				gKey |= KEY_DOWN;
			if (i == bindings[BINDING_SHOT].controller)
				gKey |= KEY_SHOT;
			if (i == bindings[BINDING_JUMP].controller)
				gKey |= KEY_JUMP;
			if (i == bindings[BINDING_ARMS].controller)
				gKey |= KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].controller)
				gKey |= KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].controller)
				gKey |= KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].controller)
				gKey |= KEY_CANCEL;
			if (i == bindings[BINDING_OK].controller)
				gKey |= KEY_OK;
			if (i == bindings[BINDING_PAUSE].controller)
				gKey |= KEY_PAUSE;
		}
		else if (!gJoystickState.bButton[i] && old_status.bButton[i])
		{
			if (i == bindings[BINDING_MAP].controller)
				gKey &= ~KEY_MAP;
			if (i == bindings[BINDING_LEFT].controller)
				gKey &= ~KEY_LEFT;
			if (i == bindings[BINDING_RIGHT].controller)
				gKey &= ~KEY_RIGHT;
			if (i == bindings[BINDING_UP].controller)
				gKey &= ~KEY_UP;
			if (i == bindings[BINDING_DOWN].controller)
				gKey &= ~KEY_DOWN;
			if (i == bindings[BINDING_SHOT].controller)
				gKey &= ~KEY_SHOT;
			if (i == bindings[BINDING_JUMP].controller)
				gKey &= ~KEY_JUMP;
			if (i == bindings[BINDING_ARMS].controller)
				gKey &= ~KEY_ARMS;
			if (i == bindings[BINDING_ARMSREV].controller)
				gKey &= ~KEY_ARMSREV;
			if (i == bindings[BINDING_ITEM].controller)
				gKey &= ~KEY_ITEM;
			if (i == bindings[BINDING_CANCEL].controller)
				gKey &= ~KEY_CANCEL;
			if (i == bindings[BINDING_OK].controller)
				gKey &= ~KEY_OK;
			if (i == bindings[BINDING_PAUSE].controller)
				gKey &= ~KEY_PAUSE;
		}
	}

	old_status = gJoystickState;
}
