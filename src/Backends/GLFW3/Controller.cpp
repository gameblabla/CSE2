#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../WindowsWrapper.h"

#define DEADZONE (10000.0f / 32767.0f)

static BOOL joystick_connected;
static int connected_joystick_id;
static int joystick_neutral_x;
static int joystick_neutral_y;

static void JoystickCallback(int joystick_id, int event)
{
	switch (event)
	{
		case GLFW_CONNECTED:
			printf("Joystick #%d connected - %s\n", joystick_id, glfwGetJoystickName(joystick_id));

			if (!joystick_connected)
			{
				int total_axis;
				const float *axis = glfwGetJoystickAxes(joystick_id, &total_axis);

				if (total_axis >= 2)
				{
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
					if (glfwJoystickIsGamepad(joystick_id) == GLFW_TRUE)	// Avoid selecting things like laptop touchpads
#endif
					{
						printf("Joystick #%d selected\n", joystick_id);
						joystick_connected = TRUE;
						connected_joystick_id = joystick_id;

						// Reset default stick positions (this is performed in ResetJoystickStatus in vanilla Cave Story
						joystick_neutral_x = axis[0];
						joystick_neutral_y = axis[1];
					}
				}
			}

			break;

		case GLFW_DISCONNECTED:
			if (joystick_connected && joystick_id == connected_joystick_id)
			{
				printf("Joystick #%d disconnected\n", connected_joystick_id);
				joystick_connected = FALSE;
			}

			break;
	}
}

BOOL ControllerBackend_Init(void)
{
	// Connect joysticks that are already plugged-in
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
		if (glfwJoystickPresent(i) == GLFW_TRUE)
			JoystickCallback(i, GLFW_CONNECTED);

	// Set-up the callback for future (dis)connections
	glfwSetJoystickCallback(JoystickCallback);

	return TRUE;
}

void ControllerBackend_Deinit(void)
{
	glfwSetJoystickCallback(NULL);

	joystick_connected = FALSE;
	connected_joystick_id = 0;
	joystick_neutral_x = 0;
	joystick_neutral_y = 0;
}

BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (!joystick_connected)
		return FALSE;

	// Read axis
	int total_axis;
	const float *axis = glfwGetJoystickAxes(connected_joystick_id, &total_axis);

	status->bLeft = axis[0] < joystick_neutral_x - DEADZONE;
	status->bRight = axis[0] > joystick_neutral_x + DEADZONE;
	status->bUp = axis[1] < joystick_neutral_y - DEADZONE;
	status->bDown = axis[1] > joystick_neutral_y + DEADZONE;

	// Read buttons
	int total_buttons;
	const unsigned char *buttons = glfwGetJoystickButtons(connected_joystick_id, &total_buttons);

	// The original `Input.cpp` assumed there were 32 buttons (because of DirectInput's `DIJOYSTATE` struct)
	if (total_buttons > 32)
		total_buttons = 32;

	// Read whatever buttons actually exist
	for (int i = 0; i < total_buttons; ++i)
		status->bButton[i] = buttons[i] == GLFW_PRESS;

	// Blank the buttons that do not
	for (int i = total_buttons; i < 32; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (!joystick_connected)
		return FALSE;

	// The code that would normally run here has been moved to JoystickCallback, to better-support hotplugging

	return TRUE;
}
