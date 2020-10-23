// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include "Main.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "WindowsWrapper.h"

#include "Backends/Misc.h"
#include "Bitmap.h"
#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "Game.h"
#include "Generic.h"
#include "Input.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "Organya.h"
#include "Profile.h"
#include "Resource.h"
#include "Sound.h"
#include "Triangle.h"

void InactiveWindow(void);
void ActiveWindow(void);

std::string gModulePath;
std::string gDataPath;

BOOL bFullscreen;
BOOL gbUseJoystick = FALSE;

int gJoystickButtonTable[8];

static BOOL bActive = TRUE;
static BOOL bFPS = FALSE;

static int windowWidth;
static int windowHeight;

#ifdef JAPANESE
static const char* const lpWindowName = "洞窟物語";	// "Cave Story"
#else
static const char* const lpWindowName = "Cave Story ~ Doukutsu Monogatari";
#endif

static void DragAndDropCallback(const char *path)
{
	LoadProfile(path);
}

static void WindowFocusCallback(bool focus)
{
	if (focus)
		ActiveWindow();
	else
		InactiveWindow();
}

// Framerate stuff
static unsigned long CountFramePerSecound(void)
{
	unsigned long current_tick;	// The original name for this variable is unknown
	static BOOL first = TRUE;
	static unsigned long max_count;
	static unsigned long count;
	static unsigned long wait;

	if (first)
	{
		wait = Backend_GetTicks();
		first = FALSE;
	}

	current_tick = Backend_GetTicks();
	++count;

	if (wait + 1000 <= current_tick)
	{
		wait += 1000;
		max_count = count;
		count = 0;
	}

	return max_count;
}

void PutFramePerSecound(void)
{
	if (bFPS)
	{
		const unsigned long fps = CountFramePerSecound();
		PutNumber4(WINDOW_WIDTH - 40, 8, fps, FALSE);
	}
}

// TODO - Inaccurate stack frame
int main(int argc, char *argv[])
{
	(void)argc;

	int i;

	if (!Backend_Init(DragAndDropCallback, WindowFocusCallback))
		return EXIT_FAILURE;

	// Get executable's path, and path of the data folder
	if (!Backend_GetPaths(&gModulePath, &gDataPath))
	{
		// Fall back on argv[0] if the backend cannot provide a path
		gModulePath = argv[0];

		for (size_t i = gModulePath.length();; --i)
		{
			if (i == 0 || gModulePath[i] == '\\' || gModulePath[i] == '/')
			{
				gModulePath.resize(i);
				break;
			}
		}

		gDataPath = gModulePath + "/data";
	}

	CONFIGDATA conf;
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
	if (IsKeyFile("s_reverse"))
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
	for (i = 0; i < 8; ++i)
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

	switch (conf.display_mode)
	{
		case 1:
		case 2:
			// Set window dimensions
			if (conf.display_mode == 1)
			{
				windowWidth = WINDOW_WIDTH;
				windowHeight = WINDOW_HEIGHT;
			}
			else
			{
				windowWidth = WINDOW_WIDTH * 2;
				windowHeight = WINDOW_HEIGHT * 2;
			}

		#ifdef FIX_MAJOR_BUGS
			if (conf.display_mode == 1)
			{
				if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 0))
				{
					Backend_Deinit();
					return EXIT_FAILURE;
				}
			}
			else
			{
				if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 1))
				{
					Backend_Deinit();
					return EXIT_FAILURE;
				}
			}
		#else
			// Doesn't handle StartDirectDraw failing
			if (conf.display_mode == 1)
				StartDirectDraw(lpWindowName, windowWidth, windowHeight, 0);
			else
				StartDirectDraw(lpWindowName, windowWidth, windowHeight, 1);
		#endif

			break;

		case 0:
		case 3:
		case 4:
			// Set window dimensions
			windowWidth = WINDOW_WIDTH * 2;
			windowHeight = WINDOW_HEIGHT * 2;

		#ifdef FIX_MAJOR_BUGS
			if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 2))
			{
				Backend_Deinit();
				return EXIT_FAILURE;
			}
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, windowWidth, windowHeight, 2);
		#endif

			bFullscreen = TRUE;

			Backend_HideMouse();
			break;
	}

#ifdef DEBUG_SAVE
	Backend_EnableDragAndDrop();
#endif

	// Set up window icon
#ifndef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	size_t window_icon_resource_size;
	const unsigned char *window_icon_resource_data = FindResource("ICON_MINI", "ICON", &window_icon_resource_size);

	if (window_icon_resource_data != NULL)
	{
		size_t window_icon_width, window_icon_height;
		unsigned char *window_icon_rgb_pixels = DecodeBitmap(window_icon_resource_data, window_icon_resource_size, &window_icon_width, &window_icon_height, 3);

		if (window_icon_rgb_pixels != NULL)
		{
			Backend_SetWindowIcon(window_icon_rgb_pixels, window_icon_width, window_icon_height);
			FreeBitmap(window_icon_rgb_pixels);
		}
	}
#endif

	// Set up the cursor
	size_t cursor_resource_size;
	const unsigned char *cursor_resource_data = FindResource("CURSOR_NORMAL", "CURSOR", &cursor_resource_size);

	if (cursor_resource_data != NULL)
	{
		size_t cursor_width, cursor_height;
		unsigned char *cursor_rgba_pixels = DecodeBitmap(cursor_resource_data, cursor_resource_size, &cursor_width, &cursor_height, 4);

		if (cursor_rgba_pixels != NULL)
		{
			Backend_SetCursor(cursor_rgba_pixels, cursor_width, cursor_height);
			FreeBitmap(cursor_rgba_pixels);
		}
	}

	if (IsKeyFile("fps"))
		bFPS = TRUE;

	// Set rects
	RECT rcLoading = {0, 0, 64, 8};
	RECT rcFull = {0, 0, 0, 0};
	rcFull.right = WINDOW_WIDTH;
	rcFull.bottom = WINDOW_HEIGHT;

	// Load the "LOADING" text
	BOOL b = MakeSurface_File("Loading", SURFACE_ID_LOADING);

	// Draw loading screen
	CortBox(&rcFull, 0x000000);
	PutBitmap3(&rcFull, (WINDOW_WIDTH / 2) - 32, (WINDOW_HEIGHT / 2) - 4, &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask())
	{
		Backend_Deinit();
		return EXIT_SUCCESS;
	}

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
		SleepNoise();
	}

	PlaySoundObject(7, SOUND_MODE_STOP);
}

void ActiveWindow(void)
{
	if (!bActive)
	{
		bActive = TRUE;
		StopOrganyaMusic();
		PlayOrganyaMusic();
		ResetNoise();
	}

	PlaySoundObject(7, SOUND_MODE_PLAY_LOOP);
}

void JoystickProc(void);

BOOL SystemTask(void)
{
	static bool previous_keyboard_state[BACKEND_KEYBOARD_TOTAL];

	do
	{
		if (!Backend_SystemTask(bActive))
		{
			StopOrganyaMusic();
			return FALSE;
		}
	} while(!bActive);

	bool keyboard_state[BACKEND_KEYBOARD_TOTAL];
	Backend_GetKeyboardState(keyboard_state);

	for (unsigned int i = 0; i < BACKEND_KEYBOARD_TOTAL; ++i)
	{
		if (keyboard_state[i] && !previous_keyboard_state[i])
		{
			switch (i)
			{
				case BACKEND_KEYBOARD_ESCAPE:
					gKey |= KEY_ESCAPE;
					break;

				case BACKEND_KEYBOARD_W:
					gKey |= KEY_MAP;
					break;

				case BACKEND_KEYBOARD_LEFT:
					gKey |= KEY_LEFT;
					break;

				case BACKEND_KEYBOARD_RIGHT:
					gKey |= KEY_RIGHT;
					break;

				case BACKEND_KEYBOARD_UP:
					gKey |= KEY_UP;
					break;

				case BACKEND_KEYBOARD_DOWN:
					gKey |= KEY_DOWN;
					break;

				case BACKEND_KEYBOARD_X:
					gKey |= KEY_X;
					break;

				case BACKEND_KEYBOARD_Z:
					gKey |= KEY_Z;
					break;

				case BACKEND_KEYBOARD_S:
					gKey |= KEY_ARMS;
					break;

				case BACKEND_KEYBOARD_A:
					gKey |= KEY_ARMSREV;
					break;

				case BACKEND_KEYBOARD_LEFT_SHIFT:
				case BACKEND_KEYBOARD_RIGHT_SHIFT:
					gKey |= KEY_SHIFT;
					break;

				case BACKEND_KEYBOARD_F1:
					gKey |= KEY_F1;
					break;

				case BACKEND_KEYBOARD_F2:
					gKey |= KEY_F2;
					break;

				case BACKEND_KEYBOARD_Q:
					gKey |= KEY_ITEM;
					break;

				case BACKEND_KEYBOARD_COMMA:
					gKey |= KEY_ALT_LEFT;
					break;

				case BACKEND_KEYBOARD_PERIOD:
					gKey |= KEY_ALT_DOWN;
					break;

				case BACKEND_KEYBOARD_FORWARD_SLASH:
					gKey |= KEY_ALT_RIGHT;
					break;

				case BACKEND_KEYBOARD_L:
					gKey |= KEY_L;
					break;

				case BACKEND_KEYBOARD_EQUALS:
					gKey |= KEY_PLUS;
					break;

				case BACKEND_KEYBOARD_F5:
					gbUseJoystick = FALSE;
					break;
			}
		}
		else if (!keyboard_state[i] && previous_keyboard_state[i])
		{
			switch (i)
			{
				case BACKEND_KEYBOARD_ESCAPE:
					gKey &= ~KEY_ESCAPE;
					break;

				case BACKEND_KEYBOARD_W:
					gKey &= ~KEY_MAP;
					break;

				case BACKEND_KEYBOARD_LEFT:
					gKey &= ~KEY_LEFT;
					break;

				case BACKEND_KEYBOARD_RIGHT:
					gKey &= ~KEY_RIGHT;
					break;

				case BACKEND_KEYBOARD_UP:
					gKey &= ~KEY_UP;
					break;

				case BACKEND_KEYBOARD_DOWN:
					gKey &= ~KEY_DOWN;
					break;

				case BACKEND_KEYBOARD_X:
					gKey &= ~KEY_X;
					break;

				case BACKEND_KEYBOARD_Z:
					gKey &= ~KEY_Z;
					break;

				case BACKEND_KEYBOARD_S:
					gKey &= ~KEY_ARMS;
					break;

				case BACKEND_KEYBOARD_A:
					gKey &= ~KEY_ARMSREV;
					break;

				case BACKEND_KEYBOARD_LEFT_SHIFT:
				case BACKEND_KEYBOARD_RIGHT_SHIFT:
					gKey &= ~KEY_SHIFT;
					break;

				case BACKEND_KEYBOARD_F1:
					gKey &= ~KEY_F1;
					break;

				case BACKEND_KEYBOARD_F2:
					gKey &= ~KEY_F2;
					break;

				case BACKEND_KEYBOARD_Q:
					gKey &= ~KEY_ITEM;
					break;

				case BACKEND_KEYBOARD_COMMA:
					gKey &= ~KEY_ALT_LEFT;
					break;

				case BACKEND_KEYBOARD_PERIOD:
					gKey &= ~KEY_ALT_DOWN;
					break;

				case BACKEND_KEYBOARD_FORWARD_SLASH:
					gKey &= ~KEY_ALT_RIGHT;
					break;

				case BACKEND_KEYBOARD_L:
					gKey &= ~KEY_L;
					break;

				case BACKEND_KEYBOARD_EQUALS:
					gKey &= ~KEY_PLUS;
					break;
			}
		}
	}

	memcpy(previous_keyboard_state, keyboard_state, sizeof(keyboard_state));

	// Run joystick code
	if (gbUseJoystick)
		JoystickProc();

	return TRUE;
}

void JoystickProc(void)
{
	int i;
	DIRECTINPUTSTATUS status;

	if (!GetJoystickStatus(&status))
		return;

	gKey &= (KEY_ESCAPE | KEY_F1 | KEY_F2);

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
	for (i = 0; i < 8; ++i)
		gKey &= ~gJoystickButtonTable[i];

	// Set held buttons
	for (i = 0; i < 8; ++i)
		if (status.bButton[i])
			gKey |= gJoystickButtonTable[i];
}
