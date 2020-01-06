#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Config.h"
#include "Main.h"

static const char* const config_filename = "Config.dat";	// Not the original name
static const char* const config_magic = "DOUKUTSU20041206";	// Not the original name

BOOL LoadConfigData(CONFIG *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIG));

	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, config_filename);

	// Open file
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Read data
	size_t fread_result = fread(conf, sizeof(CONFIG), 1, fp); // Not the original name

	// Close file
	fclose(fp);

	// Check if version is not correct, and return if it failed
	if (fread_result != 1 || strcmp(conf->proof, config_magic))
	{
		memset(conf, 0, sizeof(CONFIG));
		return FALSE;
	}

	return TRUE;
}

void DefaultConfigData(CONFIG *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIG));

	// Fun fact: The Linux port added this line:
	// conf->display_mode = 1;

	// Reset joystick settings (as these can't simply be set to 0)
	conf->bJoystick = TRUE;
	conf->joystick_button[0] = 2;
	conf->joystick_button[1] = 1;
	conf->joystick_button[2] = 5;
	conf->joystick_button[3] = 6;
	conf->joystick_button[4] = 3;
	conf->joystick_button[5] = 4;
	conf->joystick_button[6] = 6;
	conf->joystick_button[7] = 3;
}
