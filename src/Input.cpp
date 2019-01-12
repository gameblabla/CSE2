#include "Types.h"
#include <stdint.h>
#include <SDL_gamepad.h>
#include "Input.h"

#define JOYSTICK_DEADZONE 0x2000

bool gbUseJoystick;
int gJoystickButtonTable[8];

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

signed int GetJoystickStatus(JOYSTICK_STATUS *pStatus)
{
	//Clear status
	memset(pStatus, 0, sizeof(JOYSTICK_STATUS));
	
	if (joystick)
	{
		int32_t x = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX);
		int32_t y = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY);
		pStatus->bLeft = x <= -JOYSTICK_DEADZONE;
		pStatus->bRight = x >= JOYSTICK_DEADZONE;
		pStatus->bUp = v2 <= -JOYSTICK_DEADZONE;
		pStatus->bDown = v2 >= JOYSTICK_DEADZONE;
		
		int numButtons = SDL_GameControllerNumButtons(joystick);
		if (numButtons > 32)
			numButtons = 32;
		
		for (int button = 0; button < numButtons; button++)
			pStatus->bButton[button] = SDL_GameControllerGetButton(joystick, button) != 0;
	}
	return 1;
}

signed int ResetJoystickStatus()
{
	return 1;
}
