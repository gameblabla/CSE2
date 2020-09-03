#pragma once

#include "WindowsWrapper.h"

struct DIRECTINPUTSTATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[32]; // 32 is the number of buttons in DirectInput's `DIJOYSTATE` struct
};

void ReleaseDirectInput(void);
BOOL InitDirectInput(HINSTANCE hinst, HWND hWnd);
BOOL GetJoystickStatus(DIRECTINPUTSTATUS *status);
BOOL ResetJoystickStatus(void);
