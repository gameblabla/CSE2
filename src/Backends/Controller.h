#pragma once

bool ControllerBackend_Init(void);
void ControllerBackend_Deinit(void);
bool ControllerBackend_GetJoystickStatus(bool **buttons, unsigned int *button_count, short **axes, unsigned int *axis_count);
const char* ControllerBackend_GetButtonName(unsigned int button_id);
