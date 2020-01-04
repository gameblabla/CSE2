#pragma once

#include "WindowsWrapper.h"

#include "KeyControl.h"

struct CONFIG
{
	char proof[0x20];
	char font_name[0x40];
	char move_button_mode;
	char attack_button_mode;
	char ok_button_mode;
	char display_mode;
	BOOL bJoystick;
	char joystick_button[MAX_JOYSTICK_BUTTONS];
	BOOL b60fps;
	BOOL bVsync;
	unsigned long key_bindings[13];
};

BOOL LoadConfigData(CONFIG *conf);
BOOL SaveConfigData(const CONFIG *conf);
void DefaultConfigData(CONFIG *conf);
