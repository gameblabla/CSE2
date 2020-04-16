#pragma once

#include "WindowsWrapper.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern HWND ghWnd;
extern BOOL bFullscreen;

void PutFramePerSecound(void);
unsigned long CountFramePerSecound(void);

BOOL SystemTask(void);
