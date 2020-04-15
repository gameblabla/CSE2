#pragma once

#include "Backends/Misc.h"
#include "Input.h"

enum KeyBind
{
	//The movement keys go in the order of left, right, up and down
	KEY_LEFT =      0x00000001,
	KEY_RIGHT =     0x00000002,
	KEY_UP =        0x00000004,
	KEY_DOWN =      0x00000008,
	//Map key
	KEY_MAP =       0x00000010,
	//Jump and Shoot keys
	KEY_SHOT =      0x00000020,
	KEY_JUMP =      0x00000040,
	//Left and right weapon switch keys
	KEY_ARMS =      0x00000080,
	KEY_ARMSREV =   0x00000100,
	//Function keys
	KEY_F1 =        0x00000400,
	KEY_F2 =        0x00000800,
	//Inventory
	KEY_ITEM =      0x00001000,
	//Okay and cancel keys
	KEY_OK =        0x00002000,
	KEY_CANCEL =    0x00004000,
	//Escape key
	KEY_ESCAPE =    0x00008000,
	//Pause key
	KEY_PAUSE =     0x00010000,
};

extern bool gKeyboardState[BACKEND_KEYBOARD_TOTAL];
extern JOYSTICK_STATUS gJoystickState;

extern long gKey;
extern long gKeyTrg;

extern long gKeyJump;
extern long gKeyShot;
extern long gKeyArms;
extern long gKeyArmsRev;
extern long gKeyItem;
extern long gKeyMap;

extern long gKeyOk;
extern long gKeyCancel;

extern long gKeyLeft;
extern long gKeyUp;
extern long gKeyRight;
extern long gKeyDown;

void GetTrg(void);
