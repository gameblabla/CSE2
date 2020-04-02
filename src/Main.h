#pragma once

#include "WindowsWrapper.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern BOOL bFullscreen;

extern BOOL gbUseJoystick;

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
