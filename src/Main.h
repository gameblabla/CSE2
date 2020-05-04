#pragma once

#include "WindowsWrapper.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

extern BOOL gbUseJoystick;

void PutFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
