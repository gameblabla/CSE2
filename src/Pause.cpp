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
	int (*callback)(Option *options, size_t total_options, size_t index, long key);
	void *user_data;
	const char *attribute;
	long value;
} Option;

typedef struct Control
{
	const char *name;
	int *scancode;
	int groups;
} Control;

static CONFIG conf;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

static const Control controls[] = {
	{"Up", &gScancodeUp, (1 << 0) | (1 << 1)},
	{"Down", &gScancodeDown, (1 << 0) | (1 << 1)},
	{"Left", &gScancodeLeft, (1 << 0) | (1 << 1)},
	{"Right", &gScancodeRight, (1 << 0) | (1 << 1)},
	{"OK", &gScancodeOk, 1 << 1},
	{"Cancel", &gScancodeCancel, 1 << 1},
	{"Jump", &gScancodeJump, 1 << 0},
	{"Shoot", &gScancodeShot, 1 << 0},
	{"Previous weapon", &gScancodeArmsRev, 1 << 0},
	{"Next weapon", &gScancodeArms, 1 << 0},
	{"Inventory", &gScancodeItem, 1 << 0},
	{"Map", &gScancodeMap, 1 << 0},
	{"Pause", &gScancodePause, 1 << 0}
};

static int EnterOptionsMenu(Option *options, size_t total_options, int x_offset, BOOL submenu)
{
	size_t selected_option = 0;

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
			return_value = options[selected_option].callback(options, total_options, selected_option, gKeyTrg);

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

static int Callback_KeyRebind(Option *options, size_t total_options, size_t index, long key)
{
	if (!(key & gKeyOk))
		return -1;
		printf("Entered\n");

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

				// If another key in the game group uses this key, swap them
				for (size_t j = 0; j < total_options; ++j)
				{
					if (j != index && controls[j].groups & controls[index].groups && options[j].value == i)
					{
						options[j].value = options[index].value;
						options[index].value = i;

						*controls[(size_t)options[j].user_data].scancode = options[j].value;
						*controls[(size_t)options[index].user_data].scancode = options[index].value;
						conf.key_bindings[j] = options[j].value;
						conf.key_bindings[index] = options[index].value;

						free((char*)options[j].attribute);
						options[j].attribute = options[index].attribute;

						options[index].attribute = (char*)malloc(strlen(key_name));
						strcpy((char*)options[index].attribute, key_name);

						PlaySoundObject(18, 1);
						free(old_state);

						gKeyTrg = gKey = 0;	// Prevent weird key-ghosting by doing this
						return -1;
					}
				}

				// Otherwise just overwrite the selected key
				free((char*)options[index].attribute);

				options[index].attribute = (char*)malloc(strlen(key_name));
				strcpy((char*)options[index].attribute, key_name);

				options[index].value = i;
				*controls[(size_t)options[index].user_data].scancode = options[index].value;
				conf.key_bindings[index] = options[index].value;

				PlaySoundObject(18, 1);
				free(old_state);

				gKeyTrg = gKey = 0;	// Prevent weird key-ghosting by doing this
				return -1;
			}
		}

		memcpy(old_state, state, total_keys);

		CortBox(&grcFull, 0x000000);

		const char *string = "Press a key to bind to this action:";
		PutText((WINDOW_WIDTH / 2) - ((strlen(string) * 5) / 2), (WINDOW_HEIGHT / 2) - 10, string, RGB(0xFF, 0xFF, 0xFF));
		PutText((WINDOW_WIDTH / 2) - ((strlen(options[index].name) * 5) / 2), (WINDOW_HEIGHT / 2) + 10, options[index].name, RGB(0xFF, 0xFF, 0xFF));

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			free(old_state);
			return 0;
		}
	}
}

static int Callback_Controls(Option *options, size_t total_options, size_t index, long key)
{
	(void)options;
	(void)total_options;
	(void)index;

	if (!(key & gKeyOk))
		return -1;

	Option submenu_options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		submenu_options[i].name = controls[i].name;
		submenu_options[i].callback = Callback_KeyRebind;
		submenu_options[i].user_data = (void*)i;
		submenu_options[i].value = conf.key_bindings[i];

		const char *key_name = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)submenu_options[i].value));
		submenu_options[i].attribute = (char*)malloc(strlen(key_name));
		strcpy((char*)submenu_options[i].attribute, key_name);
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -80, TRUE);

	for (size_t i = 0; i < sizeof(submenu_options) / sizeof(submenu_options[0]); ++i)
		free((char*)submenu_options[i].attribute);

	PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_Framerate(Option *options, size_t total_options, size_t index, long key)
{
	(void)total_options;
	(void)key;

	const char *strings[] = {"50FPS", "60FPS"};

	options[index].value = (options[index].value + 1) % (sizeof(strings) / sizeof(strings[0]));
	
	gb60fps = options[index].value;

	options[index].attribute = strings[options[index].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Vsync(Option *options, size_t total_options, size_t index, long key)
{
	(void)total_options;
	(void)key;

	const char *strings[] = {"Off (needs restart)", "On (needs restart)"};

	options[index].value = (options[index].value + 1) % (sizeof(strings) / sizeof(strings[0]));

	options[index].attribute = strings[options[index].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resolution(Option *options, size_t total_options, size_t index, long key)
{
	(void)total_options;

	const char *strings[] = {"Fullscreen (needs restart)", "Windowed 426x240 (needs restart)", "Windowed 852x480 (needs restart)", "Windowed 1278x720 (needs restart)", "Windowed 1704x960 (needs restart)"};

	if (key & gKeyLeft)
		--options[index].value;
	else
		++options[index].value;

	options[index].value %= (sizeof(strings) / sizeof(strings[0]));

	gb60fps = options[index].value;

	options[index].attribute = strings[options[index].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resume(Option *options, size_t total_options, size_t index, long key)
{
	(void)options;
	(void)total_options;
	(void)index;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 1;
}

static int Callback_Reset(Option *options, size_t total_options, size_t index, long key)
{
	(void)options;
	(void)total_options;
	(void)index;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 2;
}

static int Callback_Options(Option *options, size_t total_options, size_t index, long key)
{
	(void)options;
	(void)total_options;
	(void)index;

	if (!(key & gKeyOk))
		return -1;

	Option submenu_options[] = {
		{"Controls", Callback_Controls},
		{"Framerate", Callback_Framerate},
		{"V-sync", Callback_Vsync},
		{"Resolution", Callback_Resolution}
	};

	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	submenu_options[1].value = conf.b60fps;
	submenu_options[1].attribute = conf.b60fps ? "60FPS" : "50FPS";

	submenu_options[2].value = conf.bVsync;
	submenu_options[2].attribute = conf.bVsync ? "On" : "Off";

	submenu_options[3].value = conf.display_mode;
	switch (conf.display_mode)
	{
		case 0:
			submenu_options[3].attribute = "Fullscreen";
			break;

		case 1:
			submenu_options[3].attribute = "Windowed 426x240";
			break;

		case 2:
			submenu_options[3].attribute = "Windowed 852x480";
			break;

		case 3:
			submenu_options[3].attribute = "Windowed 1278x720";
			break;

		case 4:
			submenu_options[3].attribute = "Windowed 1704x960";
			break;
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -80, TRUE);

	PlaySoundObject(5, 1);

	conf.b60fps = submenu_options[1].value;
	conf.bVsync = submenu_options[2].value;
	conf.display_mode = submenu_options[3].value;

	SaveConfigData(&conf);

	return return_value;
}

static int Callback_Quit(Option *options, size_t total_options, size_t index, long key)
{
	(void)options;
	(void)total_options;
	(void)index;

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
