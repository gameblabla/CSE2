#pragma once

#include "WindowsWrapper.h"

extern BOOL gbUseJoystick;
extern int gJoystickButtonTable[8];

struct JOYSTICK_STATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[32];
};

void ReleaseDirectInput();
BOOL InitDirectInput();
BOOL GetJoystickStatus(JOYSTICK_STATUS *pStatus);
BOOL ResetJoystickStatus();
