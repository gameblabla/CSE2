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

int gScancodeOk = SDL_SCANCODE_Z;
int gScancodeCancel = SDL_SCANCODE_X;
int gScancodeJump = SDL_SCANCODE_Z;
int gScancodeShot = SDL_SCANCODE_X;
int gScancodeArms = SDL_SCANCODE_S;
int gScancodeArmsRev = SDL_SCANCODE_A;
int gScancodeItem = SDL_SCANCODE_Q;
int gScancodeMap = SDL_SCANCODE_W;
int gScancodeUp = SDL_SCANCODE_UP;
int gScancodeDown = SDL_SCANCODE_DOWN;
int gScancodeLeft = SDL_SCANCODE_LEFT;
int gScancodeRight = SDL_SCANCODE_RIGHT;
int gScancodePause = SDL_SCANCODE_ESCAPE;

static BOOL bFps = FALSE;
static BOOL bActive = TRUE;

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

	// Set key-bindings
	gScancodeUp = conf.key_bindings[0];
	gScancodeDown = conf.key_bindings[1];
	gScancodeLeft = conf.key_bindings[2];
	gScancodeRight = conf.key_bindings[3];
	gScancodeOk = conf.key_bindings[4];
	gScancodeCancel = conf.key_bindings[5];
	gScancodeJump = conf.key_bindings[6];
	gScancodeShot = conf.key_bindings[7];
	gScancodeArmsRev = conf.key_bindings[8];
	gScancodeArms = conf.key_bindings[9];
	gScancodeItem = conf.key_bindings[10];
	gScancodeMap = conf.key_bindings[11];
	gScancodePause = conf.key_bindings[12];

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

			StartDirectDraw(window, conf.display_mode, conf.b60fps, conf.bVsync);

			break;

		case 0:
			// Fullscreen
			windowWidth = WINDOW_WIDTH * 2;
			windowHeight = WINDOW_HEIGHT * 2;

			window = CreateWindow(lpWindowName, windowWidth, windowHeight);

			if (window == NULL)
				return 0;

			StartDirectDraw(window, 0, conf.b60fps, conf.bVsync);
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
	PutBitmap3(&rcFull, PixelToScreenCoord((WINDOW_WIDTH / 2) - 32), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 4), &rcLoading, SURFACE_ID_LOADING);

	// Draw to screen
	if (!Flip_SystemTask())
	{
        SDL_FreeCursor(cursor);
        SDL_FreeSurface(cursor_surface);
		free(image_buffer);
		SDL_DestroyWindow(window);
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
	free(image_buffer);
	SDL_DestroyWindow(window);

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
				if (event.key.keysym.scancode == gScancodePause)
					gKey |= KEY_ESCAPE;
				if (event.key.keysym.scancode == gScancodeMap)
						gKey |= KEY_MAP;
				if (event.key.keysym.scancode == gScancodeLeft)
						gKey |= KEY_LEFT;
				if (event.key.keysym.scancode == gScancodeRight)
						gKey |= KEY_RIGHT;
				if (event.key.keysym.scancode == gScancodeUp)
						gKey |= KEY_UP;
				if (event.key.keysym.scancode == gScancodeDown)
						gKey |= KEY_DOWN;
				if (event.key.keysym.scancode == gScancodeShot)
						gKey |= KEY_SHOT;
				if (event.key.keysym.scancode == gScancodeJump)
						gKey |= KEY_JUMP;
				if (event.key.keysym.scancode == gScancodeArms)
						gKey |= KEY_ARMS;
				if (event.key.keysym.scancode == gScancodeArmsRev)
						gKey |= KEY_ARMSREV;
				if (event.key.keysym.scancode == SDL_SCANCODE_F1)
						gKey |= KEY_F1;
				if (event.key.keysym.scancode == SDL_SCANCODE_F2)
						gKey |= KEY_F2;
				if (event.key.keysym.scancode == gScancodeItem)
						gKey |= KEY_ITEM;
				if (event.key.keysym.scancode == SDL_SCANCODE_F5)
						gbUseJoystick = FALSE;
				if (event.key.keysym.scancode == gScancodeCancel)
						gKey |= KEY_CANCEL;
				if (event.key.keysym.scancode == gScancodeOk)
						gKey |= KEY_OK;

				break;

			case SDL_KEYUP:
				if (event.key.keysym.scancode == gScancodePause)
						gKey &= ~KEY_ESCAPE;
				if (event.key.keysym.scancode == gScancodeMap)
						gKey &= ~KEY_MAP;
				if (event.key.keysym.scancode == gScancodeLeft)
						gKey &= ~KEY_LEFT;
				if (event.key.keysym.scancode == gScancodeRight)
						gKey &= ~KEY_RIGHT;
				if (event.key.keysym.scancode == gScancodeUp)
						gKey &= ~KEY_UP;
				if (event.key.keysym.scancode == gScancodeDown)
						gKey &= ~KEY_DOWN;
				if (event.key.keysym.scancode == gScancodeShot)
						gKey &= ~KEY_SHOT;
				if (event.key.keysym.scancode == gScancodeJump)
						gKey &= ~KEY_JUMP;
				if (event.key.keysym.scancode == gScancodeArms)
						gKey &= ~KEY_ARMS;
				if (event.key.keysym.scancode == gScancodeArmsRev)
						gKey &= ~KEY_ARMSREV;
				if (event.key.keysym.scancode == SDL_SCANCODE_F1)
						gKey &= ~KEY_F1;
				if (event.key.keysym.scancode == SDL_SCANCODE_F2)
						gKey &= ~KEY_F2;
				if (event.key.keysym.scancode == gScancodeItem)
						gKey &= ~KEY_ITEM;
				if (event.key.keysym.scancode == gScancodeCancel)
						gKey &= ~KEY_CANCEL;
				if (event.key.keysym.scancode == gScancodeOk)
						gKey &= ~KEY_OK;

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
