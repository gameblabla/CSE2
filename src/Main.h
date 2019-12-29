#pragma once

#include "WindowsWrapper.h"

extern BOOL bFullscreen;

extern int gScancodeOk;
extern int gScancodeCancel;
extern int gScancodeJump;
extern int gScancodeShot;
extern int gScancodeArms;
extern int gScancodeArmsRev;
extern int gScancodeItem;
extern int gScancodeMap;
extern int gScancodeUp;
extern int gScancodeDown;
extern int gScancodeLeft;
extern int gScancodeRight;
extern int gScancodePause;

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);
