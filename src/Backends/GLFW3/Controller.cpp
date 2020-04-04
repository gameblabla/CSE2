#include "../Controller.h"

#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../WindowsWrapper.h"

#define DEADZONE (10000.0f / 32767.0f)

static BOOL joystick_connected;
static int connected_joystick_id;

static void JoystickCallback(int joystick_id, int event)
{
	switch (event)
	{
		case GLFW_CONNECTED:
			printf("Joystick #%d connected - %s\n", joystick_id, glfwGetJoystickName(joystick_id));

			if (!joystick_connected)
			{
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
				if (glfwJoystickIsGamepad(joystick_id) == GLFW_TRUE)	// Avoid selecting things like laptop touchpads
#endif
				{
					printf("Joystick #%d selected\n", joystick_id);
					joystick_connected = TRUE;
					connected_joystick_id = joystick_id;
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

void ControllerBackend_Deinit(void)
{
	
}

BOOL ControllerBackend_Init(void)
{
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
		if (glfwJoystickPresent(i) == GLFW_TRUE)
			JoystickCallback(i, GLFW_CONNECTED);

	glfwSetJoystickCallback(JoystickCallback);

	return TRUE;
}

BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (!joystick_connected)
		return FALSE;

	int total_axis;
	const float *axis = glfwGetJoystickAxes(connected_joystick_id, &total_axis);

	status->bLeft = axis[0] < -DEADZONE;
	status->bRight = axis[0] > DEADZONE;
	status->bUp = axis[1] < -DEADZONE;
	status->bDown = axis[1] > DEADZONE;

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

	return TRUE;
}
