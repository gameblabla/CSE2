#pragma once

#include <string>

#include "WindowsWrapper.h"

#include "Config.h"
#include "Input.h"

extern std::string gModulePath;
extern std::string gDataPath;

extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

extern CONFIG_BINDING bindings[BINDING_TOTAL];

void PutFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
