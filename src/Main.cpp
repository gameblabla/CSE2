#include <stddef.h>
#include "Types.h"
#include "CommonDefines.h"
#include <stdint.h>
#include <string>

#include <SDL_main.h>
#include <SDL_render.h>
#include <SDL_keyboard.h>
#include <SDL_filesystem.h>
#include <SDL_events.h>

#include "Config.h"
#include "KeyControl.h"

char gModulePath[PATH_LENGTH];
char gDataPath[PATH_LENGTH];

int main(int argc, char *argv[])
{
	//Get executable's path
	strcpy(gModulePath, SDL_GetBasePath());
	if (gModulePath[strlen(gModulePath) - 1] == '/' || gModulePath[strlen(gModulePath) - 1] == '\\')
		gModulePath[strlen(gModulePath) - 1] = 0; //String cannot end in slash or stuff will probably break (original does this through a windows.h provided function)
	
	//Get path of the data folder
	strcpy(gDataPath, gModulePath);
	memcpy(&gDataPath[strlen(gDataPath)], "/data", 6); //Pixel didn't use a strcat
	
	//Load configuration
	CONFIG config;
	
	if (!LoadConfigData(&config))
		DefaultConfigData(&config);
	
	//Apply keybinds
	//Swap X and Z buttons
	if (config.attack_button_mode)
	{
		if (config.attack_button_mode == 1)
		{
			gKeyJump = KEY_X;
			gKeyShot = KEY_Z;
		}
	}
	else
	{
		gKeyJump = KEY_Z;
		gKeyShot = KEY_X;
	}
	
	//Swap Okay and Cancel buttons
	if (config.ok_button_mode)
	{
		if (config.ok_button_mode == 1)
		{
			gKeyOk = gKeyShot;
			gKeyCancel = gKeyJump;
		}
	}
	else
	{
		gKeyOk = gKeyJump;
		gKeyCancel = gKeyShot;
	}
	
	//Alternate movement keys
	if (config.move_button_mode)
	{
		if (config.move_button_mode == 1)
		{
			gKeyLeft = KEY_ALT_LEFT;
			gKeyUp = KEY_ALT_UP;
			gKeyRight = KEY_ALT_RIGHT;
			gKeyDown = KEY_ALT_DOWN;
		}
	}
	else
	{
		gKeyLeft = KEY_LEFT;
		gKeyUp = KEY_UP;
		gKeyRight = KEY_RIGHT;
		gKeyDown = KEY_DOWN;
	}
	
	return 0;
}
