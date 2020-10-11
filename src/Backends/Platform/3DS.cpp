#include "../Misc.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <3ds.h>

bool Backend_Init(void (*drag_and_drop_callback)(const char *path), void (*window_focus_callback)(bool focus))
{
	(void)drag_and_drop_callback;
	(void)window_focus_callback;

	hidInit();

	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	Result rc = romfsInit();

	if (rc == 0)
	{
		osSetSpeedupEnable(true); // Enable New3DS speedup, since this doesn't run very well on Old3DSs yet

		return true;
	}
	else
	{
		Backend_PrintError("Could not initialise romfs");
	}

	return false;
}

void Backend_Deinit(void)
{
	romfsExit();

	gfxExit();

	hidExit();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(std::string *string_buffer)
{
	*string_buffer = "romfs:";

	return true;
}

void Backend_HideMouse(void)
{
	
}

void Backend_SetWindowIcon(const unsigned char *rgb_pixels, size_t width, size_t height)
{
	(void)rgb_pixels;
	(void)width;
	(void)height;
}

void Backend_SetCursor(const unsigned char *rgba_pixels, size_t width, size_t height)
{
	(void)rgba_pixels;
	(void)width;
	(void)height;
}

void Backend_EnableDragAndDrop(void)
{
	
}

bool Backend_SystemTask(bool active)
{
	(void)active;

	return aptMainLoop();
}

void Backend_GetKeyboardState(bool *keyboard_state)
{
	memset(keyboard_state, 0, sizeof(bool) * BACKEND_KEYBOARD_TOTAL);

	// Read controller
	hidScanInput();

	u32 buttons = hidKeysHeld();

	keyboard_state[BACKEND_KEYBOARD_UP] |= buttons & KEY_UP;
	keyboard_state[BACKEND_KEYBOARD_DOWN] |= buttons & KEY_DOWN;
	keyboard_state[BACKEND_KEYBOARD_LEFT] |= buttons & KEY_LEFT;
	keyboard_state[BACKEND_KEYBOARD_RIGHT] |= buttons & KEY_RIGHT;
	keyboard_state[BACKEND_KEYBOARD_Z] |= buttons & KEY_B;                               // Jump
	keyboard_state[BACKEND_KEYBOARD_X] |= buttons & KEY_Y;                               // Shoot
	keyboard_state[BACKEND_KEYBOARD_Q] |= buttons & (KEY_A | KEY_START);                 // Inventory
	keyboard_state[BACKEND_KEYBOARD_W] |= buttons & (KEY_X | KEY_SELECT);                // Map
	keyboard_state[BACKEND_KEYBOARD_A] |= buttons & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT);  // Weapon left
	keyboard_state[BACKEND_KEYBOARD_S] |= buttons & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT); // Weapon right
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	Backend_PrintInfo("ShowMessageBox - %s - %s", title, message);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char *format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("ERROR: ", stderr);
	vfprintf(stderr, format, argumentList);
	fputc('\n', stderr);
	va_end(argumentList);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char *format, ...)
{
	va_list argumentList;
	va_start(argumentList, format);
	fputs("INFO: ", stderr);
	vfprintf(stderr, format, argumentList);
	fputc('\n', stderr);
	va_end(argumentList);
}

unsigned long Backend_GetTicks(void)
{
	return svcGetSystemTick() / CPU_TICKS_PER_MSEC;
}

void Backend_Delay(unsigned int ticks)
{
	// svcSleepThread measures in nanoseconds
	svcSleepThread(ticks * 1000000);
}
