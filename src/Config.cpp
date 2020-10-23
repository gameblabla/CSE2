// THIS IS DECOMPILED PROPRIETARY CODE - USE AT YOUR OWN RISK.
//
// The original code belongs to Daisuke "Pixel" Amaya.
//
// Modifications and custom code are under the MIT licence.
// See LICENCE.txt for details.

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include "WindowsWrapper.h"

#include "Config.h"
#include "File.h"
#include "Main.h"

const char* const gConfigName = "Config.dat";
const char* const gProof = "DOUKUTSU20041206";

BOOL LoadConfigData(CONFIGDATA *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIGDATA));

	// Get path
	std::string path = gModulePath + '/' + gConfigName;

	// Open file
	FILE *fp = fopen(path.c_str(), "rb");
	if (fp == NULL)
		return FALSE;

	// Read the version id and font name
	fread(conf->proof, sizeof(conf->proof), 1, fp);
	fread(conf->font_name, sizeof(conf->font_name), 1, fp);

	// Read control settings
	conf->move_button_mode = File_ReadLE32(fp);
	conf->attack_button_mode = File_ReadLE32(fp);
	conf->ok_button_mode = File_ReadLE32(fp);

	// Read display mode (320x240, 640x480, 24-bit fullscreen, 32-bit fullscreen) TODO: add more things?
	conf->display_mode = File_ReadLE32(fp);

	// Read joystick configuration (if enabled, and mappings)
	conf->bJoystick = File_ReadLE32(fp);
	for (int button = 0; button < 8; button++)
		conf->joystick_button[button] = File_ReadLE32(fp);

	// Close file
	fclose(fp);

	// Check if version is not correct, and return if it failed
	if (strcmp(conf->proof, gProof))
	{
		memset(conf, 0, sizeof(CONFIGDATA));
		return FALSE;
	}

	return TRUE;
}

void DefaultConfigData(CONFIGDATA *conf)
{
	// Clear old configuration data
	memset(conf, 0, sizeof(CONFIGDATA));

	// Fun fact: The Linux port added this line:
	// conf->display_mode = 1;

#ifdef _3DS
	conf->display_mode = 1;
#elif defined(__riscos__)
	conf->display_mode = 2;
#endif

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
