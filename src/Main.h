#pragma once

#include <string>

#include "WindowsWrapper.h"

extern std::string gModulePath;
extern std::string gDataPath;

extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

extern BOOL gbUseJoystick;

void PutFramePerSecound(void);

BOOL SystemTask(void);
