// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Misc.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <coreinit/thread.h>
#include <padscore/kpad.h>
#include <vpad/input.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/proc.h>
#include <whb/sdcard.h>

static unsigned long ticks_per_second;

bool Backend_Init(void (*drag_and_drop_callback)(const char *path), void (*window_focus_callback)(bool focus))
{
	(void)drag_and_drop_callback;
	(void)window_focus_callback;

	WHBProcInit();

	if (!WHBMountSdCard())
		return FALSE;

	VPADInit();

	WPADInit();
	KPADInit();

	// Enable Wii U Pro Controllers to be connected
	WPADEnableURCC(1);

	WHBLogUdpInit();

	ticks_per_second = OSGetSystemInfo()->busClockSpeed / 4;

	return true;
}

void Backend_Deinit(void)
{
	WHBLogUdpDeinit();

	WPADShutdown();

	VPADShutdown();

	WHBUnmountSdCard();

	WHBProcShutdown();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetPaths(std::string *module_path, std::string *data_path)
{
	*module_path = WHBGetSdCardMountPath();
#ifdef JAPANESE
	*module_path += "/CSE2-portable-jp";
#else
	*module_path += "/CSE2-portable-en";
#endif

	*data_path = *module_path + "/data";

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
	keyboard_state[BACKEND_KEYBOARD_Z] |= vpad_buttons & VPAD_BUTTON_B;                       // Jump
	keyboard_state[BACKEND_KEYBOARD_X] |= vpad_buttons & VPAD_BUTTON_Y;                       // Shoot
	keyboard_state[BACKEND_KEYBOARD_Q] |= vpad_buttons & (VPAD_BUTTON_A | VPAD_BUTTON_PLUS);  // Inventory
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
	keyboard_state[BACKEND_KEYBOARD_Z] |= kpad_buttons & WPAD_PRO_BUTTON_B;                           // Jump
	keyboard_state[BACKEND_KEYBOARD_X] |= kpad_buttons & WPAD_PRO_BUTTON_Y;                           // Shoot
	keyboard_state[BACKEND_KEYBOARD_Q] |= kpad_buttons & (WPAD_PRO_BUTTON_A | WPAD_PRO_BUTTON_PLUS);  // Inventory
	keyboard_state[BACKEND_KEYBOARD_W] |= kpad_buttons & (WPAD_PRO_BUTTON_X | WPAD_PRO_BUTTON_MINUS); // Map
	keyboard_state[BACKEND_KEYBOARD_A] |= kpad_buttons & (WPAD_PRO_TRIGGER_L | WPAD_PRO_TRIGGER_ZL | WPAD_PRO_STICK_R_EMULATION_LEFT);  // Weapon left
	keyboard_state[BACKEND_KEYBOARD_S] |= kpad_buttons & (WPAD_PRO_TRIGGER_R | WPAD_PRO_TRIGGER_ZR | WPAD_PRO_STICK_R_EMULATION_RIGHT); // Weapon right
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	Backend_PrintInfo("ShowMessageBox - %s - %s", title, message);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintError(const char *format, ...)
{
	char message_buffer[0x100];

	va_list argument_list;
	va_start(argument_list, format);
	vsnprintf(message_buffer, sizeof(message_buffer), format, argument_list);
	va_end(argument_list);

	WHBLogPrint("ERROR:");
	WHBLogPrint(message_buffer);
}

ATTRIBUTE_FORMAT_PRINTF(1, 2) void Backend_PrintInfo(const char *format, ...)
{
	char message_buffer[0x100];

	va_list argument_list;
	va_start(argument_list, format);
	vsnprintf(message_buffer, sizeof(message_buffer), format, argument_list);
	va_end(argument_list);

	WHBLogPrint("INFO:");
	WHBLogPrint(message_buffer);
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
