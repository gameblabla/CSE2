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

#define MAX_OPTIONS ((WINDOW_HEIGHT / 20) - 4)	// The maximum number of options we can fit on-screen at once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct Option
{
	const char *name;
	int (*callback)(Option *options, size_t total_options, size_t selected_option, long key);
	void *user_data;
	const char *attribute;
	long value;
} Option;

typedef struct Control
{
	const char *name;
	int *scancode;
	int groups;
	char bound_name[20];
} Control;

static CONFIG conf;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

static int EnterOptionsMenu(const char *title, Option *options, size_t total_options, int x_offset, BOOL submenu)
{
	int selected_option = 0;
	int scroll = 0;

	unsigned int anime = 0;

	int return_value;

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		// Allow unpausing by pressing the pause button only when in the main pause menu (not submenus)
		if (!submenu && gKeyTrg & KEY_PAUSE)
		{
			return_value = -2;
			break;
		}

		// Go back one submenu when the 'cancel' button is pressed
		if (gKeyTrg & gKeyCancel)
		{
			return_value = -1;
			break;
		}

		// Handling up/down input
		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			const int old_selection = selected_option;

			if (gKeyTrg & gKeyDown)
				if (selected_option++ == total_options - 1)
					selected_option = 0;

			if (gKeyTrg & gKeyUp)
				if (selected_option-- == 0)
					selected_option = total_options - 1;

			// Update the menu-scrolling, if there are more options than can be fit on the screen
			if (selected_option < old_selection)
				scroll = MAX(0, MIN(scroll, selected_option - 1));

			if (selected_option > old_selection)
				scroll = MIN(total_options - MAX_OPTIONS, MAX(scroll, selected_option - (MAX_OPTIONS - 2)));

			PlaySoundObject(1, 1);
		}

		// Run option callback if OK/left/right buttons are pressed
		if (gKeyTrg & (gKeyOk | gKeyLeft | gKeyRight))
		{
			return_value = options[selected_option].callback(options, total_options, selected_option, gKeyTrg);

			// If the callback returned -1, it's time to exit this submenu
			if (return_value != -1)
				break;
		}

		// Update Quote animation counter
		if (++anime >= 40)
			anime = 0;

		// Draw screen
		CortBox(&grcFull, 0x000000);

		PutText((WINDOW_WIDTH / 2) - ((strlen(title) * 5) / 2), 20, title, RGB(0xFF, 0xFF, 0xFF));

		const size_t visible_options = MIN(MAX_OPTIONS, total_options);

		for (int i = scroll; i < scroll + visible_options; ++i)
		{
			const int x = (WINDOW_WIDTH / 2) + x_offset;
			const int y = (WINDOW_HEIGHT / 2) + (10 * 0) - (((visible_options - 1) * 20) / 2) + ((i - scroll) * 20);

			// Draw Quote next to the selected option
			if (i == selected_option)
				PutBitmap3(&grcFull, PixelToScreenCoord(x - 20), PixelToScreenCoord(y - 8), &rcMyChar[anime / 10 % 4], SURFACE_ID_MY_CHAR);

			// Draw option name
			PutText(x, y - (9 / 2), options[i].name, RGB(0xFF, 0xFF, 0xFF));

			// Draw option value, if it has one
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

	// Filter internal return values to something Cave Story can understand
	if (!submenu && (return_value == -1 || return_value == -2))
		return_value = 1;

	return return_value;
}

/********************
 * Controls menu
 ********************/

/*
 * The bitfield on the right determines which 'group' the
 * control belongs to - if two controls are in the same group,
 * they cannot be bound to the same key.
 */
static Control controls[] = {
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

static int Callback_KeyRebind(Option *options, size_t total_options, size_t selected_option, long key)
{
	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(5, 1);

	int total_keys;
	const Uint8 *state = SDL_GetKeyboardState(&total_keys);

	Uint8 *old_state = (Uint8*)malloc(total_keys);

	memcpy(old_state, state, total_keys);

	for (;;)
	{
		for (int scancode = 0; scancode < total_keys; ++scancode)
		{
			if (((old_state[scancode] ^ state[scancode]) & state[scancode]) == 1)
			{
				const char *key_name = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)scancode));

				// If another key in the game group uses this key, swap them
				for (size_t other_option = 0; other_option < total_options; ++other_option)
				{
					if (other_option != selected_option && controls[other_option].groups & controls[selected_option].groups && options[other_option].value == scancode)
					{
						*controls[other_option].scancode = *controls[selected_option].scancode;
						*controls[selected_option].scancode = scancode;

						conf.key_bindings[other_option] = *controls[other_option].scancode;
						conf.key_bindings[selected_option] = *controls[selected_option].scancode;

						memcpy(controls[other_option].bound_name, controls[selected_option].bound_name, sizeof(controls[0].bound_name));
						strncpy(controls[selected_option].bound_name, key_name, sizeof(controls[0].bound_name) - 1);

						PlaySoundObject(18, 1);
						free(old_state);

						gKeyTrg = gKey = 0;	// Prevent weird key-ghosting by doing this
						return -1;
					}
				}

				// Otherwise just overwrite the selected key
				strncpy(controls[selected_option].bound_name, key_name, sizeof(controls[0].bound_name) - 1);

				*controls[selected_option].scancode = scancode;
				conf.key_bindings[selected_option] = *controls[selected_option].scancode;

				PlaySoundObject(18, 1);
				free(old_state);

				gKeyTrg = gKey = 0;	// Prevent weird key-ghosting by doing this
				return -1;
			}
		}

		memcpy(old_state, state, total_keys);

		// Draw screen
		CortBox(&grcFull, 0x000000);

		const char *string = "Press a key to bind to this action:";
		PutText((WINDOW_WIDTH / 2) - ((strlen(string) * 5) / 2), (WINDOW_HEIGHT / 2) - 10, string, RGB(0xFF, 0xFF, 0xFF));
		PutText((WINDOW_WIDTH / 2) - ((strlen(options[selected_option].name) * 5) / 2), (WINDOW_HEIGHT / 2) + 10, options[selected_option].name, RGB(0xFF, 0xFF, 0xFF));

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			free(old_state);
			return 0;
		}
	}
}

static int Callback_Controls(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

	if (!(key & gKeyOk))
		return -1;

	Option submenu_options[sizeof(controls) / sizeof(controls[0])];

	for (size_t i = 0; i < sizeof(controls) / sizeof(controls[0]); ++i)
	{
		submenu_options[i].name = controls[i].name;
		submenu_options[i].callback = Callback_KeyRebind;
		submenu_options[i].attribute = controls[i].bound_name;

		strncpy(controls[i].bound_name, SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)*controls[i].scancode)), sizeof(controls[0].bound_name) - 1);
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu("CONTROLS", submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -60, TRUE);

	PlaySoundObject(5, 1);

	return return_value;
}

/********************
 * Options menu
 ********************/

static int Callback_Framerate(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)total_options;
	(void)key;

	const char *strings[] = {"50FPS", "60FPS"};

	options[selected_option].value = (options[selected_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));
	
	gb60fps = options[selected_option].value;

	options[selected_option].attribute = strings[options[selected_option].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Vsync(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)total_options;
	(void)key;

	const char *strings[] = {"Off (needs restart)", "On (needs restart)"};

	options[selected_option].value = (options[selected_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

	options[selected_option].attribute = strings[options[selected_option].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Resolution(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)total_options;

	const char *strings[] = {"Fullscreen (needs restart)", "Windowed 426x240 (needs restart)", "Windowed 852x480 (needs restart)", "Windowed 1278x720 (needs restart)", "Windowed 1704x960 (needs restart)"};

	if (key & gKeyLeft)
		--options[selected_option].value;
	else
		++options[selected_option].value;

	options[selected_option].value %= (sizeof(strings) / sizeof(strings[0]));

	gb60fps = options[selected_option].value;

	options[selected_option].attribute = strings[options[selected_option].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_Options(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

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

	const int return_value = EnterOptionsMenu("OPTIONS", submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -70, TRUE);

	PlaySoundObject(5, 1);

	conf.b60fps = submenu_options[1].value;
	conf.bVsync = submenu_options[2].value;
	conf.display_mode = submenu_options[3].value;

	SaveConfigData(&conf);

	return return_value;
}

/********************
 * Pause menu
 ********************/

static int Callback_Resume(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 1;
}

static int Callback_Reset(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return 2;
}

static int Callback_Quit(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

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

	int return_value = EnterOptionsMenu("PAUSED", options, sizeof(options) / sizeof(options[0]), -14, FALSE);

	gKeyTrg = gKey = 0;

	return return_value;
}
