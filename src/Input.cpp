#include "Types.h"
#include "CommonDefines.h"
#include <stdint.h>

#include <SDL_gamecontroller.h>
#include "WindowsWrapper.h"

#include "Input.h"
#include "Tags.h"

#define JOYSTICK_DEADZONE 0x2000

SDL_GameController *joystick; //This may be a name that was given by Simon, but it fits the rest of Pixel's names so it's fine.

void ReleaseDirectInput()
{
	//Close opened joystick (if exists)
	if (joystick)
	{
		SDL_GameControllerClose(joystick);
		joystick = nullptr;
	}
}

bool InitDirectInput()
{
	//Load mappings
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, "gamecontrollerdb.txt");
	SDL_GameControllerAddMappingsFromFile(path);
	
	//Open first available joystick
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			joystick = SDL_GameControllerOpen(i);
			
			//Break as soon as a joystick is properly opened
			if (joystick)
				break;
		}
	}
	
	return true;
}

bool GetJoystickStatus(JOYSTICK_STATUS *pStatus)
{
	//Clear status
	memset(pStatus, 0, sizeof(JOYSTICK_STATUS));
	
	if (joystick)
	{
		int32_t x = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
		int32_t y = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
		pStatus->bLeft = x <= -JOYSTICK_DEADZONE;
		pStatus->bRight = x >= JOYSTICK_DEADZONE;
		pStatus->bUp = y <= -JOYSTICK_DEADZONE;
		pStatus->bDown = y >= JOYSTICK_DEADZONE;
		
		int numButtons = SDL_JoystickNumButtons(SDL_GameControllerGetJoystick(joystick));
		if (numButtons > 32)
			numButtons = 32;
		
		for (int button = 0; button < numButtons; button++)
			pStatus->bButton[button] = SDL_GameControllerGetButton(joystick, (SDL_GameControllerButton)button) != 0;
		
		return true;
	}
	
	return false;
}

bool ResetJoystickStatus()
{
	return true;
}
