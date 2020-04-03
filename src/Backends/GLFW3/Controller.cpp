#include "../Controller.h"

#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../WindowsWrapper.h"

#define DEADZONE (10000.0f / 32767.0f)

static BOOL joystick_connected;
static int connected_joystick_id;
static float joystick_neutral_x;
static float joystick_neutral_y;

void ControllerBackend_Deinit(void)
{
	
}

BOOL ControllerBackend_Init(void)
{
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
		if (glfwJoystickPresent(i) == GLFW_TRUE)
			printf("Joystick #%d name: %s\n", i, glfwGetJoystickName(i));

	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
	{
		if (glfwJoystickPresent(i) == GLFW_TRUE && glfwJoystickIsGamepad(i) == GLFW_TRUE)
		{
			printf("Joystick #%d selected\n", i);
			joystick_connected = TRUE;
			connected_joystick_id = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (!joystick_connected)
		return FALSE;

	if (glfwJoystickPresent(connected_joystick_id) == GLFW_FALSE || glfwJoystickIsGamepad(connected_joystick_id) == GLFW_FALSE)
		return FALSE;

	int total_axis;
	const float *axis = glfwGetJoystickAxes(connected_joystick_id, &total_axis);

	status->bLeft = axis[0] < joystick_neutral_x - DEADZONE;
	status->bRight = axis[0] > joystick_neutral_x + DEADZONE;
	status->bUp = axis[1] < joystick_neutral_x - DEADZONE;
	status->bDown = axis[1] > joystick_neutral_x + DEADZONE;

	int total_buttons;
	const unsigned char *buttons = glfwGetJoystickButtons(connected_joystick_id, &total_buttons);

	if (total_buttons > 32)
		total_buttons = 32;

	for (int i = 0; i < total_buttons; ++i)
		status->bButton[i] = buttons[i] == GLFW_PRESS;

	for (int i = total_buttons; i < 32; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (!joystick_connected)
		return FALSE;

	if (glfwJoystickPresent(connected_joystick_id) == GLFW_FALSE || glfwJoystickIsGamepad(connected_joystick_id) == GLFW_FALSE)
		return FALSE;

	int total_axis;
	const float *axis = glfwGetJoystickAxes(connected_joystick_id, &total_axis);

	joystick_neutral_x = axis[0];
	joystick_neutral_y = axis[1];

	return TRUE;
}
