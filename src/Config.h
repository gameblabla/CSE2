#pragma once

#include <stdint.h>

struct CONFIG
{
	char proof[0x20];
	char font_name[0x40];
	int32_t move_button_mode;
	int32_t attack_button_mode;
	int32_t ok_button_mode;
	int32_t display_mode;
	int32_t bJoystick;
	int32_t joystick_button[8];
};

bool LoadConfigData(CONFIG *conf);
void DefaultConfigData(CONFIG *conf);
