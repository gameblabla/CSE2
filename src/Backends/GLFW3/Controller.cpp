#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../../WindowsWrapper.h"

#define DEADZONE (10000.0f / 32767.0f)

static BOOL joystick_connected;
static int connected_joystick_id;

static float *axis_neutrals;

static void JoystickCallback(int joystick_id, int event)
{
	switch (event)
	{
		case GLFW_CONNECTED:
			printf("Joystick #%d connected - %s\n", joystick_id, glfwGetJoystickName(joystick_id));

			if (!joystick_connected)
			{
				int total_axes;
				const float *axes = glfwGetJoystickAxes(joystick_id, &total_axes);

				int total_buttons;
				const unsigned char *buttons = glfwGetJoystickButtons(connected_joystick_id, &total_buttons);

				if (total_axes >= 2 && total_buttons >= 6)
				{
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
					if (glfwJoystickIsGamepad(joystick_id) == GLFW_TRUE)	// Avoid selecting things like laptop touchpads
#endif
					{
						printf("Joystick #%d selected\n", joystick_id);
						joystick_connected = TRUE;
						connected_joystick_id = joystick_id;

						// Set up neutral axes
						axis_neutrals = (float*)malloc(sizeof(float) * total_axes);

						for (int i = 0; i < total_axes; ++i)
							axis_neutrals[i] = axes[i];
					}
				}
			}

			break;

		case GLFW_DISCONNECTED:
			if (joystick_connected && joystick_id == connected_joystick_id)
			{
				printf("Joystick #%d disconnected\n", connected_joystick_id);
				joystick_connected = FALSE;

				free(axis_neutrals);
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
}

BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (!joystick_connected)
		return FALSE;

	const size_t button_limit = sizeof(status->bButton) / sizeof(status->bButton[0]);

	int total_buttons;
	const unsigned char *buttons = glfwGetJoystickButtons(connected_joystick_id, &total_buttons);

	int total_axes;
	const float *axes = glfwGetJoystickAxes(connected_joystick_id, &total_axes);

	int total_hats;
	const unsigned char *hats = glfwGetJoystickHats(connected_joystick_id, &total_hats);

	// Handle direction inputs
	status->bLeft = axes[0] < -DEADZONE;
	status->bRight = axes[0] > DEADZONE;
	status->bUp = axes[1] < -DEADZONE;
	status->bDown = axes[1] > DEADZONE;

	// Handle button inputs
	unsigned int buttons_done = 0;

	// Start with the joystick buttons
	for (int i = 0; i < total_buttons; ++i)
	{
		status->bButton[buttons_done] = buttons[i] == GLFW_PRESS;

		if (++buttons_done >= button_limit)
			break;
	}

	// Then the joystick axes
	for (int i = 0; i < total_axes; ++i)
	{
		status->bButton[buttons_done] = axes[i] < axis_neutrals[i] - DEADZONE;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = axes[i] > axis_neutrals[i] + DEADZONE;

		if (++buttons_done >= button_limit)
			break;
	}

	// Then the joystick hats
	for (int i = 0; i < total_axes; ++i)
	{
		status->bButton[buttons_done] = hats[i] == GLFW_HAT_UP;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hats[i] == GLFW_HAT_RIGHT;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hats[i] == GLFW_HAT_DOWN;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hats[i] == GLFW_HAT_LEFT;

		if (++buttons_done >= button_limit)
			break;
	}

	// Blank any remaining buttons
	for (size_t i = buttons_done; i < button_limit; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (!joystick_connected)
		return FALSE;

	return TRUE;
}
