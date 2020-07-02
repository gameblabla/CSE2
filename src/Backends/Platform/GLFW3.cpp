#include "../Misc.h"

#include <chrono>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>

#include <GLFW/glfw3.h>

#include "../Rendering.h"
#include "../Shared/GLFW3.h"
#include "../../Attributes.h"
#include "../../Main.h"
#include "../../Organya.h"
#include "../../Profile.h"

#define DO_KEY(GLFW_KEY, BACKEND_KEY) \
	case GLFW_KEY: \
		keyboard_state[BACKEND_KEY] = action == GLFW_PRESS; \
		break;

static bool keyboard_state[BACKEND_KEYBOARD_TOTAL];

static GLFWcursor* cursor;

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	(void)window;
	(void)scancode;
	(void)mods;

	switch (action)
	{
		case GLFW_RELEASE:
		case GLFW_PRESS:
			switch (key)
			{
				DO_KEY(GLFW_KEY_A, BACKEND_KEYBOARD_A)
				DO_KEY(GLFW_KEY_B, BACKEND_KEYBOARD_B)
				DO_KEY(GLFW_KEY_C, BACKEND_KEYBOARD_C)
				DO_KEY(GLFW_KEY_D, BACKEND_KEYBOARD_D)
				DO_KEY(GLFW_KEY_E, BACKEND_KEYBOARD_E)
				DO_KEY(GLFW_KEY_F, BACKEND_KEYBOARD_F)
				DO_KEY(GLFW_KEY_G, BACKEND_KEYBOARD_G)
				DO_KEY(GLFW_KEY_H, BACKEND_KEYBOARD_H)
				DO_KEY(GLFW_KEY_I, BACKEND_KEYBOARD_I)
				DO_KEY(GLFW_KEY_J, BACKEND_KEYBOARD_J)
				DO_KEY(GLFW_KEY_K, BACKEND_KEYBOARD_K)
				DO_KEY(GLFW_KEY_L, BACKEND_KEYBOARD_L)
				DO_KEY(GLFW_KEY_M, BACKEND_KEYBOARD_M)
				DO_KEY(GLFW_KEY_N, BACKEND_KEYBOARD_N)
				DO_KEY(GLFW_KEY_O, BACKEND_KEYBOARD_O)
				DO_KEY(GLFW_KEY_P, BACKEND_KEYBOARD_P)
				DO_KEY(GLFW_KEY_Q, BACKEND_KEYBOARD_Q)
				DO_KEY(GLFW_KEY_R, BACKEND_KEYBOARD_R)
				DO_KEY(GLFW_KEY_S, BACKEND_KEYBOARD_S)
				DO_KEY(GLFW_KEY_T, BACKEND_KEYBOARD_T)
				DO_KEY(GLFW_KEY_U, BACKEND_KEYBOARD_U)
				DO_KEY(GLFW_KEY_V, BACKEND_KEYBOARD_V)
				DO_KEY(GLFW_KEY_W, BACKEND_KEYBOARD_W)
				DO_KEY(GLFW_KEY_X, BACKEND_KEYBOARD_X)
				DO_KEY(GLFW_KEY_Y, BACKEND_KEYBOARD_Y)
				DO_KEY(GLFW_KEY_Z, BACKEND_KEYBOARD_Z)
				DO_KEY(GLFW_KEY_0, BACKEND_KEYBOARD_0)
				DO_KEY(GLFW_KEY_1, BACKEND_KEYBOARD_1)
				DO_KEY(GLFW_KEY_2, BACKEND_KEYBOARD_2)
				DO_KEY(GLFW_KEY_3, BACKEND_KEYBOARD_3)
				DO_KEY(GLFW_KEY_4, BACKEND_KEYBOARD_4)
				DO_KEY(GLFW_KEY_5, BACKEND_KEYBOARD_5)
				DO_KEY(GLFW_KEY_6, BACKEND_KEYBOARD_6)
				DO_KEY(GLFW_KEY_7, BACKEND_KEYBOARD_7)
				DO_KEY(GLFW_KEY_8, BACKEND_KEYBOARD_8)
				DO_KEY(GLFW_KEY_9, BACKEND_KEYBOARD_9)
				DO_KEY(GLFW_KEY_F1, BACKEND_KEYBOARD_F1)
				DO_KEY(GLFW_KEY_F2, BACKEND_KEYBOARD_F2)
				DO_KEY(GLFW_KEY_F3, BACKEND_KEYBOARD_F3)
				DO_KEY(GLFW_KEY_F4, BACKEND_KEYBOARD_F4)
				DO_KEY(GLFW_KEY_F5, BACKEND_KEYBOARD_F5)
				DO_KEY(GLFW_KEY_F6, BACKEND_KEYBOARD_F6)
				DO_KEY(GLFW_KEY_F7, BACKEND_KEYBOARD_F7)
				DO_KEY(GLFW_KEY_F8, BACKEND_KEYBOARD_F8)
				DO_KEY(GLFW_KEY_F9, BACKEND_KEYBOARD_F9)
				DO_KEY(GLFW_KEY_F10, BACKEND_KEYBOARD_F10)
				DO_KEY(GLFW_KEY_F11, BACKEND_KEYBOARD_F11)
				DO_KEY(GLFW_KEY_F12, BACKEND_KEYBOARD_F12)
				DO_KEY(GLFW_KEY_UP, BACKEND_KEYBOARD_UP)
				DO_KEY(GLFW_KEY_DOWN, BACKEND_KEYBOARD_DOWN)
				DO_KEY(GLFW_KEY_LEFT, BACKEND_KEYBOARD_LEFT)
				DO_KEY(GLFW_KEY_RIGHT, BACKEND_KEYBOARD_RIGHT)
				DO_KEY(GLFW_KEY_ESCAPE, BACKEND_KEYBOARD_ESCAPE)
				DO_KEY(GLFW_KEY_GRAVE_ACCENT, BACKEND_KEYBOARD_BACK_QUOTE)
				DO_KEY(GLFW_KEY_TAB, BACKEND_KEYBOARD_TAB)
				DO_KEY(GLFW_KEY_CAPS_LOCK, BACKEND_KEYBOARD_CAPS_LOCK)
				DO_KEY(GLFW_KEY_LEFT_SHIFT, BACKEND_KEYBOARD_LEFT_SHIFT)
				DO_KEY(GLFW_KEY_LEFT_CONTROL, BACKEND_KEYBOARD_LEFT_CTRL)
				DO_KEY(GLFW_KEY_LEFT_ALT, BACKEND_KEYBOARD_LEFT_ALT)
				DO_KEY(GLFW_KEY_SPACE, BACKEND_KEYBOARD_SPACE)
				DO_KEY(GLFW_KEY_RIGHT_ALT, BACKEND_KEYBOARD_RIGHT_ALT)
				DO_KEY(GLFW_KEY_RIGHT_CONTROL, BACKEND_KEYBOARD_RIGHT_CTRL)
				DO_KEY(GLFW_KEY_RIGHT_SHIFT, BACKEND_KEYBOARD_RIGHT_SHIFT)
				DO_KEY(GLFW_KEY_ENTER, BACKEND_KEYBOARD_ENTER)
				DO_KEY(GLFW_KEY_BACKSPACE, BACKEND_KEYBOARD_BACKSPACE)
				DO_KEY(GLFW_KEY_MINUS, BACKEND_KEYBOARD_MINUS)
				DO_KEY(GLFW_KEY_EQUAL, BACKEND_KEYBOARD_EQUALS)
				DO_KEY(GLFW_KEY_LEFT_BRACKET, BACKEND_KEYBOARD_LEFT_BRACKET)
				DO_KEY(GLFW_KEY_RIGHT_BRACKET, BACKEND_KEYBOARD_RIGHT_BRACKET)
				DO_KEY(GLFW_KEY_BACKSLASH, BACKEND_KEYBOARD_BACK_SLASH)
				DO_KEY(GLFW_KEY_SEMICOLON, BACKEND_KEYBOARD_SEMICOLON)
				DO_KEY(GLFW_KEY_APOSTROPHE, BACKEND_KEYBOARD_APOSTROPHE)
				DO_KEY(GLFW_KEY_COMMA, BACKEND_KEYBOARD_COMMA)
				DO_KEY(GLFW_KEY_PERIOD, BACKEND_KEYBOARD_PERIOD)
				DO_KEY(GLFW_KEY_SLASH, BACKEND_KEYBOARD_FORWARD_SLASH)

				default:
					break;
			}

			break;
	}
}

static void WindowFocusCallback(GLFWwindow *window, int focused)
{
	(void)window;

	if (focused)
		ActiveWindow();
	else
		InactiveWindow();
}

static void WindowSizeCallback(GLFWwindow *window, int width, int height)
{
	(void)window;

	RenderBackend_HandleWindowResize(width, height);
}

static void DragAndDropCallback(GLFWwindow *window, int count, const char **paths)
{
	(void)window;
	(void)count;

	LoadProfile(paths[0]);
}

static void ErrorCallback(int code, const char *description)
{
	Backend_PrintError("GLFW error received (%d): %s", code, description);
}

bool Backend_Init(void)
{
	glfwSetErrorCallback(ErrorCallback);

	if (glfwInit() == GL_TRUE)
		return true;

	Backend_ShowMessageBox("Fatal error", "Could not initialise GLFW3");

	return false;
}

void Backend_Deinit(void)
{
	if (cursor != NULL)
		glfwDestroyCursor(cursor);

	glfwTerminate();
}

void Backend_PostWindowCreation(void)
{
	// Hook callbacks
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);
	glfwSetWindowSizeCallback(window, WindowSizeCallback);
}

bool Backend_GetBasePath(std::string *string_buffer)
{
	(void)string_buffer;

	// GLFW3 doesn't seem to have a mechanism for this
	return false;
}

void Backend_HideMouse(void)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Backend_SetWindowIcon(const unsigned char *rgb_pixels, unsigned int width, unsigned int height)
{
	// Convert to RGBA, since that's the only thing GLFW3 accepts
	unsigned char *rgba_pixels = (unsigned char*)malloc(width * height * 4);

	const unsigned char *rgb_pointer = rgb_pixels;
	unsigned char *rgba_pointer = rgba_pixels;

	if (rgba_pixels != NULL)
	{
		for (unsigned int y = 0; y < height; ++y)
		{
			for (unsigned int x = 0; x < width; ++x)
			{
				*rgba_pointer++ = *rgb_pointer++;
				*rgba_pointer++ = *rgb_pointer++;
				*rgba_pointer++ = *rgb_pointer++;
				*rgba_pointer++ = 0xFF;
			}
		}

		GLFWimage glfw_image = {(int)width, (int)height, rgba_pixels};
		glfwSetWindowIcon(window, 1, &glfw_image);

		free(rgba_pixels);
	}
}

void Backend_SetCursor(const unsigned char *rgba_pixels, unsigned int width, unsigned int height)
{
	GLFWimage glfw_image = {(int)width, (int)height, (unsigned char*)rgba_pixels};
	cursor = glfwCreateCursor(&glfw_image, 0, 0);

	if (cursor != NULL)
		glfwSetCursor(window, cursor);
}

void PlaybackBackend_EnableDragAndDrop(void)
{
	glfwSetDropCallback(window, DragAndDropCallback);
}

bool Backend_SystemTask(bool active)
{
	if (glfwWindowShouldClose(window))
	{
		StopOrganyaMusic();
		return false;
	}

	if (active)
		glfwPollEvents();
	else
		glfwWaitEvents();

	return true;
}

void Backend_GetKeyboardState(bool *out_keyboard_state)
{
	memcpy(out_keyboard_state, keyboard_state, sizeof(keyboard_state));
}

void Backend_ShowMessageBox(const char *title, const char *message)
{
	// GLFW3 doesn't have a message box
	printf("ShowMessageBox - '%s' - '%s'\n", title, message);
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
	fputs("INFO: ", stdout);
	vprintf(format, argumentList);
	putchar('\n');
	va_end(argumentList);
}

unsigned long Backend_GetTicks(void)
{
	return (unsigned long)(glfwGetTime() * 1000.0);
}

void Backend_Delay(unsigned int ticks)
{
	// GLFW3 doesn't have a delay function, so here's some butt-ugly C++11
	std::this_thread::sleep_for(std::chrono::milliseconds(ticks));
}

void Backend_GetDisplayMode(Backend_DisplayMode *display_mode)
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();

	const GLFWvidmode *mode = glfwGetVideoMode(monitor);

	display_mode->width = mode->width;
	display_mode->height = mode->height;
	display_mode->refresh_rate = mode->refreshRate;
}

bool Backend_IsConsole(void)
{
	return false;
}
