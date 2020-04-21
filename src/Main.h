#pragma once

#include "WindowsWrapper.h"

#include "Config.h"
#include "Input.h"

extern char gModulePath[MAX_PATH];
extern char gDataPath[MAX_PATH];

extern BOOL bFullscreen;

extern CONFIG_BINDING bindings[BINDING_TOTAL];

void PutFramePerSecound(void);
unsigned long CountFramePerSecound(void);

void InactiveWindow(void);
void ActiveWindow(void);

BOOL SystemTask(void);
