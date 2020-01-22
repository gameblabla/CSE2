#include "Main.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

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
#include "Profile.h"
#include "Resource.h"
#include "Sound.h"
#include "Triangle.h"

char gModulePath[MAX_PATH];
char gDataPath[MAX_PATH];

BOOL bFullscreen;
BOOL gbUseJoystick = FALSE;

int gJoystickButtonTable[8];

static BOOL bActive = TRUE;
static BOOL bFps = FALSE;

static int windowWidth;
static int windowHeight;

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

// TODO - Inaccurate stack frame
int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	int i;

	SDL_Init(SDL_INIT_EVENTS);

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

#ifdef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "1");
#endif

	SDL_InitSubSystem(SDL_INIT_VIDEO);

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

		#ifdef FIX_BUGS
			if (conf.display_mode == 1)
			{
				if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 0))
					return 0;
			}
			else
			{
				if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 1))
					return 0;
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

		#ifdef FIX_BUGS
			if (!StartDirectDraw(lpWindowName, windowWidth, windowHeight, 2))
				return 0;
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, windowWidth, windowHeight, 2);
		#endif

			bFullscreen = TRUE;

			SDL_ShowCursor(SDL_DISABLE);
			break;
	}

#ifdef DEBUG_SAVE
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
#endif

	// Set up the cursor
	size_t resource_size;
	const unsigned char *resource_data = FindResource("CURSOR_NORMAL", "CURSOR", &resource_size);
	SDL_RWops *rwops = SDL_RWFromConstMem(resource_data, resource_size);
	SDL_Surface *cursor_surface = SDL_LoadBMP_RW(rwops, 1);
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
	PutBitmap3(&rcFull, (WINDOW_WIDTH / 2) - 32, (WINDOW_HEIGHT / 2) - 4, &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask())
	{
        SDL_FreeCursor(cursor);
        SDL_FreeSurface(cursor_surface);
		return 1;
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

	SDL_FreeCursor(cursor);
	SDL_FreeSurface(cursor_surface);

	return 1;
}

void InactiveWindow(void)
{
	if (bActive)
	{
		bActive = FALSE;
		StopOrganyaMusic();
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
	for (i = 0; i < 8; ++i)
		gKey &= ~gJoystickButtonTable[i];

	// Set held buttons
	for (i = 0; i < 8; ++i)
		if (status.bButton[i])
			gKey |= gJoystickButtonTable[i];
}
