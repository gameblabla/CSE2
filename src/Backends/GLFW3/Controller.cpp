#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../Misc.h"

#define DEADZONE (10000.0f / 32767.0f)

static bool joystick_connected;
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

				if (total_buttons >= 6)
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

							Backend_PrintInfo("Joystick #%d selected\n", joystick_id);
							joystick_connected = true;
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
				joystick_connected = false;

				free(axis_neutrals);
			}

			break;
	}
}

bool ControllerBackend_Init(void)
{
	// Connect joysticks that are already plugged-in
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i)
		if (glfwJoystickPresent(i) == GLFW_TRUE)
			JoystickCallback(i, GLFW_CONNECTED);

	// Set-up the callback for future (dis)connections
	glfwSetJoystickCallback(JoystickCallback);

	return true;
}

void ControllerBackend_Deinit(void)
{
	glfwSetJoystickCallback(NULL);

	joystick_connected = false;
	connected_joystick_id = 0;

	free(axis_neutrals);
	axis_neutrals = NULL;
}

bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count)
{
	if (!joystick_connected)
		return false;

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

	static bool *button_buffer = NULL;
	static short *axis_buffer = NULL;

	bool *new_button_buffer = (bool*)realloc(button_buffer, *button_count * sizeof(bool));
	short *new_axis_buffer = (short*)realloc(axis_buffer, *axis_count * sizeof(short));

	if (new_button_buffer == NULL || new_axis_buffer == NULL)
		return false;

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

	return true;
}

const char* ControllerBackend_GetButtonName(unsigned int button_id)
{
	static char name_buffer[0x10];

	snprintf(name_buffer, sizeof(name_buffer), "Button #%d", button_id);

	return name_buffer;
}
