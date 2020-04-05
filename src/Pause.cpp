#include "Pause.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Backends/Controller.h"
#include "Backends/Misc.h"
#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "Escape.h"
#include "Input.h"
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

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

static const char* GetKeyName(int key)
{
	switch (key)
	{
		case BACKEND_KEYBOARD_A:
			return "A";

		case BACKEND_KEYBOARD_B:
			return "B";

		case BACKEND_KEYBOARD_C:
			return "C";

		case BACKEND_KEYBOARD_D:
			return "D";

		case BACKEND_KEYBOARD_E:
			return "E";

		case BACKEND_KEYBOARD_F:
			return "F";

		case BACKEND_KEYBOARD_G:
			return "G";

		case BACKEND_KEYBOARD_H:
			return "H";

		case BACKEND_KEYBOARD_I:
			return "I";

		case BACKEND_KEYBOARD_J:
			return "J";

		case BACKEND_KEYBOARD_K:
			return "K";

		case BACKEND_KEYBOARD_L:
			return "L";

		case BACKEND_KEYBOARD_M:
			return "M";

		case BACKEND_KEYBOARD_N:
			return "N";

		case BACKEND_KEYBOARD_O:
			return "O";

		case BACKEND_KEYBOARD_P:
			return "P";

		case BACKEND_KEYBOARD_Q:
			return "Q";

		case BACKEND_KEYBOARD_R:
			return "R";

		case BACKEND_KEYBOARD_S:
			return "S";

		case BACKEND_KEYBOARD_T:
			return "T";

		case BACKEND_KEYBOARD_U:
			return "U";

		case BACKEND_KEYBOARD_V:
			return "V";

		case BACKEND_KEYBOARD_W:
			return "W";

		case BACKEND_KEYBOARD_X:
			return "X";

		case BACKEND_KEYBOARD_Y:
			return "Y";

		case BACKEND_KEYBOARD_Z:
			return "Z";

		case BACKEND_KEYBOARD_0:
			return "0";

		case BACKEND_KEYBOARD_1:
			return "1";

		case BACKEND_KEYBOARD_2:
			return "2";

		case BACKEND_KEYBOARD_3:
			return "3";

		case BACKEND_KEYBOARD_4:
			return "4";

		case BACKEND_KEYBOARD_5:
			return "5";

		case BACKEND_KEYBOARD_6:
			return "6";

		case BACKEND_KEYBOARD_7:
			return "7";

		case BACKEND_KEYBOARD_8:
			return "8";

		case BACKEND_KEYBOARD_9:
			return "9";

		case BACKEND_KEYBOARD_F1:
			return "F1";

		case BACKEND_KEYBOARD_F2:
			return "F2";

		case BACKEND_KEYBOARD_F3:
			return "F3";

		case BACKEND_KEYBOARD_F4:
			return "F4";

		case BACKEND_KEYBOARD_F5:
			return "F5";

		case BACKEND_KEYBOARD_F6:
			return "F6";

		case BACKEND_KEYBOARD_F7:
			return "F7";

		case BACKEND_KEYBOARD_F8:
			return "F8";

		case BACKEND_KEYBOARD_F9:
			return "F9";

		case BACKEND_KEYBOARD_F10:
			return "F10";

		case BACKEND_KEYBOARD_F11:
			return "F11";

		case BACKEND_KEYBOARD_F12:
			return "F12";

		case BACKEND_KEYBOARD_UP:
			return "Up";

		case BACKEND_KEYBOARD_DOWN:
			return "Down";

		case BACKEND_KEYBOARD_LEFT:
			return "Left";

		case BACKEND_KEYBOARD_RIGHT:
			return "Right";

		case BACKEND_KEYBOARD_ESCAPE:
			return "Escape";

		case BACKEND_KEYBOARD_BACK_QUOTE:
			return "Back Quote";

		case BACKEND_KEYBOARD_TAB:
			return "Tab";

		case BACKEND_KEYBOARD_CAPS_LOCK:
			return "Caps Lock";

		case BACKEND_KEYBOARD_LEFT_SHIFT:
			return "Left Shift";

		case BACKEND_KEYBOARD_LEFT_CTRL:
			return "Left Ctrl";

		case BACKEND_KEYBOARD_LEFT_ALT:
			return "Left Alt";

		case BACKEND_KEYBOARD_SPACE:
			return "Space";

		case BACKEND_KEYBOARD_RIGHT_ALT:
			return "Right Alt";

		case BACKEND_KEYBOARD_RIGHT_CTRL:
			return "Right Ctrl";

		case BACKEND_KEYBOARD_RIGHT_SHIFT:
			return "Right Shift";

		case BACKEND_KEYBOARD_ENTER:
			return "Enter";

		case BACKEND_KEYBOARD_BACKSPACE:
			return "Backspace";

		case BACKEND_KEYBOARD_MINUS:
			return "-";

		case BACKEND_KEYBOARD_EQUALS:
			return "=";

		case BACKEND_KEYBOARD_LEFT_BRACKET:
			return "[";

		case BACKEND_KEYBOARD_RIGHT_BRACKET:
			return "]";

		case BACKEND_KEYBOARD_BACK_SLASH:
			return "\\";

		case BACKEND_KEYBOARD_SEMICOLON:
			return ";";

		case BACKEND_KEYBOARD_APOSTROPHE:
			return "'";

		case BACKEND_KEYBOARD_COMMA:
			return ",";

		case BACKEND_KEYBOARD_PERIOD:
			return ".";

		case BACKEND_KEYBOARD_FORWARD_SLASH:
			return "/";
	}

	return "Unknown";
}

static int EnterOptionsMenu(const char *title, Option *options, size_t total_options, int x_offset, BOOL submenu)
{
	size_t selected_option = 0;
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
			return_value = enum_ESCRETURN_continue;
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
			const size_t old_selection = selected_option;

			if (gKeyTrg & gKeyDown)
				if (selected_option++ == total_options - 1)
					selected_option = 0;

			if (gKeyTrg & gKeyUp)
				if (selected_option-- == 0)
					selected_option = total_options - 1;

			// Update the menu-scrolling, if there are more options than can be fit on the screen
			if (selected_option < old_selection)
				scroll = MAX(0, MIN(scroll, (int)selected_option - 1));

			if (selected_option > old_selection)
				scroll = MIN(MAX(0, (int)total_options - MAX_OPTIONS), MAX(scroll, (int)selected_option - (MAX_OPTIONS - 2)));

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

		for (size_t i = scroll; i < scroll + visible_options; ++i)
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
			return_value = enum_ESCRETURN_exit;
			break;
		}
	}

	// Filter internal return values to something Cave Story can understand
	if (!submenu && return_value == -1)
		return_value = enum_ESCRETURN_continue;

	return return_value;
}

/********************
 * Controls menu
 ********************/

typedef struct Control
{
	const char *name;
	size_t binding_index;
	unsigned char groups;
} Control;

/*
 * The bitfield on the right determines which 'group' the
 * control belongs to - if two controls are in the same group,
 * they cannot be bound to the same key.
 */
static const Control controls[] = {
	{"Up", BINDING_UP, (1 << 0) | (1 << 1)},
	{"Down", BINDING_DOWN, (1 << 0) | (1 << 1)},
	{"Left", BINDING_LEFT, (1 << 0) | (1 << 1)},
	{"Right", BINDING_RIGHT, (1 << 0) | (1 << 1)},
	{"OK", BINDING_OK, 1 << 1},
	{"Cancel", BINDING_CANCEL, 1 << 1},
	{"Jump", BINDING_JUMP, 1 << 0},
	{"Shoot", BINDING_SHOT, 1 << 0},
	{"Next Weapon", BINDING_ARMS, 1 << 0},
	{"Previous Weapon", BINDING_ARMSREV, 1 << 0},
	{"Inventory", BINDING_ITEM, 1 << 0},
	{"Map", BINDING_MAP, 1 << 0},
	{"Pause", BINDING_PAUSE, 1 << 0}
};

static char bound_name_buffers[sizeof(controls) / sizeof(controls[0])][20];

static int Callback_ControlsKeyboard_Rebind(Option *options, size_t total_options, size_t selected_option, long key)
{
	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(5, 1);

	char timeout_string[2];
	timeout_string[1] = '\0';

	for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
	{
		for (int scancode = 0; scancode < BACKEND_KEYBOARD_TOTAL; ++scancode)
		{
			if (!backend_previous_keyboard_state[scancode] && backend_keyboard_state[scancode])
			{
				const char *key_name = GetKeyName(scancode);

				// If another control in the same group uses this key, swap them
				for (size_t other_option = 0; other_option < total_options; ++other_option)
				{
					if (other_option != selected_option && controls[other_option].groups & controls[selected_option].groups && bindings[controls[other_option].binding_index].keyboard == scancode)
					{
						bindings[controls[other_option].binding_index].keyboard = bindings[controls[selected_option].binding_index].keyboard;
						memcpy(bound_name_buffers[other_option], bound_name_buffers[selected_option], sizeof(bound_name_buffers[0]));
						break;
					}
				}

				// Otherwise just overwrite the selected control
				bindings[controls[selected_option].binding_index].keyboard = scancode;
				strncpy(bound_name_buffers[selected_option], key_name, sizeof(bound_name_buffers[0]) - 1);

				PlaySoundObject(18, 1);

				gKeyTrg = gKey = 0;	// Prevent weird input-ghosting by doing this
				return -1;
			}
		}

		// Draw screen
		CortBox(&grcFull, 0x000000);

		const char *string = "Press a key to bind to this action:";
		PutText((WINDOW_WIDTH / 2) - ((strlen(string) * 5) / 2), (WINDOW_HEIGHT / 2) - 10, string, RGB(0xFF, 0xFF, 0xFF));
		PutText((WINDOW_WIDTH / 2) - ((strlen(options[selected_option].name) * 5) / 2), (WINDOW_HEIGHT / 2) + 10, options[selected_option].name, RGB(0xFF, 0xFF, 0xFF));

		timeout_string[0] = '0' + (timeout / 60) + 1;
		PutText((WINDOW_WIDTH / 2) - (5 / 2), (WINDOW_HEIGHT / 2) + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			return enum_ESCRETURN_exit;
		}
	}

	return -1;
}

static int Callback_ControlsKeyboard(Option *options, size_t total_options, size_t selected_option, long key)
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
		submenu_options[i].callback = Callback_ControlsKeyboard_Rebind;
		submenu_options[i].attribute = bound_name_buffers[i];

		strncpy(bound_name_buffers[i], GetKeyName(bindings[controls[i].binding_index].keyboard), sizeof(bound_name_buffers[0]) - 1);
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu("CONTROLS (KEYBOARD)", submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -60, TRUE);

	PlaySoundObject(5, 1);

	return return_value;
}

static int Callback_ControlsController_Rebind(Option *options, size_t total_options, size_t selected_option, long key)
{
	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(5, 1);

	JOYSTICK_STATUS old_state;
	if (!GetJoystickStatus(&old_state))
		memset(&old_state, 0, sizeof(JOYSTICK_STATUS));

	char timeout_string[2];
	timeout_string[1] = '\0';

	for (unsigned int timeout = (60 * 5) - 1; timeout != 0; --timeout)
	{
		JOYSTICK_STATUS state;
		if (!GetJoystickStatus(&state))
			memset(&state, 0, sizeof(JOYSTICK_STATUS));

		for (int button = 0; button < sizeof(state.bButton) / sizeof(state.bButton[0]); ++button)
		{
			if (!old_state.bButton[button] && state.bButton[button])
			{
				// If another control in the same group uses this button, swap them
				for (size_t other_option = 0; other_option < total_options; ++other_option)
				{
					if (other_option != selected_option && controls[other_option].groups & controls[selected_option].groups && bindings[controls[other_option].binding_index].controller == button)
					{
						bindings[controls[other_option].binding_index].controller = bindings[controls[selected_option].binding_index].controller;
						memcpy(bound_name_buffers[other_option], bound_name_buffers[selected_option], sizeof(bound_name_buffers[0]));
						break;
					}
				}

				// Otherwise just overwrite the selected control
				bindings[controls[selected_option].binding_index].controller = button;
				snprintf(bound_name_buffers[selected_option], sizeof(bound_name_buffers[0]), "Button %d", button);

				PlaySoundObject(18, 1);

				gKeyTrg = gKey = 0;	// Prevent weird input-ghosting by doing this
				return -1;
			}
		}

		old_state = state;

		// Draw screen
		CortBox(&grcFull, 0x000000);

		const char *string = "Press a button to bind to this action:";
		PutText((WINDOW_WIDTH / 2) - ((strlen(string) * 5) / 2), (WINDOW_HEIGHT / 2) - 10, string, RGB(0xFF, 0xFF, 0xFF));
		PutText((WINDOW_WIDTH / 2) - ((strlen(options[selected_option].name) * 5) / 2), (WINDOW_HEIGHT / 2) + 10, options[selected_option].name, RGB(0xFF, 0xFF, 0xFF));

		timeout_string[0] = '0' + (timeout / 60) + 1;
		PutText((WINDOW_WIDTH / 2) - (5 / 2), (WINDOW_HEIGHT / 2) + 60, timeout_string, RGB(0xFF, 0xFF, 0xFF));

		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			return enum_ESCRETURN_exit;
		}
	}

	return -1;
}

static int Callback_ControlsController(Option *options, size_t total_options, size_t selected_option, long key)
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
		submenu_options[i].callback = Callback_ControlsController_Rebind;
		submenu_options[i].attribute = bound_name_buffers[i];

		if (bindings[controls[i].binding_index].controller == 0xFF)
			strncpy(bound_name_buffers[i], "[Unbound]", sizeof(bound_name_buffers[0]));
		else
			snprintf(bound_name_buffers[i], sizeof(bound_name_buffers[0]), "Button %d", bindings[controls[i].binding_index].controller);
	}

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu("CONTROLS (GAMEPAD)", submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -70, TRUE);

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
	{
		if (--options[selected_option].value < 0)
			options[selected_option].value = (sizeof(strings) / sizeof(strings[0])) - 1;
	}
	else
	{
		if (++options[selected_option].value > (sizeof(strings) / sizeof(strings[0])) - 1)
			options[selected_option].value = 0;
	}

	gb60fps = options[selected_option].value;

	options[selected_option].attribute = strings[options[selected_option].value];

	PlaySoundObject(SND_SWITCH_WEAPON, 1);
	return -1;
}

static int Callback_SmoothScrolling(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)total_options;
	(void)key;

	const char *strings[] = {"Off", "On"};

	options[selected_option].value = (options[selected_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

	gbSmoothScrolling = options[selected_option].value;

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
		{"Controls (Keyboard)", Callback_ControlsKeyboard, NULL, NULL, 0},
		{"Controls (Gamepad)", Callback_ControlsController, NULL, NULL, 0},
		{"Framerate", Callback_Framerate, NULL, NULL, 0},
		{"V-sync", Callback_Vsync, NULL, NULL, 0},
		{"Resolution", Callback_Resolution, NULL, NULL, 0},
		{"Smooth Scrolling", Callback_SmoothScrolling, NULL, NULL, 0}
	};

	CONFIG conf;
	if (!LoadConfigData(&conf))
		DefaultConfigData(&conf);

	submenu_options[2].value = conf.b60fps;
	submenu_options[2].attribute = conf.b60fps ? "60FPS" : "50FPS";

	submenu_options[3].value = conf.bVsync;
	submenu_options[3].attribute = conf.bVsync ? "On" : "Off";

	submenu_options[4].value = conf.display_mode;
	switch (conf.display_mode)
	{
		case 0:
			submenu_options[4].attribute = "Fullscreen";
			break;

		case 1:
			submenu_options[4].attribute = "Windowed 426x240";
			break;

		case 2:
			submenu_options[4].attribute = "Windowed 852x480";
			break;

		case 3:
			submenu_options[4].attribute = "Windowed 1278x720";
			break;

		case 4:
			submenu_options[4].attribute = "Windowed 1704x960";
			break;
	}

	submenu_options[5].value = conf.bSmoothScrolling;
	submenu_options[5].attribute = conf.bSmoothScrolling ? "On" : "Off";

	PlaySoundObject(5, 1);

	const int return_value = EnterOptionsMenu("OPTIONS", submenu_options, sizeof(submenu_options) / sizeof(submenu_options[0]), -70, TRUE);

	PlaySoundObject(5, 1);

	conf.b60fps = submenu_options[2].value;
	conf.bVsync = submenu_options[3].value;
	conf.display_mode = submenu_options[4].value;
	conf.bSmoothScrolling = submenu_options[5].value;

	memcpy(conf.bindings, bindings, sizeof(bindings));

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
	return enum_ESCRETURN_continue;
}

static int Callback_Reset(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

	if (!(key & gKeyOk))
		return -1;

	PlaySoundObject(18, 1);
	return enum_ESCRETURN_restart;
}

static int Callback_Quit(Option *options, size_t total_options, size_t selected_option, long key)
{
	(void)options;
	(void)total_options;
	(void)selected_option;

	if (!(key & gKeyOk))
		return -1;

	return enum_ESCRETURN_exit;
}

int Call_Pause(void)
{
	Option options[] = {
		{"Resume", Callback_Resume, NULL, NULL, 0},
		{"Reset", Callback_Reset, NULL, NULL, 0},
		{"Options", Callback_Options, NULL, NULL, 0},
		{"Quit", Callback_Quit, NULL, NULL, 0}
	};

	int return_value = EnterOptionsMenu("PAUSED", options, sizeof(options) / sizeof(options[0]), -14, FALSE);

	gKeyTrg = gKey = 0;

	return return_value;
}
