#pragma once

#include "WindowsWrapper.h"

extern char *gModulePath;
extern char *gDataPath;

extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

extern BOOL gbUseJoystick;

void PutFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
