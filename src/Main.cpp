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

CONFIG_BINDING bindings[BINDING_TOTAL];

static BOOL bActive = TRUE;
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

	memcpy(bindings, conf.bindings, sizeof(bindings));

	RECT unused_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

#ifdef _WIN32	// On Windows, we use native icons instead (so we can give the taskbar and window separate icons, like the original EXE does)
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON, "0");
	SDL_SetHint(SDL_HINT_WINDOWS_INTRESOURCE_ICON_SMALL, "1");
#endif

	SDL_InitSubSystem(SDL_INIT_VIDEO);

	switch (conf.display_mode)
	{
		default:
			// Windowed

		#ifdef FIX_BUGS
			if (!StartDirectDraw(lpWindowName, conf.display_mode, conf.b60fps, conf.bSmoothScrolling, conf.bVsync))
				return 0;
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, conf.display_mode, conf.b60fps, conf.bSmoothScrolling, conf.bVsync);
		#endif

			break;

		case 0:
			// Fullscreen

		#ifdef FIX_BUGS
			if (!StartDirectDraw(lpWindowName, 0, conf.b60fps, conf.bSmoothScrolling, conf.bVsync))
				return 0;
		#else
			// Doesn't handle StartDirectDraw failing
			StartDirectDraw(lpWindowName, 0, conf.b60fps, conf.bSmoothScrolling, conf.bVsync);
		#endif

			bFullscreen = TRUE;

			SDL_ShowCursor(SDL_DISABLE);
			break;
	}

#ifdef DEBUG_SAVE
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
#endif

	// Set up the cursor
	char image_path[MAX_PATH];
	sprintf(image_path, "%s/Resource/CURSOR/CURSOR_NORMAL.png", gDataPath);

	unsigned char *image_buffer;
	unsigned int image_width;
	unsigned int image_height;
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
		return 1;
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

	SDL_FreeCursor(cursor);
	SDL_FreeSurface(cursor_surface);
	free(image_buffer);

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

				if (event.key.keysym.scancode == bindings[BINDING_MAP].keyboard)
					gKey |= KEY_MAP;
				if (event.key.keysym.scancode == bindings[BINDING_LEFT].keyboard)
					gKey |= KEY_LEFT;
				if (event.key.keysym.scancode == bindings[BINDING_RIGHT].keyboard)
					gKey |= KEY_RIGHT;
				if (event.key.keysym.scancode == bindings[BINDING_UP].keyboard)
					gKey |= KEY_UP;
				if (event.key.keysym.scancode == bindings[BINDING_DOWN].keyboard)
					gKey |= KEY_DOWN;
				if (event.key.keysym.scancode == bindings[BINDING_SHOT].keyboard)
					gKey |= KEY_SHOT;
				if (event.key.keysym.scancode == bindings[BINDING_JUMP].keyboard)
					gKey |= KEY_JUMP;
				if (event.key.keysym.scancode == bindings[BINDING_ARMS].keyboard)
					gKey |= KEY_ARMS;
				if (event.key.keysym.scancode == bindings[BINDING_ARMSREV].keyboard)
					gKey |= KEY_ARMSREV;
				if (event.key.keysym.scancode == bindings[BINDING_ITEM].keyboard)
					gKey |= KEY_ITEM;
				if (event.key.keysym.scancode == bindings[BINDING_CANCEL].keyboard)
					gKey |= KEY_CANCEL;
				if (event.key.keysym.scancode == bindings[BINDING_OK].keyboard)
					gKey |= KEY_OK;
				if (event.key.keysym.scancode == bindings[BINDING_PAUSE].keyboard)
					gKey |= KEY_PAUSE;

				break;

			case SDL_KEYUP:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					gKey &= ~KEY_ESCAPE;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F1)
					gKey &= ~KEY_F1;
				else if (event.key.keysym.scancode == SDL_SCANCODE_F2)
					gKey &= ~KEY_F2;

				if (event.key.keysym.scancode == bindings[BINDING_MAP].keyboard)
					gKey &= ~KEY_MAP;
				if (event.key.keysym.scancode == bindings[BINDING_LEFT].keyboard)
					gKey &= ~KEY_LEFT;
				if (event.key.keysym.scancode == bindings[BINDING_RIGHT].keyboard)
					gKey &= ~KEY_RIGHT;
				if (event.key.keysym.scancode == bindings[BINDING_UP].keyboard)
					gKey &= ~KEY_UP;
				if (event.key.keysym.scancode == bindings[BINDING_DOWN].keyboard)
					gKey &= ~KEY_DOWN;
				if (event.key.keysym.scancode == bindings[BINDING_SHOT].keyboard)
					gKey &= ~KEY_SHOT;
				if (event.key.keysym.scancode == bindings[BINDING_JUMP].keyboard)
					gKey &= ~KEY_JUMP;
				if (event.key.keysym.scancode == bindings[BINDING_ARMS].keyboard)
					gKey &= ~KEY_ARMS;
				if (event.key.keysym.scancode == bindings[BINDING_ARMSREV].keyboard)
					gKey &= ~KEY_ARMSREV;
				if (event.key.keysym.scancode == bindings[BINDING_ITEM].keyboard)
					gKey &= ~KEY_ITEM;
				if (event.key.keysym.scancode == bindings[BINDING_CANCEL].keyboard)
					gKey &= ~KEY_CANCEL;
				if (event.key.keysym.scancode == bindings[BINDING_OK].keyboard)
					gKey &= ~KEY_OK;
				if (event.key.keysym.scancode == bindings[BINDING_PAUSE].keyboard)
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

			case SDL_JOYDEVICEADDED:
			#ifndef NDEBUG
				printf("Joystick connected:\nIndex - %d\nName - '%s'\n", event.jdevice.which, SDL_JoystickNameForIndex(event.jdevice.which));
			#endif

				if (joystick == NULL)
				{
					joystick = SDL_JoystickOpen(event.jdevice.which);

					if (joystick != NULL)
						ResetJoystickStatus();
				}

				break;

			case SDL_JOYDEVICEREMOVED:
				#ifndef NDEBUG
					puts("Joystick disconnected");
				#endif

				if (SDL_JoystickFromInstanceID(event.jdevice.which) == joystick)
				{
					SDL_JoystickClose(joystick);
					joystick = NULL;
				}

				break;
		}
	}

	// Run joystick code
	JoystickProc();

	return TRUE;
}

void JoystickProc(void)
{
	int i;
	JOYSTICK_STATUS status;
	static JOYSTICK_STATUS old_status;

	if (!GetJoystickStatus(&status))
		memset(&status, 0, sizeof(status));

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
		else if (!status.bButton[i] && old_status.bButton[i])
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

	old_status = status;
}
