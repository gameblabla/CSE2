#pragma once

#include "WindowsWrapper.h"

extern HWND ghWnd;
extern BOOL bFullscreen;

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);
