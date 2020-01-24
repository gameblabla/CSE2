#pragma once

#include "SDL.h"

#include "WindowsWrapper.h"

// Was originally 32, like older versions of DirectInput, but since
// later versions support up to 128, let's use that instead
#define MAX_JOYSTICK_BUTTONS 128

struct JOYSTICK_STATUS
{
	BOOL bLeft;
	BOOL bRight;
	BOOL bUp;
	BOOL bDown;
	BOOL bButton[MAX_JOYSTICK_BUTTONS];
};

extern SDL_Joystick *joystick;

void ReleaseDirectInput(void);
void InitDirectInput(void);
BOOL GetJoystickStatus(JOYSTICK_STATUS *status);
BOOL ResetJoystickStatus(void);
