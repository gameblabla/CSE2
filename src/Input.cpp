#include "Input.h"

#include <stddef.h>

#include "Backends/Controller.h"

#include "WindowsWrapper.h"

static int joystick_neutral_x = 0;
static int joystick_neutral_y = 0;

void ReleaseDirectInput(void)
{
	ControllerBackend_Deinit();
}

BOOL InitDirectInput(void)
{
	return ControllerBackend_Init();
}

BOOL GetJoystickStatus(JOYSTICK_STATUS *status)
{
	const size_t button_limit = sizeof(status->bButton) / sizeof(status->bButton[0]);

	bool *buttons;
	unsigned int button_count;

	short *axes;
	unsigned int axis_count;

	if (!ControllerBackend_GetJoystickStatus(&buttons, &button_count, &axes, &axis_count))
		return FALSE;

	if (button_count > button_limit)
		button_count = button_limit;

	for (unsigned int i = 0; i < button_count; ++i)
		status->bButton[i] = buttons[i];

	for (unsigned int i = button_count; i < button_limit; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ResetJoystickStatus(void)
{
	bool *buttons;
	unsigned int button_count;

	short *axes;
	unsigned int axis_count;

	if (!ControllerBackend_GetJoystickStatus(&buttons, &button_count, &axes, &axis_count))
		return FALSE;

	joystick_neutral_x = 0;
	joystick_neutral_y = 0;

	if (axis_count >= 1)
		joystick_neutral_x = axes[0];

	if (axis_count >= 2)
		joystick_neutral_y = axes[1];

	return TRUE;
}
