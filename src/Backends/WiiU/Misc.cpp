#include "../Misc.h"

#include <string.h>

#include <coreinit/thread.h>
#include <vpad/input.h>
#include <whb/proc.h>
#include <whb/sdcard.h>

static unsigned long tick_delta;

bool Backend_Init(void)
{
	WHBProcInit();

	if (!WHBMountSdCard())
		return FALSE;

	VPADInit();

	tick_delta = OSGetSystemInfo()->busClockSpeed / 4;

	return true;
}

void Backend_Deinit(void)
{
	VPADShutdown();

	WHBUnmountSdCard();

	WHBProcShutdown();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(char *string_buffer)
{
	strcpy(string_buffer, WHBGetSdCardMountPath());
	strcat(string_buffer, "/CSE2-portable");

	return true;
}

void Backend_HideMouse(void)
{
	
}

void Backend_SetWindowIcon(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	(void)rgb_pixels;
	(void)width;
	(void)height;
}

void Backend_SetCursor(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	(void)rgb_pixels;
	(void)width;
	(void)height;
}

void PlaybackBackend_EnableDragAndDrop(void)
{
	
}

bool Backend_SystemTask(bool active)
{
	(void)active;

	return WHBProcIsRunning();
}

void Backend_GetKeyboardState(bool *out_keyboard_state)
{
	static bool keyboard_state[BACKEND_KEYBOARD_TOTAL];

	VPADStatus vpad_status;
	if (VPADRead(VPAD_CHAN_0, &vpad_status, 1, NULL) == 1)
	{
		keyboard_state[BACKEND_KEYBOARD_UP] = vpad_status.hold & (VPAD_BUTTON_UP | VPAD_STICK_L_EMULATION_UP);
		keyboard_state[BACKEND_KEYBOARD_DOWN] = vpad_status.hold & (VPAD_BUTTON_DOWN | VPAD_STICK_L_EMULATION_DOWN);
		keyboard_state[BACKEND_KEYBOARD_LEFT] = vpad_status.hold & (VPAD_BUTTON_LEFT | VPAD_STICK_L_EMULATION_LEFT);
		keyboard_state[BACKEND_KEYBOARD_RIGHT] = vpad_status.hold & (VPAD_BUTTON_RIGHT | VPAD_STICK_L_EMULATION_RIGHT);
		keyboard_state[BACKEND_KEYBOARD_Z] = vpad_status.hold & VPAD_BUTTON_B;                       // Jump
		keyboard_state[BACKEND_KEYBOARD_X] = vpad_status.hold & VPAD_BUTTON_Y;                       // Shoot
		keyboard_state[BACKEND_KEYBOARD_Q] = vpad_status.hold & (VPAD_BUTTON_A | VPAD_BUTTON_PLUS);  // Inventory
		keyboard_state[BACKEND_KEYBOARD_W] = vpad_status.hold & (VPAD_BUTTON_X | VPAD_BUTTON_MINUS); // Map
		keyboard_state[BACKEND_KEYBOARD_A] = vpad_status.hold & (VPAD_BUTTON_L | VPAD_BUTTON_ZL | VPAD_STICK_R_EMULATION_LEFT);  // Weapon left
		keyboard_state[BACKEND_KEYBOARD_S] = vpad_status.hold & (VPAD_BUTTON_R | VPAD_BUTTON_ZR | VPAD_STICK_R_EMULATION_RIGHT); // Weapon right
	}

	memcpy(out_keyboard_state, keyboard_state, sizeof(keyboard_state));
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	(void)title;
	(void)message;
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char *format, ...)
{
	(void)format;
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char *format, ...)
{
	(void)format;
}

unsigned long Backend_GetTicks(void)
{
	static uint64_t accumulator;

	static unsigned long last_tick;

	unsigned long current_tick = OSGetTick();

	accumulator += current_tick - last_tick;

	last_tick = current_tick;

	return (accumulator * 1000) / tick_delta;
}

void Backend_Delay(unsigned int ticks)
{
	OSSleepTicks((ticks * tick_delta) / 1000);
}
