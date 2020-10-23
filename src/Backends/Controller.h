// Released under the MIT licence.
// See LICENCE.txt for details.

#pragma once

bool ControllerBackend_Init(void);
void ControllerBackend_Deinit(void);
bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count);
