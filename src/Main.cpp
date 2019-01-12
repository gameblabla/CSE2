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

#include "Input.h"
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
	
	//Set gamepad inputs
	for (int i = 0; i < 8; i++)
	{
		switch (config.joystick_button[i])
		{
			case 1:
				gJoystickButtonTable[i] = gKeyJump;
				break;
				
			case 2:
				gJoystickButtonTable[i] = gKeyShot;
				break;
				
			case 3:
				gJoystickButtonTable[i] = gKeyArms;
				break;
				
			case 6:
				gJoystickButtonTable[i] = gKeyArmsRev;
				break;
				
			case 4:
				gJoystickButtonTable[i] = gKeyItem;
				break;
				
			case 5:
				gJoystickButtonTable[i] = gKeyMap;
				break;
				
			default:
				continue;
		}
	}
	
	return 0;
}




bool SystemTask()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return false;
				break;
			
			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						gKey |= KEY_ESCAPE;
						break;
						
					case SDL_SCANCODE_W:
						gKey |= KEY_MAP;
						break;
						
					case SDL_SCANCODE_LEFT:
						gKey |= KEY_LEFT;
						break;
						
					case SDL_SCANCODE_RIGHT:
						gKey |= KEY_RIGHT;
						break;
						
					case SDL_SCANCODE_UP:
						gKey |= KEY_UP;
						break;
						
					case SDL_SCANCODE_DOWN:
						gKey |= KEY_DOWN;
						break;
						
					case SDL_SCANCODE_X:
						gKey |= KEY_X;
						break;
						
					case SDL_SCANCODE_Z:
						gKey |= KEY_Z;
						break;
						
					case SDL_SCANCODE_S:
						gKey |= KEY_ARMS;
						break;
						
					case SDL_SCANCODE_A:
						gKey |= KEY_ARMSREV;
						break;
					
					case SDL_SCANCODE_RSHIFT:
					case SDL_SCANCODE_LSHIFT:
						gKey |= KEY_SHIFT;
						break;
						
					case SDL_SCANCODE_F1:
						gKey |= KEY_F1;
						break;
						
					case SDL_SCANCODE_F2:
						gKey |= KEY_F2;
						break;
						
					case SDL_SCANCODE_Q:
						gKey |= KEY_ITEM;
						break;
						
					case SDL_SCANCODE_COMMA:
						gKey |= KEY_ALT_LEFT;
						break;
						
					case SDL_SCANCODE_PERIOD:
						gKey |= KEY_ALT_DOWN;
						break;
						
					case SDL_SCANCODE_SLASH:
						gKey |= KEY_ALT_RIGHT;
						break;
						
					case SDL_SCANCODE_L:
						gKey |= KEY_ALT_UP;
						break;
						
					case SDL_SCANCODE_EQUALS:
						gKey |= KEY_PLUS;
						break;
						
					case SDL_SCANCODE_F5:
						gbUseJoystick = false;
						break;
						
					default:
						return true;
				}
				break;
				
			case SDL_KEYUP:
				switch (event.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
						gKey &= ~KEY_ESCAPE;
						break;
						
					case SDL_SCANCODE_W:
						gKey &= ~KEY_MAP;
						break;
						
					case SDL_SCANCODE_LEFT:
						gKey &= ~KEY_LEFT;
						break;
						
					case SDL_SCANCODE_RIGHT:
						gKey &= ~KEY_RIGHT;
						break;
						
					case SDL_SCANCODE_UP:
						gKey &= ~KEY_UP;
						break;
						
					case SDL_SCANCODE_DOWN:
						gKey &= ~KEY_DOWN;
						break;
						
					case SDL_SCANCODE_X:
						gKey &= ~KEY_X;
						break;
						
					case SDL_SCANCODE_Z:
						gKey &= ~KEY_Z;
						break;
						
					case SDL_SCANCODE_S:
						gKey &= ~KEY_ARMS;
						break;
						
					case SDL_SCANCODE_A:
						gKey &= ~KEY_ARMSREV;
						break;
					
					case SDL_SCANCODE_RSHIFT:
					case SDL_SCANCODE_LSHIFT:
						gKey &= ~KEY_SHIFT;
						break;
						
					case SDL_SCANCODE_F1:
						gKey &= ~KEY_F1;
						break;
						
					case SDL_SCANCODE_F2:
						gKey &= ~KEY_F2;
						break;
						
					case SDL_SCANCODE_Q:
						gKey &= ~KEY_ITEM;
						break;
						
					case SDL_SCANCODE_COMMA:
						gKey &= ~KEY_ALT_LEFT;
						break;
						
					case SDL_SCANCODE_PERIOD:
						gKey &= ~KEY_ALT_DOWN;
						break;
						
					case SDL_SCANCODE_SLASH:
						gKey &= ~KEY_ALT_RIGHT;
						break;
						
					case SDL_SCANCODE_L:
						gKey &= ~KEY_ALT_UP;
						break;
						
					case SDL_SCANCODE_EQUALS:
						gKey &= ~KEY_PLUS;
						break;
						
					default:
						return true;
				}
				break;
		}
	}
	
	return true;
}
