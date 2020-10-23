// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include "Input.h"

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

BOOL GetJoystickStatus(DIRECTINPUTSTATUS *status)
{
	bool *buttons;
	unsigned int button_count;

	short *axes;
	unsigned int axis_count;

	if (!ControllerBackend_GetJoystickStatus(&buttons, &button_count, &axes, &axis_count))
		return FALSE;

	if (button_count > 32)
		button_count = 32;

	for (unsigned int i = 0; i < button_count; ++i)
		status->bButton[i] = buttons[i];

	for (unsigned int i = button_count; i < 32; ++i)
		status->bButton[i] = FALSE;

	status->bDown = FALSE;
	status->bRight = FALSE;
	status->bUp = FALSE;
	status->bLeft = FALSE;

	if (axis_count >= 1)
	{
		if (axes[0] < joystick_neutral_x - 10000)
			status->bLeft = TRUE;
		else if (axes[0] > joystick_neutral_x + 10000)
			status->bRight = TRUE;
	}

	if (axis_count >= 2)
	{
		if (axes[1] < joystick_neutral_y - 10000)
			status->bUp = TRUE;
		else if (axes[1] > joystick_neutral_y + 10000)
			status->bDown = TRUE;
	}

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
