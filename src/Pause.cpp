#include "Pause.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Main.h"
#include "Organya.h"
#include "Sound.h"

#include "SDL.h"

typedef struct Option
{
	const char *name;
	const char *attribute;
	long value;
	int (*callback)(Option *option, long key);
} Option;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

static int EnterOptionsMenu(Option *options, size_t total_options, int x_offset)
{
	unsigned int selected_option = 0;

	unsigned int anime = 0;

	int return_value;

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		if (gKeyTrg & KEY_ESCAPE)
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

	return return_value;
}

static int Callback_Null(Option *option, long key)
{
	(void)option;

	if (!(key & gKeyOk))
		return -1;

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
				option->attribute = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)i));
				return -1;
			}
		}

		memcpy(old_state, state, total_keys);

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
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
		{"OK", NULL, 0, Callback_Null},
		{"Cancel", NULL, 0, Callback_Null},
		{"Jump", NULL, 0, Callback_Null},
		{"Shoot", NULL, 0, Callback_Null},
		{"Previous weapon", NULL, 0, Callback_Null},
		{"Next weapon", NULL, 0, Callback_Null},
		{"Inventory", NULL, 0, Callback_Null},
		{"Map", NULL, 0, Callback_Null},
		{"Pause", NULL, 0, Callback_Null}
	};

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -80);

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
		{"Controls", NULL, 0, Callback_Controls},
		{"Framerate", NULL, 0, Callback_Framerate},
		{"V-sync", NULL, 0, Callback_Vsync},
		{"Resolution", NULL, 0, Callback_Resolution}
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

	const int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -80);

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
		{"Resume", NULL, 0, Callback_Resume},
		{"Reset", NULL, 0, Callback_Reset},
		{"Options", NULL, 0, Callback_Options},
		{"Quit", NULL, 0, Callback_Quit}
	};

	StopOrganyaMusic();

	PlaySoundObject(5, 1);

	int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]), -30);

	if (return_value == -1 || return_value == -2)
		return_value = 1;

//	PlaySoundObject(18, 1);

	PlayOrganyaMusic();

	gKeyTrg = gKey = 0;

	return return_value;
}
