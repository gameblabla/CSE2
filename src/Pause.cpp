#include "Pause.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Main.h"
#include "Organya.h"
#include "Sound.h"

typedef struct Option
{
	const char *name;
	int (*callback)(Option *option, long key);
	void *user_data;
	const char *attribute;
	long value;
} Option;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

static int EnterOptionsMenu(Option *options, size_t total_options, int x_offset, BOOL submenu)
{
	unsigned int selected_option = 0;

	unsigned int anime = 0;

	int return_value;

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		if (!submenu && gKeyTrg & KEY_ESCAPE)
		{
			return_value = -2;
			break;
		}

		if (gKeyTrg & gKeyCancel)
		{
			return_value = -1;
			break;
		}

		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			if (gKeyTrg & gKeyDown)
				if (selected_option++ == total_options - 1)
					selected_option = 0;

			if (gKeyTrg & gKeyUp)
				if (selected_option-- == 0)
					selected_option = total_options - 1;

			PlaySoundObject(1, 1);
		}

		if (gKeyTrg & (gKeyOk | gKeyLeft | gKeyRight))
		{
			return_value = options[selected_option].callback(&options[selected_option], gKeyTrg);

			if (return_value != -1)
				break;
		}

		if (++anime >= 40)
			anime = 0;

		// Draw screen
		CortBox(&grcFull, 0x000000);

		for (size_t i = 0; i < total_options; ++i)
		{
			const int x = (WINDOW_WIDTH / 2) + x_offset;
			const int y = (WINDOW_HEIGHT / 2) - (((total_options - 1) * 20) / 2) + (i * 20);

			if (i == selected_option)
				PutBitmap3(&grcGame, PixelToScreenCoord(x - 20), PixelToScreenCoord(y - 8), &rcMyChar[anime / 10 % 4], SURFACE_ID_MY_CHAR);

			PutText(x, y - (9 / 2), options[i].name, RGB(0xFF, 0xFF, 0xFF));

			if (options[i].attribute != NULL)
				PutText(x + 100, y - (9 / 2), options[i].attribute, RGB(0xFF, 0xFF, 0xFF));
		}

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			return_value = 0;
			break;
		}
	}

	if (!submenu && (return_value == -1 || return_value == -2))
		return_value = 1;

	return return_value;
}

static int Callback_KeyRebind(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(5, 1);

	int total_keys;
	const Uint8 *state = SDL_GetKeyboardState(&total_keys);

	Uint8 *old_state = (Uint8*)malloc(total_keys);

	memcpy(old_state, state, total_keys);

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		for (int i = 0; i < total_keys; ++i)
		{
			if (((old_state[i] ^ state[i]) & state[i]) == 1)
			{
				const char *key_name = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)i));

				free((char*)option->attribute);

				option->attribute = (char*)malloc(strlen(key_name));
				strcpy((char*)option->attribute, key_name);

				*(int*)option->user_data = i;

				PlaySoundObject(18, 1);
				free(old_state);
				return -1;
			}
		}

		memcpy(old_state, state, total_keys);

		CortBox(&grcFull, 0x000000);

		const char *string = "Press a key to bind to this action:";
		PutText((WINDOW_WIDTH / 2) - ((strlen(string) * 5) / 2), (WINDOW_HEIGHT / 2) - 10, string, RGB(0xFF, 0xFF, 0xFF));
		PutText((WINDOW_WIDTH / 2) - ((strlen(option->name) * 5) / 2), (WINDOW_HEIGHT / 2) + 10, option->name, RGB(0xFF, 0xFF, 0xFF));

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			free(old_state);
			return 0;
		}
	}
}

static int Callback_Controls(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	Option options[] = {
		{"Up", Callback_KeyRebind, &gScancodeUp},
		{"Down", Callback_KeyRebind, &gScancodeDown},
		{"Left", Callback_KeyRebind, &gScancodeLeft},
		{"Right", Callback_KeyRebind, &gScancodeRight},
		{"OK", Callback_KeyRebind, &gScancodeOk},
		{"Cancel", Callback_KeyRebind, &gScancodeCancel},
		{"Jump", Callback_KeyRebind, &gScancodeJump},
		{"Shoot", Callback_KeyRebind, &gScancodeShot},
		{"Previous weapon", Callback_KeyRebind, &gScancodeArmsRev},
		{"Next weapon", Callback_KeyRebind, &gScancodeArms},
		{"Inventory", Callback_KeyRebind, &gScancodeItem},
		{"Map", Callback_KeyRebind, &gScancodeMap},
		{"Pause", Callback_KeyRebind, &gScancodePause}
	};

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -80, TRUE);

	for (size_t i = 0; i < sizeof(options) / sizeof(options[0]); ++i)
		free((char*)option->attribute);

	PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_Framerate(Option *option, long key)
{
	(void)key;

	const char *strings[] = {"50FPS", "60FPS"};

	option->value = (option->value + 1) % (sizeof(strings) / sizeof(strings[0]));
	
	gb60fps = option->value;

	option->attribute = strings[option->value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Vsync(Option *option, long key)
{
	(void)key;

	const char *strings[] = {"Off (needs restart)", "On (needs restart)"};

	option->value = (option->value + 1) % (sizeof(strings) / sizeof(strings[0]));

	option->attribute = strings[option->value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resolution(Option *option, long key)
{
	const char *strings[] = {"Fullscreen (needs restart)", "Windowed 426x240 (needs restart)", "Windowed 852x480 (needs restart)", "Windowed 1278x720 (needs restart)", "Windowed 1704x960 (needs restart)"};

	if (key & gKeyLeft)
		--option->value;
	else
		++option->value;

	option->value %= (sizeof(strings) / sizeof(strings[0]));

	gb60fps = option->value;

	option->attribute = strings[option->value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resume(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 1;
}

static int Callback_Reset(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 2;
}

static int Callback_Options(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	Option options[] = {
		{"Controls", Callback_Controls},
		{"Framerate", Callback_Framerate},
		{"V-sync", Callback_Vsync},
		{"Resolution", Callback_Resolution}
	};

	CONFIG conf;
	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	options[1].value = conf.b60fps;
	options[1].attribute = conf.b60fps ? "60FPS" : "50FPS";

	options[2].value = conf.bVsync;
	options[2].attribute = conf.bVsync ? "On" : "Off";

	options[3].value = conf.display_mode;
	switch (conf.display_mode)
	{
		case 0:
			options[3].attribute = "Fullscreen";
			break;

		case 1:
			options[3].attribute = "Windowed 426x240";
			break;

		case 2:
			options[3].attribute = "Windowed 852x480";
			break;

		case 3:
			options[3].attribute = "Windowed 1278x720";
			break;

		case 4:
			options[3].attribute = "Windowed 1704x960";
			break;
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -80, TRUE);

	PlaySoundObject(5, 1);

	conf.b60fps = options[1].value;
	conf.bVsync = options[2].value;
	conf.display_mode = options[3].value;

	SaveConfigData(&conf);

	return return_value;
}

static int Callback_Quit(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

	return 0;
}

int Call_Pause(void)
{
	Option options[] = {
		{"Resume", Callback_Resume},
		{"Reset", Callback_Reset},
		{"Options", Callback_Options},
		{"Quit", Callback_Quit}
	};

	int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -30, FALSE);

	gKeyTrg = gKey = 0;

	return return_value;
}
