#pragma once

#include "WindowsWrapper.h"

#include "Config.h"
#include "Input.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

extern CONFIG_BINDING bindings[BINDING_TOTAL];

void PutFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
