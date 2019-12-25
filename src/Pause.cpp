#include "Pause.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Main.h"
#include "Organya.h"
#include "Sound.h"

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

static int EnterOptionsMenu(Option *options, size_t total_options)
{
	unsigned int selected_option = 0;

	unsigned int anime = 0;

	int return_value;

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		if (gKeyTrg & (KEY_ESCAPE | gKeyCancel))
		{
			return_value = -1;
			break;
		}

		if (gKeyTrg & (gKeyOk | gKeyLeft | gKeyRight))
		{
			return_value = options[selected_option].callback(&options[selected_option], gKeyTrg);

			if (return_value != -1)
				break;
		}

		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			if (gKeyTrg & gKeyDown)
				++selected_option;

			if (gKeyTrg & gKeyUp)
				--selected_option;

			selected_option %= total_options;

			PlaySoundObject(1, 1);
		}

		if (++anime >= 40)
			anime = 0;

		// Draw screen
		CortBox(&grcFull, 0x000000);

		for (size_t i = 0; i < total_options; ++i)
		{
			const int x = (WINDOW_WIDTH / 2) - 20;
			const int y = (WINDOW_HEIGHT / 2) - (((total_options - 1) * 20) / 2) + (i * 20);

			if (i == selected_option)
				PutBitmap3(&grcGame, PixelToScreenCoord(x - 20), PixelToScreenCoord(y - 8), &rcMyChar[anime / 10 % 4], SURFACE_ID_MY_CHAR);

			PutText(x, y - (9 / 2), options[i].name, RGB(0xFF, 0xFF, 0xFF));

			if (options[i].attribute != NULL)
				PutText(x + 60, y - (9 / 2), options[i].attribute, RGB(0xFF, 0xFF, 0xFF));
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

static int Callback_Controls(Option *option, long key)
{
	(void)option;
	(void)key;

	return -1;
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

	const char *strings[] = {"Off", "On"};

	option->value = (option->value + 1) % (sizeof(strings) / sizeof(strings[0]));

	option->attribute = strings[option->value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resolution(Option *option, long key)
{
	const char *strings[] = {"Fullscreen", "Windowed 426x240", "Windowed 852x480", "Windowed 1278x720", "Windowed 1704x960"};

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
	(void)key;

	PlaySoundObject(18, 1);
	return 1;
}

static int Callback_Reset(Option *option, long key)
{
	(void)option;
	(void)key;

	PlaySoundObject(18, 1);
	return 2;
}

static int Callback_Options(Option *option, long key)
{
	(void)option;
	(void)key;

	Option options[] = {
		{"Controls", NULL, 0, Callback_Controls},
		{"Framerate", NULL, 0, Callback_Framerate},
		{"V-sync", NULL, 0, Callback_Vsync},
		{"Resolution", NULL, 0, Callback_Resolution}
	};

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]));

	PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_Quit(Option *option, long key)
{
	(void)option;
	(void)key;

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

	int return_value = EnterOptionsMenu(options, sizeof(options) / sizeof(options[0]));

	if (return_value == -1)
		return_value = 1;

//	PlaySoundObject(18, 1);

	PlayOrganyaMusic();

	gKeyTrg = gKey = 0;

	return return_value;
}
