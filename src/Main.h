#pragma once

#include "WindowsWrapper.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern HWND ghWnd;
extern BOOL bFullscreen;
extern BOOL gbUseJoystick;

extern int gJoystickButtonTable[8];

void PutFramePerSecound(void);

BOOL SystemTask(void);
