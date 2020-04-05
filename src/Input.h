#pragma once

#include "WindowsWrapper.h"

struct JOYSTICK_STATUS
{
	BOOL bButton[128];	// The original `Input.cpp` assumed there were 32 buttons (because of DirectInput's `DIJOYSTATE` struct)
};

void ReleaseDirectInput(void);
BOOL InitDirectInput(void);
BOOL GetJoystickStatus(JOYSTICK_STATUS *status);
BOOL ResetJoystickStatus(void);
