#include "../Misc.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <coreinit/thread.h>
#include <whb/log.h>
#include <whb/log_udp.h>
#include <whb/proc.h>
#include <whb/sdcard.h>

static unsigned long ticks_per_second;

bool Backend_Init(void)
{
	WHBProcInit();

	if (!WHBMountSdCard())
		return FALSE;

	WHBLogUdpInit();

	ticks_per_second = OSGetSystemInfo()->busClockSpeed / 4;

	return true;
}

void Backend_Deinit(void)
{
	WHBLogUdpDeinit();

	WHBUnmountSdCard();

	WHBProcShutdown();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(std::string *string_buffer)
{
	*string_buffer = WHBGetSdCardMountPath();
#ifdef JAPANESE
	*string_buffer += "/CSE2-enhanced-lite-jp";
#else
	*string_buffer += "/CSE2-enhanced-lite-en";
#endif

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

void Backend_GetDisplayMode(Backend_DisplayMode *display_mode)
{
	display_mode->width = 854;
	display_mode->height = 480;
	display_mode->refresh_rate = 60;
}

bool Backend_IsConsole(void)
{
	return true;
}
