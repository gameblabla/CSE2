#include "../Misc.h"

#include <string>

#include "../../Attributes.h"

bool Backend_Init(void)
{
	return true;
}

void Backend_Deinit(void)
{
	
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetBasePath(std::string *string_buffer)
{
	(void)string_buffer;

	return false;
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

	return true;
}

void Backend_GetKeyboardState(bool *keyboard_state)
{
	(void)keyboard_state;
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
	static unsigned long fake_ticks = 0;

	fake_ticks += 1000 / 50;

	return fake_ticks;
}

void Backend_Delay(unsigned int ticks)
{
	(void)ticks;
}

void Backend_GetDisplayMode(Backend_DisplayMode *display_mode)
{
	(void)display_mode;
}

bool Backend_IsConsole(void)
{
	return false;
}
