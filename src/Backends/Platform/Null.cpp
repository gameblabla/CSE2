// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Misc.h"

#include <string>

#include "../../Attributes.h"

bool Backend_Init(void (*drag_and_drop_callback)(const char *path), void (*window_focus_callback)(bool focus))
{
	(void)drag_and_drop_callback;
	(void)window_focus_callback;

	return true;
}

void Backend_Deinit(void)
{
	
}

void Backend_PostWindowCreation(void)
{
	
}

bool Backend_GetPaths(std::string *module_path, std::string *data_path)
{
	(void)module_path;
	(void)data_path;

	return false;
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
