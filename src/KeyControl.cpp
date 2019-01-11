#include "KeyControl.h"

int gKey;
int gKeyTrg;

int gKeyJump = KEY_Z;
int gKeyShot = KEY_X;
int gKeyArms = KEY_ARMS;
int gKeyArmsRev = KEY_ARMSREV;
int gKeyItem = KEY_ITEM;
int gKeyMap = KEY_MAP;
int gKeyOk = KEY_Z;
int gKeyCancel = KEY_X;
int gKeyLeft = KEY_LEFT;
int gKeyUp = KEY_UP;
int gKeyRight = KEY_RIGHT;
int gKeyDown = KEY_DOWN;

void GetTrg()
{
	static int key_old;
	gKeyTrg = gKey & (gKey ^ key_old);
	key_old = gKey;
}
