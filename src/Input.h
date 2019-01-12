#pragma once
extern bool gbUseJoystick;
extern int gJoystickButtonTable[8];

struct JOYSTICK_STATUS
{
	bool bLeft;
	bool bRight;
	bool bUp;
	bool bDown;
	bool bButton[32];
};
