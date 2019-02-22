#include "Input.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <SDL.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"
#include "Types.h"

#define JOYSTICK_DEADZONE 10000

SDL_Joystick *joystick; //This may be a name that was given by Simon, but it fits the rest of Pixel's names so it's fine.

void ReleaseDirectInput()
{
	//Close opened joystick (if exists)
	if (joystick)
	{
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
}

bool InitDirectInput()
{
	//Open first available joystick
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		joystick = SDL_JoystickOpen(i);
			
		//Break as soon as a joystick is properly opened
		if (joystick)
			break;
	}
	
	return true;
}

bool GetJoystickStatus(JOYSTICK_STATUS *pStatus)
{
	//Clear status
	memset(pStatus, 0, sizeof(JOYSTICK_STATUS));
	
	if (joystick)
	{
		int32_t x = SDL_JoystickGetAxis(joystick, 0);
		int32_t y = SDL_JoystickGetAxis(joystick, 1);
		pStatus->bLeft = x <= -JOYSTICK_DEADZONE;
		pStatus->bRight = x >= JOYSTICK_DEADZONE;
		pStatus->bUp = y <= -JOYSTICK_DEADZONE;
		pStatus->bDown = y >= JOYSTICK_DEADZONE;
		
		int numButtons = SDL_JoystickNumButtons(joystick);
		if (numButtons > 32)
			numButtons = 32;
		
		for (int button = 0; button < numButtons; button++)
			pStatus->bButton[button] = SDL_JoystickGetButton(joystick, button) != 0;
		
		return true;
	}
	
	return false;
}

bool ResetJoystickStatus()
{
	return true;
}
