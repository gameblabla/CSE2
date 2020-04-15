#include "../Misc.h"

#include <string.h>

#include <coreinit/thread.h>
#include <whb/proc.h>
#include <whb/sdcard.h>

static unsigned long tick_delta;

bool Backend_Init(void)
{
	WHBProcInit();

	if (!WHBMountSdCard())
		return FALSE;

	tick_delta = OSGetSystemInfo()->busClockSpeed / 4;

	return true;
}

void Backend_Deinit(void)
{
	WHBUnmountSdCard();

	WHBProcShutdown();
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(char *string_buffer)
{
	strcpy(string_buffer, WHBGetSdCardMountPath());
	strcat(string_buffer, "/CSE2-enhanced");

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

void Backend_GetDisplayMode(Backend_DisplayMode *display_mode)
{
	display_mode->width = 854;
	display_mode->height = 480;
	display_mode->refresh_rate = 60;
}
