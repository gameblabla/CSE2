#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"

static SDL_Joystick *joystick;
static int joystick_neutral_x;
static int joystick_neutral_y;

BOOL ControllerBackend_Init(void)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		printf("Joystick #%d connected - %s\n", i, SDL_JoystickNameForIndex(i));

		if (joystick == NULL)
		{
			joystick = SDL_JoystickOpen(i);

			if (joystick != NULL)
				printf("Joystick #%d selected\n", i);
		}
	}

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

	// The original `Input.cpp` assumed there were 32 buttons (because of DirectInput's `DIJOYSTATE` struct)
	int numButtons = SDL_JoystickNumButtons(joystick);
	if (numButtons > 32)
		numButtons = 32;

	// Read whatever buttons actually exist
	int i = 0;
	for (; i < numButtons; ++i)
	{
		if (SDL_JoystickGetButton(joystick, i) != 0)
			status->bButton[i] = TRUE;
		else
			status->bButton[i] = FALSE;
	}

	// Blank the buttons that do not
	for (; i < 32; ++i)
		status->bButton[i] = FALSE;

	status->bDown = FALSE;
	status->bRight = FALSE;
	status->bUp = FALSE;
	status->bLeft = FALSE;

	const Sint16 joystick_x = SDL_JoystickGetAxis(joystick, 0);
	if (joystick_x < joystick_neutral_x - 10000)
		status->bLeft = TRUE;
	else if (joystick_x > joystick_neutral_x + 10000)
		status->bRight = TRUE;

	const Sint16 joystick_y = SDL_JoystickGetAxis(joystick, 1);
	if (joystick_y < joystick_neutral_y - 10000)
		status->bUp = TRUE;
	else if (joystick_y > joystick_neutral_y + 10000)
		status->bDown = TRUE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (joystick == NULL)
		return FALSE;

	joystick_neutral_x = SDL_JoystickGetAxis(joystick, 0);
	joystick_neutral_y = SDL_JoystickGetAxis(joystick, 1);

	return TRUE;
}
