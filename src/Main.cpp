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

BOOL bFullscreen;
BOOL gbUseJoystick = FALSE;

int gKeyOk_Scancode = SDL_SCANCODE_Z;
int gKeyCancel_Scancode = SDL_SCANCODE_X;
int gKeyJump_Scancode = SDL_SCANCODE_Z;
int gKeyShot_Scancode = SDL_SCANCODE_X;
int gKeyArms_Scancode = SDL_SCANCODE_S;
int gKeyArmsRev_Scancode = SDL_SCANCODE_A;
int gKeyItem_Scancode = SDL_SCANCODE_Q;
int gKeyMap_Scancode = SDL_SCANCODE_W;
int gKeyUp_Scancode = SDL_SCANCODE_UP;
int gKeyDown_Scancode = SDL_SCANCODE_DOWN;
int gKeyLeft_Scancode = SDL_SCANCODE_LEFT;
int gKeyRight_Scancode = SDL_SCANCODE_RIGHT;
int gKeyPause_Scancode = SDL_SCANCODE_ESCAPE;

int gKeyOk_Button;
int gKeyCancel_Button;
int gKeyJump_Button;
int gKeyShot_Button;
int gKeyArms_Button;
int gKeyArmsRev_Button;
int gKeyItem_Button;
int gKeyMap_Button;
int gKeyUp_Button;
int gKeyDown_Button;
int gKeyLeft_Button;
int gKeyRight_Button;
int gKeyPause_Button;

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
	gKeyUp_Scancode = conf.key_bindings[0];
	gKeyDown_Scancode = conf.key_bindings[1];
	gKeyLeft_Scancode = conf.key_bindings[2];
	gKeyRight_Scancode = conf.key_bindings[3];
	gKeyOk_Scancode = conf.key_bindings[4];
	gKeyCancel_Scancode = conf.key_bindings[5];
	gKeyJump_Scancode = conf.key_bindings[6];
	gKeyShot_Scancode = conf.key_bindings[7];
	gKeyArmsRev_Scancode = conf.key_bindings[8];
	gKeyArms_Scancode = conf.key_bindings[9];
	gKeyItem_Scancode = conf.key_bindings[10];
	gKeyMap_Scancode = conf.key_bindings[11];
	gKeyPause_Scancode = conf.key_bindings[12];

	// Set gamepad inputs
	gKeyUp_Button = conf.button_bindings[0];
	gKeyDown_Button = conf.button_bindings[1];
	gKeyLeft_Button = conf.button_bindings[2];
	gKeyRight_Button = conf.button_bindings[3];
	gKeyOk_Button = conf.button_bindings[4];
	gKeyCancel_Button = conf.button_bindings[5];
	gKeyJump_Button = conf.button_bindings[6];
	gKeyShot_Button = conf.button_bindings[7];
	gKeyArmsRev_Button = conf.button_bindings[8];
	gKeyArms_Button = conf.button_bindings[9];
	gKeyItem_Button = conf.button_bindings[10];
	gKeyMap_Button = conf.button_bindings[11];
	gKeyPause_Button = conf.button_bindings[12];

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
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					gKey |= KEY_ESCAPE;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F1)
					gKey |= KEY_F1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F2)
					gKey |= KEY_F2;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F5)
					gbUseJoystick = FALSE;

				if (event.key.keysym.scancode == gKeyMap_Scancode)
					gKey |= KEY_MAP;
				if (event.key.keysym.scancode == gKeyLeft_Scancode)
					gKey |= KEY_LEFT;
				if (event.key.keysym.scancode == gKeyRight_Scancode)
					gKey |= KEY_RIGHT;
				if (event.key.keysym.scancode == gKeyUp_Scancode)
					gKey |= KEY_UP;
				if (event.key.keysym.scancode == gKeyDown_Scancode)
					gKey |= KEY_DOWN;
				if (event.key.keysym.scancode == gKeyShot_Scancode)
					gKey |= KEY_SHOT;
				if (event.key.keysym.scancode == gKeyJump_Scancode)
					gKey |= KEY_JUMP;
				if (event.key.keysym.scancode == gKeyArms_Scancode)
					gKey |= KEY_ARMS;
				if (event.key.keysym.scancode == gKeyArmsRev_Scancode)
					gKey |= KEY_ARMSREV;
				if (event.key.keysym.scancode == gKeyItem_Scancode)
					gKey |= KEY_ITEM;
				if (event.key.keysym.scancode == gKeyCancel_Scancode)
					gKey |= KEY_CANCEL;
				if (event.key.keysym.scancode == gKeyOk_Scancode)
					gKey |= KEY_OK;
				if (event.key.keysym.scancode == gKeyPause_Scancode)
					gKey |= KEY_PAUSE;

				break;

			case SDL_KEYUP:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					gKey &= ~KEY_ESCAPE;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F1)
					gKey &= ~KEY_F1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F2)
					gKey &= ~KEY_F2;

				if (event.key.keysym.scancode == gKeyMap_Scancode)
					gKey &= ~KEY_MAP;
				if (event.key.keysym.scancode == gKeyLeft_Scancode)
					gKey &= ~KEY_LEFT;
				if (event.key.keysym.scancode == gKeyRight_Scancode)
					gKey &= ~KEY_RIGHT;
				if (event.key.keysym.scancode == gKeyUp_Scancode)
					gKey &= ~KEY_UP;
				if (event.key.keysym.scancode == gKeyDown_Scancode)
					gKey &= ~KEY_DOWN;
				if (event.key.keysym.scancode == gKeyShot_Scancode)
					gKey &= ~KEY_SHOT;
				if (event.key.keysym.scancode == gKeyJump_Scancode)
					gKey &= ~KEY_JUMP;
				if (event.key.keysym.scancode == gKeyArms_Scancode)
					gKey &= ~KEY_ARMS;
				if (event.key.keysym.scancode == gKeyArmsRev_Scancode)
					gKey &= ~KEY_ARMSREV;
				if (event.key.keysym.scancode == gKeyItem_Scancode)
					gKey &= ~KEY_ITEM;
				if (event.key.keysym.scancode == gKeyCancel_Scancode)
					gKey &= ~KEY_CANCEL;
				if (event.key.keysym.scancode == gKeyOk_Scancode)
					gKey &= ~KEY_OK;
				if (event.key.keysym.scancode == gKeyPause_Scancode)
					gKey &= ~KEY_PAUSE;

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
	static JOYSTICK_STATUS old_status;

	if (!GetJoystickStatus(&status))
		return;

//	gKey &= (KEY_ESCAPE | KEY_F2 | KEY_F1);

	// Set movement buttons
	if (status.bLeft && !old_status.bLeft)
		gKey |= gKeyLeft;
	else if (!status.bLeft && old_status.bLeft)
		gKey &= ~gKeyLeft;

	if (status.bRight && !old_status.bRight)
		gKey |= gKeyRight;
	else if (!status.bRight && old_status.bRight)
		gKey &= ~gKeyRight;

	if (status.bUp && !old_status.bUp)
		gKey |= gKeyUp;
	else if (!status.bUp && old_status.bUp)
		gKey &= ~gKeyUp;

	if (status.bDown && !old_status.bDown)
		gKey |= gKeyDown;
	else if (!status.bDown && old_status.bDown)
		gKey &= ~gKeyDown;

	// Set held buttons
	for (i = 0; i < MAX_JOYSTICK_BUTTONS; ++i)
	{
		if (status.bButton[i] && !old_status.bButton[i])
		{
			if (i == gKeyMap_Button)
				gKey |= KEY_MAP;
			if (i == gKeyLeft_Button)
				gKey |= KEY_LEFT;
			if (i == gKeyRight_Button)
				gKey |= KEY_RIGHT;
			if (i == gKeyUp_Button)
				gKey |= KEY_UP;
			if (i == gKeyDown_Button)
				gKey |= KEY_DOWN;
			if (i == gKeyShot_Button)
				gKey |= KEY_SHOT;
			if (i == gKeyJump_Button)
				gKey |= KEY_JUMP;
			if (i == gKeyArms_Button)
				gKey |= KEY_ARMS;
			if (i == gKeyArmsRev_Button)
				gKey |= KEY_ARMSREV;
			if (i == gKeyItem_Button)
				gKey |= KEY_ITEM;
			if (i == gKeyCancel_Button)
				gKey |= KEY_CANCEL;
			if (i == gKeyOk_Button)
				gKey |= KEY_OK;
			if (i == gKeyPause_Button)
				gKey |= KEY_PAUSE;
		}
		else if (!status.bButton[i] && old_status.bButton[i])
		{
			if (i == gKeyMap_Button)
				gKey &= ~KEY_MAP;
			if (i == gKeyLeft_Button)
				gKey &= ~KEY_LEFT;
			if (i == gKeyRight_Button)
				gKey &= ~KEY_RIGHT;
			if (i == gKeyUp_Button)
				gKey &= ~KEY_UP;
			if (i == gKeyDown_Button)
				gKey &= ~KEY_DOWN;
			if (i == gKeyShot_Button)
				gKey &= ~KEY_SHOT;
			if (i == gKeyJump_Button)
				gKey &= ~KEY_JUMP;
			if (i == gKeyArms_Button)
				gKey &= ~KEY_ARMS;
			if (i == gKeyArmsRev_Button)
				gKey &= ~KEY_ARMSREV;
			if (i == gKeyItem_Button)
				gKey &= ~KEY_ITEM;
			if (i == gKeyCancel_Button)
				gKey &= ~KEY_CANCEL;
			if (i == gKeyOk_Button)
				gKey &= ~KEY_OK;
			if (i == gKeyPause_Button)
				gKey &= ~KEY_PAUSE;
		}
	}

	old_status = status;
}
