#pragma once

#include "WindowsWrapper.h"

#include "Input.h"

extern BOOL bFullscreen;

extern int gKeyOk_Scancode;
extern int gKeyCancel_Scancode;
extern int gKeyJump_Scancode;
extern int gKeyShot_Scancode;
extern int gKeyArms_Scancode;
extern int gKeyArmsRev_Scancode;
extern int gKeyItem_Scancode;
extern int gKeyMap_Scancode;
extern int gKeyUp_Scancode;
extern int gKeyDown_Scancode;
extern int gKeyLeft_Scancode;
extern int gKeyRight_Scancode;
extern int gKeyPause_Scancode;

extern int gKeyOk_Button;
extern int gKeyCancel_Button;
extern int gKeyJump_Button;
extern int gKeyShot_Button;
extern int gKeyArms_Button;
extern int gKeyArmsRev_Button;
extern int gKeyItem_Button;
extern int gKeyMap_Button;
extern int gKeyUp_Button;
extern int gKeyDown_Button;
extern int gKeyLeft_Button;
extern int gKeyRight_Button;
extern int gKeyPause_Button;

extern int gJoystickButtonTable[MAX_JOYSTICK_BUTTONS];

void PutFramePerSecound(void);
unsigned long GetFramePerSecound(void);

BOOL SystemTask(void);
