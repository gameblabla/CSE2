#include "../Controller.h"
#include "Controller.h"

#include <stddef.h>
#include <stdio.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

#define DEADZONE 10000;

static SDL_Joystick *joystick;
static int joystick_neutral_x;
static int joystick_neutral_y;

BOOL ControllerBackend_Init(void)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	return TRUE;
}

void ControllerBackend_Deinit(void)
{
	if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (joystick == NULL)
		return FALSE;

	// Read axis
	const Sint16 joystick_x = SDL_JoystickGetAxis(joystick, 0);
	const Sint16 joystick_y = SDL_JoystickGetAxis(joystick, 1);

	status->bLeft = joystick_x < joystick_neutral_x - DEADZONE;
	status->bRight = joystick_x > joystick_neutral_x + DEADZONE;
	status->bUp = joystick_y < joystick_neutral_y - DEADZONE;
	status->bDown = joystick_y > joystick_neutral_y + DEADZONE;

	// The original `Input.cpp` assumed there were 32 buttons (because of DirectInput's `DIJOYSTATE` struct)
	int numButtons = SDL_JoystickNumButtons(joystick);
	if (numButtons > 32)
		numButtons = 32;

	// Read whatever buttons actually exist
	for (int i = 0; i < numButtons; ++i)
		status->bButton[i] = SDL_JoystickGetButton(joystick, i);

	// Blank the buttons that do not
	for (int i = numButtons; i < 32; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (joystick == NULL)
		return FALSE;

	// The code that would normally run here has been moved to JoystickCallback, to better-support hotplugging

	return TRUE;
}

void ControllerBackend_JoystickConnect(Sint32 joystick_id)
{
	printf("Joystick #%d connected - %s\n", joystick_id, SDL_JoystickNameForIndex(joystick_id));

	if (joystick == NULL)
	{
		joystick = SDL_JoystickOpen(joystick_id);

		if (joystick != NULL)
		{
			printf("Joystick #%d selected\n", joystick_id);

			// Reset default stick positions (this is performed in ResetJoystickStatus in vanilla Cave Story
			joystick_neutral_x = SDL_JoystickGetAxis(joystick, 0);
			joystick_neutral_y = SDL_JoystickGetAxis(joystick, 1);
		}
	}
}

void ControllerBackend_JoystickDisconnect(Sint32 joystick_id)
{
	if (joystick_id == SDL_JoystickInstanceID(joystick))
	{
		printf("Joystick #%d disconnected\n", joystick_id);
		joystick = NULL;
	}
}
