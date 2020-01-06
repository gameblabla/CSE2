#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern HWND ghWnd;
extern BOOL bFullscreen;

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);

#ifdef __cplusplus
}
#endif
