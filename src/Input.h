#pragma once

#include "WindowsWrapper.h"

#define MAX_JOYSTICK_BUTTONS 32

struct JOYSTICK_STATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[MAX_JOYSTICK_BUTTONS];
};

void ReleaseDirectInput(void);
BOOL InitDirectInput(void);
BOOL GetJoystickStatus(JOYSTICK_STATUS *status);
BOOL ResetJoystickStatus(void);
