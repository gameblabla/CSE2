#pragma once

#include "WindowsWrapper.h"

#include "Input.h"

struct CONFIG
{
	char proof[0x20];
	char font_name[0x40];
	char display_mode;
	BOOL bJoystick;
	BOOL b60fps;
	BOOL bVsync;
	unsigned long key_bindings[13];
	char button_bindings[13];
};

BOOL LoadConfigData(CONFIG *conf);
BOOL SaveConfigData(const CONFIG *conf);
void DefaultConfigData(CONFIG *conf);
