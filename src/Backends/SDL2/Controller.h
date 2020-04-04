#pragma once

#include "SDL.h"

#include "../../WindowsWrapper.h"

void ControllerBackend_JoystickConnect(Sint32 joystick_id);
void ControllerBackend_JoystickDisconnect(Sint32 joystick_id);
