#include "../Controller.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "../Misc.h"
#include "../Shared/SDL2.h"

#define DEADZONE 10000

static SDL_Joystick *joystick;

static Sint16 *axis_neutrals;

bool ControllerBackend_Init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
	{
		Backend_PrintError("Couldn't initialise joystick SDL subsystem: %s", SDL_GetError());
		return false;
	}

	return true;
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

bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count)
{
	if (joystick == NULL)
		return false;

	int total_sdl_buttons = SDL_JoystickNumButtons(joystick);
	if (total_sdl_buttons < 0)
		Backend_PrintError("Failed to get number of buttons on joystick: %s", SDL_GetError());

	int total_sdl_axes = SDL_JoystickNumAxes(joystick);
	if (total_sdl_axes < 0)
		Backend_PrintError("Failed to get number of general axis controls on joystick: %s", SDL_GetError());

	int total_sdl_hats = SDL_JoystickNumHats(joystick);
	if (total_sdl_hats < 0)
		Backend_PrintError("Failed to get number of POV hats on joystick: %s", SDL_GetError());

	*button_count = total_sdl_buttons + total_sdl_axes * 2 + total_sdl_hats * 4;
	*axis_count = total_sdl_axes;

	static bool *button_buffer = NULL;
	static short *axis_buffer = NULL;

	bool *new_button_buffer = (bool*)realloc(button_buffer, *button_count * sizeof(bool));
	short *new_axis_buffer = (short*)realloc(axis_buffer, *axis_count * sizeof(short));

	if (new_button_buffer == NULL || new_axis_buffer == NULL)
		return false;

	button_buffer = new_button_buffer;
	axis_buffer = new_axis_buffer;

	//////////////////////////
	// Handle button inputs //
	//////////////////////////

	unsigned int current_button = 0;

	// Start with the joystick buttons
	for (int i = 0; i < total_sdl_buttons; ++i)
		button_buffer[current_button++] = SDL_JoystickGetButton(joystick, i);

	// Then the joystick axes
	for (int i = 0; i < total_sdl_axes; ++i)
	{
		Sint16 axis = SDL_JoystickGetAxis(joystick, i);

		button_buffer[current_button++] = axis < axis_neutrals[i] - DEADZONE;
		button_buffer[current_button++] = axis > axis_neutrals[i] + DEADZONE;
	}

	// Then the joystick hats
	for (int i = 0; i < total_sdl_hats; ++i)
	{
		Uint8 hat = SDL_JoystickGetHat(joystick, i);

		button_buffer[current_button++] = hat == SDL_HAT_UP || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_RIGHTUP;
		button_buffer[current_button++] = hat == SDL_HAT_RIGHT || hat == SDL_HAT_RIGHTUP || hat == SDL_HAT_RIGHTDOWN;
		button_buffer[current_button++] = hat == SDL_HAT_DOWN || hat == SDL_HAT_LEFTDOWN || hat == SDL_HAT_RIGHTDOWN;
		button_buffer[current_button++] = hat == SDL_HAT_LEFT || hat == SDL_HAT_LEFTUP || hat == SDL_HAT_LEFTDOWN;
	}

	*buttons = button_buffer;

	////////////////////////
	// Handle axis inputs //
	////////////////////////

	for (int i = 0; i < total_sdl_axes; ++i)
		axis_buffer[i] = SDL_JoystickGetAxis(joystick, i);

	*axes = axis_buffer;

	return true;
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

			if (total_buttons >= 6)
			{
				Backend_PrintInfo("Joystick #%d selected", joystick_id);

				// Set up neutral axes
				axis_neutrals = (Sint16*)malloc(sizeof(Sint16) * total_axes);

				if (axis_neutrals != NULL)
				{
					for (int i = 0; i < total_axes; ++i)
						axis_neutrals[i] = SDL_JoystickGetAxis(joystick, i);

					return;
				}
				else
				{
					Backend_PrintError("Couldn't allocate memory for neutral axes");
				}
			}

			SDL_JoystickClose(joystick);
			joystick = NULL;
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

const char* ControllerBackend_GetButtonName(unsigned int button_id)
{
	static char name_buffer[0x10];

	snprintf(name_buffer, sizeof(name_buffer), "Button #%d", button_id);

	return name_buffer;
}
