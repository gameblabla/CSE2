#include "Main.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <shlwapi.h>

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
#include "Profile.h"
#include "Sound.h"
#include "Triangle.h"

char gModulePath[MAX_PATH];
char gDataPath[MAX_PATH];

int gJoystickButtonTable[8];

HWND ghWnd;
BOOL bFullscreen;

static BOOL gbUseJoystick = FALSE;
static BOOL bFps = FALSE;
static BOOL bActive = TRUE;

static HANDLE hObject;
static HANDLE hMutex;
static HINSTANCE ghInstance;

static int windowWidth;
static int windowHeight;

static const char *mutex_name = "Doukutsu";

#ifdef JAPANESE
static const char *lpWindowName = "\x93\xB4\x8C\x41\x95\xA8\x8C\xEA";
#else
static const char *lpWindowName = "Cave Story ~ Doukutsu Monogatari";
#endif

// Framerate stuff
void PutFramePerSecound()
{
	if (bFps)
		PutNumber4(WINDOW_WIDTH - 40, 8, GetFramePerSecound(), FALSE);
}

int GetFramePerSecound()
{
/*	unsigned int current_tick;
	static BOOL need_new_base_tick = TRUE;
	static int frames_this_second;
	static int current_frame;
	static int base_tick;

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

	return frames_this_second;*/
	return 0;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hObject = OpenMutexA(MUTEX_ALL_ACCESS, 0, mutex_name);
	if (hObject != NULL)
	{
		CloseHandle(hObject);
		return 0;
	}

	hMutex = CreateMutexA(NULL, FALSE, mutex_name);

	ghInstance = hInstance;

	// Get executable's path
	GetModuleFileNameA(NULL, gModulePath, MAX_PATH);
	PathRemoveFileSpecA(gModulePath);

	// Get path of the data folder
	strcpy(gDataPath, gModulePath);
	strcat(gDataPath, "\\data");

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
	for (int i = 0; i < 8; i++)
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

	WNDCLASSEXA wndclassex;
	memset(&wndclassex, 0, sizeof(WNDCLASSEXA));
	wndclassex.cbSize = sizeof(WNDCLASSEXA);
//	wndclassex.lpfnWndProc = WindowProcedure;
	wndclassex.hInstance = hInstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(3);
	wndclassex.lpszClassName = lpWindowName;
	wndclassex.hCursor = LoadCursorA(hInstance, "CURSOR_NORMAL");
	wndclassex.hIcon = LoadIconA(hInstance, "0");
	wndclassex.hIconSm = LoadIconA(hInstance, "ICON_MINI");

	HWND hWnd;

	switch (conf.display_mode)
	{
		case 1:
		case 2:
		{
			wndclassex.lpszMenuName = "MENU_MAIN";
			if (RegisterClassExA(&wndclassex) == 0)
			{
				ReleaseMutex(hMutex);
				return 0;
			}

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

			int nWidth = windowWidth + 2 * GetSystemMetrics(7) + 2;

			int nHeight = (2 * GetSystemMetrics(8) + GetSystemMetrics(4)) + GetSystemMetrics(15) + windowHeight + 2;
			int x = (GetSystemMetrics(0) - nWidth) / 2;
			int y = (GetSystemMetrics(1) - nHeight) / 2;
			SetWindowPadding(GetSystemMetrics(7) + 1, GetSystemMetrics(8) + GetSystemMetrics(4) + GetSystemMetrics(15) + 1);

			hWnd = CreateWindowExA(0, lpWindowName, lpWindowName, 0x10CA0000u, x, y, nWidth, nHeight, 0, 0, hInstance, 0);
			ghWnd = hWnd;

			if (hWnd == NULL)
			{
				ReleaseMutex(hMutex);
				return 0;
			}

			HMENU v18 = GetMenu(hWnd);

			if (conf.display_mode == 1)
				StartDirectDraw(hWnd, 0, 0);
			else
				StartDirectDraw(hWnd, 1, 0);

			break;
		}

		case 0:
		case 3:
		case 4:
		{
			if (RegisterClassExA(&wndclassex) == 0)
			{
				ReleaseMutex(hMutex);
				return 0;
			}

			// Set window dimensions
			windowWidth = WINDOW_WIDTH * 2;
			windowHeight = WINDOW_HEIGHT * 2;

			SetWindowPadding(0, 0);
			hWnd = CreateWindowExA(0, lpWindowName, lpWindowName, 0x90080000, 0, 0, GetSystemMetrics(0), GetSystemMetrics(1), 0, 0, hInstance, 0);
			ghWnd = hWnd;
			if (hWnd == NULL)
			{
				ReleaseMutex(hMutex);
				return 0;
			}

			// Set colour depth
			int depth;

			switch (conf.display_mode)
			{
				case 0:
					depth = 16;
					break;
				case 3:
					depth = 24;
					break;
				case 4:
					depth = 32;
					break;
			}

			StartDirectDraw(ghWnd, 2, depth);
			bFullscreen = TRUE;

			ShowCursor(0);
			break;
		}
	}

	// Set rects
	RECT rcLoading = {0, 0, 64, 8};
	RECT rcFull = {0, 0, 0, 0};
	rcFull.right = WINDOW_WIDTH;
	rcFull.bottom = WINDOW_HEIGHT;

	// Load the "LOADING" text
	BOOL b = MakeSurface_File("Loading", SURFACE_ID_LOADING);

	// Draw loading screen
	CortBox(&rcFull, 0x000000);
	PutBitmap3(&rcFull, (WINDOW_WIDTH - 64) / 2, (WINDOW_HEIGHT - 8) / 2, &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask(ghWnd))
	{
		ReleaseMutex(hMutex);
		return 1;
	}
	else
	{
		// Initialize sound
		InitDirectSound(hWnd);

		// Initialize joystick
		if (conf.bJoystick && InitDirectInput(hInstance, hWnd))
		{
			ResetJoystickStatus();
			gbUseJoystick = TRUE;
		}

		// Initialize stuff
		InitTextObject(conf.font_name);
		InitTriangleTable();

		// Run game code
		Game(hWnd);

		// End stuff
		EndDirectSound();
		EndTextObject();
		EndDirectDraw(hWnd);

		ReleaseMutex(hMutex);
	}

	return 1;
}

void InactiveWindow()
{
	if (bActive)
	{
		bActive = FALSE;
		StopOrganyaMusic();
		SleepNoise();
	}

	PlaySoundObject(7, 0);
}

void ActiveWindow()
{
	if (!bActive)
	{
		bActive = TRUE;
		StopOrganyaMusic();
		PlayOrganyaMusic();
		ResetNoise();
	}

	PlaySoundObject(7, -1);
}

void JoystickProc()
{
/*	JOYSTICK_STATUS status;

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
	}*/
}

#define DO_KEY_PRESS(key) \
	if (event.type == SDL_KEYDOWN) \
		gKey |= key; \
	else \
		gKey &= ~key; \
	break;

BOOL SystemTask()
{
	// Handle window events
	BOOL focusGained = TRUE;
/*
	while (SDL_PollEvent(NULL) || !focusGained)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);

		switch (event.type)
		{
			case SDL_QUIT:
				return FALSE;

			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focusGained = TRUE;
						ActiveWindow();
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:
						focusGained = FALSE;
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
						gbUseJoystick = FALSE;
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
						gbUseJoystick = FALSE;
						break;
				}
				break;
#endif
		}
	}
*/
	// Run joystick code
	if (gbUseJoystick)
		JoystickProc();

	return TRUE;
}
