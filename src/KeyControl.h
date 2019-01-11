#pragma once
enum KEYBIND
{
	//The movement keys go in the order of left, right, up and down
	KEY_LEFT =		0x00000001,
	KEY_RIGHT =		0x00000002,
	KEY_UP =		0x00000004,
	KEY_DOWN =		0x00000008,
	//Map key
	KEY_MAP =		0x00000010,
	//Okay and cancel keys
	KEY_X =			0x00000020,
	KEY_Z =			0x00000040,
	//Left and right weapon switch keys
	KEY_ARMS =		0x00000080,
	KEY_ARMSREV =	0x00000100,
	//Function keys
	KEY_F1 =		0x00000400,
	KEY_F2 =		0x00000800,
	//Inventory
	KEY_ITEM =		0x00001000,
	//Escape key
	KEY_ESCAPE =	0x00008000,
	//The alt movement keys go in the order of left, up, right and down
	KEY_ALT_LEFT =	0x00010000,
	KEY_ALT_UP =	0x00020000,
	KEY_ALT_RIGHT =	0x00040000,
	KEY_ALT_DOWN =	0x00080000,
};

extern int gKey;
extern int gKeyTrg;

extern int gKeyJump;
extern int gKeyShot;
extern int gKeyArms;
extern int gKeyArmsRev;
extern int gKeyItem;
extern int gKeyMap;
extern int gKeyOk;
extern int gKeyCancel;
extern int gKeyLeft;
extern int gKeyUp;
extern int gKeyRight;
extern int gKeyDown;
