#pragma once

#include "../WindowsWrapper.h"

#include "../Input.h"

BOOL ControllerBackend_Init(void);
void ControllerBackend_Deinit(void);
BOOL ControllerBackend_GetJoystickStatus(JOYSTICK_STATUS *status);
BOOL ControllerBackend_ResetJoystickStatus(void);
