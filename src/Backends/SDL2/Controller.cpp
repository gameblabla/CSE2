#include "../Controller.h"
#include "Controller.h"

#include <stddef.h>
#include <stdio.h>

#include "SDL.h"

#include "../../WindowsWrapper.h"
#include "../Misc.h"

#define DEADZONE 10000

static SDL_Joystick *joystick;

static Sint16 *axis_neutrals;

BOOL ControllerBackend_Init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		Backend_PrintError("Couldn't initialise joystack SDL2 subsystem: %s", SDL_GetError());
		return FALSE;
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

	const size_t button_limit = sizeof(status->bButton) / sizeof(status->bButton[0]);

	// Handle directional inputs
	const Sint16 joystick_x = SDL_JoystickGetAxis(joystick, 0);
	if (!joystick_x)
		Backend_PrintError("Failed to get current state of X axis control on joystick: %s", SDL_GetError());

	const Sint16 joystick_y = SDL_JoystickGetAxis(joystick, 1);
	if (!joystick_y)
		Backend_PrintError("Failed to get current state of Y axis control on joystick: %s", SDL_GetError());

	status->bLeft = joystick_x < axis_neutrals[0] - DEADZONE;
	status->bRight = joystick_x > axis_neutrals[0] + DEADZONE;
	status->bUp = joystick_y < axis_neutrals[1] - DEADZONE;
	status->bDown = joystick_y > axis_neutrals[1] + DEADZONE;

	// Handle button inputs
	int total_buttons = SDL_JoystickNumButtons(joystick);
	if (total_buttons < 0)
		Backend_PrintError("Failed to get number of buttons on joystick: %s", SDL_GetError());

	int total_axes = SDL_JoystickNumAxes(joystick);
	if (total_axes < 0)
		Backend_PrintError("Failed to get number of general axis controls on joystick: %s", SDL_GetError());

	int total_hats = SDL_JoystickNumHats(joystick);
	if (total_hats < 0)
		Backend_PrintError("Failed to get number of POV hats on joystick: %s", SDL_GetError());

	unsigned int buttons_done = 0;

	// Start with the joystick buttons
	for (int i = 0; i < total_buttons; ++i)
	{
		status->bButton[buttons_done] = SDL_JoystickGetButton(joystick, i);

		if (++buttons_done >= button_limit)
			break;
	}

	// Then the joystick axes
	for (int i = 0; i < total_axes; ++i)
	{
		Sint16 axis = SDL_JoystickGetAxis(joystick, i);

		status->bButton[buttons_done] = axis < axis_neutrals[i] - DEADZONE;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = axis > axis_neutrals[i] + DEADZONE;

		if (++buttons_done >= button_limit)
			break;
	}

	// Then the joystick hats
	for (int i = 0; i < total_hats; ++i)
	{
		Uint8 hat = SDL_JoystickGetHat(joystick, i);

		status->bButton[buttons_done] = hat == SDL_HAT_UP || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_RIGHTUP;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hat == SDL_HAT_RIGHT || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_RIGHTDOWN;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hat == SDL_HAT_DOWN || hat == SDL_HAT_LEFTDOWN || hat == SDL_HAT_RIGHTDOWN;

		if (++buttons_done >= button_limit)
			break;

		status->bButton[buttons_done] = hat == SDL_HAT_LEFT || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_LEFTDOWN;

		if (++buttons_done >= button_limit)
			break;
	}

	// Blank any remaining buttons
	for (size_t i = buttons_done; i < button_limit; ++i)
		status->bButton[i] = FALSE;

	return TRUE;
}

BOOL ControllerBackend_ResetJoystickStatus(void)
{
	if (joystick == NULL)
		return FALSE;

	return TRUE;
}

void ControllerBackend_JoystickConnect(Sint32 joystick_id)
{
	const char *joystick_name = SDL_JoystickNameForIndex(joystick_id);
	if (joystick_name != NULL)
	{
		Backend_PrintInfo("Joystick #%d connected - %s", joystick_id, joystick_name);
	}
	else
	{
		Backend_PrintError("Couldn't get joystick name: %s", SDL_GetError());
		Backend_PrintInfo("Joystick #%d connected - Name unknown", joystick_id);
	}

	if (joystick == NULL)
	{
		joystick = SDL_JoystickOpen(joystick_id);

		if (joystick != NULL)
		{
			int total_axes = SDL_JoystickNumAxes(joystick);
			if (total_axes < 0)
				Backend_PrintError("Couldn't get number of general axis control on connected joystick: %s", SDL_GetError());

			int total_buttons = SDL_JoystickNumButtons(joystick);
			if (total_buttons < 0)
				Backend_PrintError("Couldn't get number of buttons on connected joystick: %s", SDL_GetError());

			if (total_axes >= 2 && total_buttons >= 6)
			{
				Backend_PrintInfo("Joystick #%d selected", joystick_id);

				// Set up neutral axes
				axis_neutrals = (Sint16*)malloc(sizeof(Sint16) * total_axes);
				if (axis_neutrals)
				{
					for (int i = 0; i < total_axes; ++i)
						axis_neutrals[i] = SDL_JoystickGetAxis(joystick, i);
				}
				else
				{
					Backend_PrintError("Couldn't allocate memory for neutral axes");
				}
			}
			else
			{
				SDL_JoystickClose(joystick);
				joystick = NULL;
			}
		}
		else
		{
			Backend_PrintError("Couldn't open joystick for use: %s", SDL_GetError());
		}
	}
}

void ControllerBackend_JoystickDisconnect(Sint32 joystick_id)
{
	SDL_JoystickID current_joystick_id = SDL_JoystickInstanceID(joystick);
	if (current_joystick_id < 0)
		Backend_PrintError("Couldn't get instance ID for current joystick: %s", SDL_GetError());

	if (joystick_id == current_joystick_id)
	{
		Backend_PrintInfo("Joystick #%d disconnected", joystick_id);
		SDL_JoystickClose(joystick);
		joystick = NULL;

		free(axis_neutrals);
	}
}
