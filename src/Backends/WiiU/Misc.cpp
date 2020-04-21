#include "../Misc.h"

#include <string.h>

#include <coreinit/thread.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <whb/proc.h>
#include <whb/sdcard.h>

static unsigned long ticks_per_second;

bool Backend_Init(void)
{
	WHBProcInit();

	if (!WHBMountSdCard())
		return FALSE;

	VPADInit();

	WPADInit();
	KPADInit();

	// Enable Wii U Pro Controllers to be connected
	WPADEnableURCC(1);

	ticks_per_second = OSGetSystemInfo()->busClockSpeed / 4;

	return true;
}

void Backend_Deinit(void)
{
	WPADShutdown();

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

void Backend_GetKeyboardState(bool *keyboard_state)
{
	memset(keyboard_state, 0, sizeof(bool) * BACKEND_KEYBOARD_TOTAL);

	// Read gamepad
	static uint32_t vpad_buttons;

	VPADStatus vpad_status;
	if (VPADRead(VPAD_CHAN_0, &vpad_status, 1, NULL) == 1)
		vpad_buttons = vpad_status.hold;

	keyboard_state[BACKEND_KEYBOARD_UP] |= vpad_buttons & (VPAD_BUTTON_UP | VPAD_STICK_L_EMULATION_UP);
	keyboard_state[BACKEND_KEYBOARD_DOWN] |= vpad_buttons & (VPAD_BUTTON_DOWN | VPAD_STICK_L_EMULATION_DOWN);
	keyboard_state[BACKEND_KEYBOARD_LEFT] |= vpad_buttons & (VPAD_BUTTON_LEFT | VPAD_STICK_L_EMULATION_LEFT);
	keyboard_state[BACKEND_KEYBOARD_RIGHT] |= vpad_buttons & (VPAD_BUTTON_RIGHT | VPAD_STICK_L_EMULATION_RIGHT);
	keyboard_state[BACKEND_KEYBOARD_Z] |= vpad_buttons & VPAD_BUTTON_A;                       // Jump
	keyboard_state[BACKEND_KEYBOARD_X] |= vpad_buttons & VPAD_BUTTON_B;                       // Shoot
	keyboard_state[BACKEND_KEYBOARD_Q] |= vpad_buttons & (VPAD_BUTTON_Y | VPAD_BUTTON_PLUS);  // Inventory
	keyboard_state[BACKEND_KEYBOARD_W] |= vpad_buttons & (VPAD_BUTTON_X | VPAD_BUTTON_MINUS); // Map
	keyboard_state[BACKEND_KEYBOARD_A] |= vpad_buttons & (VPAD_BUTTON_L | VPAD_BUTTON_ZL | VPAD_STICK_R_EMULATION_LEFT);  // Weapon left
	keyboard_state[BACKEND_KEYBOARD_S] |= vpad_buttons & (VPAD_BUTTON_R | VPAD_BUTTON_ZR | VPAD_STICK_R_EMULATION_RIGHT); // Weapon right

	// Read Wii U Pro Controller
	static uint32_t kpad_buttons;

	KPADStatus kpad_status;
	if (KPADRead(WPAD_CHAN_0, &kpad_status, 1) == 1)
		kpad_buttons = kpad_status.pro.hold;

	keyboard_state[BACKEND_KEYBOARD_UP] |= kpad_buttons & (WPAD_PRO_BUTTON_UP | WPAD_PRO_STICK_L_EMULATION_UP);
	keyboard_state[BACKEND_KEYBOARD_DOWN] |= kpad_buttons & (WPAD_PRO_BUTTON_DOWN | WPAD_PRO_STICK_L_EMULATION_DOWN);
	keyboard_state[BACKEND_KEYBOARD_LEFT] |= kpad_buttons & (WPAD_PRO_BUTTON_LEFT | WPAD_PRO_STICK_L_EMULATION_LEFT);
	keyboard_state[BACKEND_KEYBOARD_RIGHT] |= kpad_buttons & (WPAD_PRO_BUTTON_RIGHT | WPAD_PRO_STICK_L_EMULATION_RIGHT);
	keyboard_state[BACKEND_KEYBOARD_Z] |= kpad_buttons & WPAD_PRO_BUTTON_A;                           // Jump
	keyboard_state[BACKEND_KEYBOARD_X] |= kpad_buttons & WPAD_PRO_BUTTON_B;                           // Shoot
	keyboard_state[BACKEND_KEYBOARD_Q] |= kpad_buttons & (WPAD_PRO_BUTTON_Y | WPAD_PRO_BUTTON_PLUS);  // Inventory
	keyboard_state[BACKEND_KEYBOARD_W] |= kpad_buttons & (WPAD_PRO_BUTTON_X | WPAD_PRO_BUTTON_MINUS); // Map
	keyboard_state[BACKEND_KEYBOARD_A] |= kpad_buttons & (WPAD_PRO_TRIGGER_L | WPAD_PRO_TRIGGER_ZL | WPAD_PRO_STICK_R_EMULATION_LEFT);  // Weapon left
	keyboard_state[BACKEND_KEYBOARD_S] |= kpad_buttons & (WPAD_PRO_TRIGGER_R | WPAD_PRO_TRIGGER_ZR | WPAD_PRO_STICK_R_EMULATION_RIGHT); // Weapon right
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	(void)title;
	(void)message;
	// TODO - We might be able to funnel this into `WHBLogPrintf`...
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char *format, ...)
{
	(void)format;
	// TODO - We might be able to funnel this into `WHBLogPrintf`...
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char *format, ...)
{
	(void)format;
	// TODO - We might be able to funnel this into `WHBLogPrintf`...
}

unsigned long Backend_GetTicks(void)
{
	static uint64_t accumulator;

	static unsigned long last_tick;

	unsigned long current_tick = OSGetTick();

	accumulator += current_tick - last_tick;

	last_tick = current_tick;

	return (accumulator * 1000) / ticks_per_second;
}

void Backend_Delay(unsigned int ticks)
{
	OSSleepTicks((ticks * ticks_per_second) / 1000);
}