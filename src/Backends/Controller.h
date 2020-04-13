#pragma once

#include "../WindowsWrapper.h"

BOOL ControllerBackend_Init(void);
void ControllerBackend_Deinit(void);
BOOL ControllerBackend_GetJoystickStatus(BOOL **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count);
