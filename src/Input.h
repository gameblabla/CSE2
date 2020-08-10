#pragma once

#include "WindowsWrapper.h"

struct DIRECTINPUTSTATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[32];
};

void ReleaseDirectInput(void);
BOOL InitDirectInput(HINSTANCE hinst, HWND hWnd);
BOOL GetJoystickStatus(DIRECTINPUTSTATUS *status);
BOOL ResetJoystickStatus(void);
