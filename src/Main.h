#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern HWND ghWnd;
extern BOOL bFullscreen;

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);

#ifdef __cplusplus
}
#endif
