#pragma once

#include "SDL.h"

extern SDL_Window *window;

void ControllerBackend_JoystickConnect(Sint32 joystick_id);
void ControllerBackend_JoystickDisconnect(Sint32 joystick_id);
