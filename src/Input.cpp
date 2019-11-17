#include "Input.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

static SDL_Joystick *joystick;

static int joystick_neutral_x;
static int joystick_neutral_y;

void ReleaseDirectInput(void)
{
	// Close opened joystick (if exists)
	if (joystick != NULL)
	{
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
}

BOOL HookAllDirectInputDevices(void);

BOOL InitDirectInput(void)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	if (!HookAllDirectInputDevices())
		return FALSE;

	return TRUE;
}

// The original name for this function is unknown
BOOL HookAllDirectInputDevices(void)
{
	int i;

	// Open first available joystick
	for (i = 0; i < SDL_NumJoysticks(); ++i)
	{
		joystick = SDL_JoystickOpen(i);

		// Break as soon as a joystick is properly opened
		if (joystick != NULL)
			return TRUE;
	}

	return FALSE;
}

BOOL GetJoystickStatus(JOYSTICK_STATUS *status)
{
	if (joystick == NULL)
		return FALSE;

	int numButtons = SDL_JoystickNumButtons(joystick);
	if (numButtons > 32)
		numButtons = 32;

	for (int i = 0; i < numButtons; ++i)
	{
		if (SDL_JoystickGetButton(joystick, i) != 0)
			status->bButton[i] = TRUE;
		else
			status->bButton[i] = FALSE;
	}

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

BOOL ResetJoystickStatus(void)
{
	if (joystick == NULL)
		return FALSE;

	joystick_neutral_x = SDL_JoystickGetAxis(joystick, 0);
	joystick_neutral_y = SDL_JoystickGetAxis(joystick, 1);

	return TRUE;
}
