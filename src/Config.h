#pragma once

#include "WindowsWrapper.h"

struct CONFIG
{
	char proof[0x20];
	char font_name[0x40];
	long move_button_mode;
	long attack_button_mode;
	long ok_button_mode;
	long display_mode;
	long bJoystick;
	long joystick_button[8];
};

BOOL LoadConfigData(CONFIG *conf);
void DefaultConfigData(CONFIG *conf);
