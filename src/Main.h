#pragma once

#include "WindowsWrapper.h"

#include "Config.h"
#include "Input.h"

extern BOOL bFullscreen;

extern CONFIG_BINDING bindings[BINDING_TOTAL];

extern int gJoystickButtonTable[MAX_JOYSTICK_BUTTONS];

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);
