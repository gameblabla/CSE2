#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../Misc.h"
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
			Backend_PrintInfo("Joystick #%d connected - %s", joystick_id, glfwGetJoystickName(joystick_id));

			if (!joystick_connected)
			{
				int total_axes;
				const float *axes = glfwGetJoystickAxes(joystick_id, &total_axes);

				int total_buttons;
				const unsigned char *buttons = glfwGetJoystickButtons(joystick_id, &total_buttons);

				if (total_axes >= 2 && total_buttons >= 6)
				{
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
					if (glfwJoystickIsGamepad(joystick_id) == GLFW_TRUE)	// Avoid selecting things like laptop touchpads
#endif
					{
						// Set up neutral axes
						axis_neutrals = (float*)malloc(sizeof(float) * total_axes);

						if (axis_neutrals != NULL)
						{
							for (int i = 0; i < total_axes; ++i)
								axis_neutrals[i] = axes[i];

							printf("Joystick #%d selected\n", joystick_id);
							joystick_connected = TRUE;
							connected_joystick_id = joystick_id;
						}
						else
						{
							Backend_PrintError("Couldn't allocate memory for joystick axes");
						}
					}
				}
			}

			break;

		case GLFW_DISCONNECTED:
			if (joystick_connected && joystick_id == connected_joystick_id)
			{
				Backend_PrintInfo("Joystick #%d disconnected", connected_joystick_id);
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

	free(axis_neutrals);
	axis_neutrals = NULL;
}

BOOL ControllerBackend_GetJoystickStatus(BOOL **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count)
{
	if (!joystick_connected)
		return FALSE;

	int total_glfw_buttons;
	const unsigned char *glfw_buttons = glfwGetJoystickButtons(connected_joystick_id, &total_glfw_buttons);

	int total_glfw_axes;
	const float *glfw_axes = glfwGetJoystickAxes(connected_joystick_id, &total_glfw_axes);

	int total_glfw_hats = 0;
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
	const unsigned char *glfw_hats = glfwGetJoystickHats(connected_joystick_id, &total_glfw_hats);
#endif

	*button_count = total_glfw_buttons + total_glfw_axes * 2 + total_glfw_hats * 4;
	*axis_count = total_glfw_axes;

	static BOOL *button_buffer = NULL;
	static short *axis_buffer = NULL;

	BOOL *new_button_buffer = (BOOL*)realloc(button_buffer, *button_count * sizeof(BOOL));
	short *new_axis_buffer = (short*)realloc(axis_buffer, *axis_count * sizeof(short));

	if (new_button_buffer == NULL || new_axis_buffer == NULL)
		return FALSE;

	button_buffer = new_button_buffer;
	axis_buffer = new_axis_buffer;

	//////////////////////////
	// Handle button inputs //
	//////////////////////////

	unsigned int current_button = 0;

	// Start with the joystick buttons
	for (int i = 0; i < total_glfw_buttons; ++i)
		button_buffer[current_button++] = glfw_buttons[i] == GLFW_PRESS;

	// Then the joystick axes
	for (int i = 0; i < total_glfw_axes; ++i)
	{
		button_buffer[current_button++] = glfw_axes[i] < axis_neutrals[i] - DEADZONE;
		button_buffer[current_button++] = glfw_axes[i] > axis_neutrals[i] + DEADZONE;
	}

#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
	// Then the joystick hats
	for (int i = 0; i < total_glfw_hats; ++i)
	{
		button_buffer[current_button++] = glfw_hats[i] & GLFW_HAT_UP;
		button_buffer[current_button++] = glfw_hats[i] & GLFW_HAT_RIGHT;
		button_buffer[current_button++] = glfw_hats[i] & GLFW_HAT_DOWN;
		button_buffer[current_button++] = glfw_hats[i] & GLFW_HAT_LEFT;
	}
#endif

	*buttons = button_buffer;

	////////////////////////
	// Handle axis inputs //
	////////////////////////

	for (int i = 0; i < total_glfw_axes; ++i)
		axis_buffer[i] = (short)(glfw_axes[i] * 0x7FFF);

	*axes = axis_buffer;

	return TRUE;
}
