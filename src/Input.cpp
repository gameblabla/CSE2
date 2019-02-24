#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <wiiuse/wpad.h>

#include "WindowsWrapper.h"

#include "Input.h"
#include "KeyControl.h"
#include "CommonDefines.h"
#include "Tags.h"
#include "Types.h"

void ReleaseDirectInput()
{
	//Nothing
}

bool InitDirectInput()
{
	WPAD_Init();
	return true;
}

bool UpdateInput()
{
	WPAD_ScanPads();
	uint32_t wpadHeld = WPAD_ButtonsHeld(0);
	
	//Clear all keys
	gKey = 0;
	
	//Escape key
	if (wpadHeld & WPAD_BUTTON_HOME)
		gKey |= KEY_ESCAPE;
	
	//Direction
	if (!(wpadHeld & WPAD_BUTTON_B))
	{
		gKey |= (wpadHeld & WPAD_BUTTON_UP)	? gKeyLeft : 0;
		gKey |= (wpadHeld & WPAD_BUTTON_DOWN)	? gKeyRight : 0;
	}
	
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_LEFT)	? gKeyLeft : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_RIGHT)	? gKeyRight : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_RIGHT	|| wpadHeld & WPAD_CLASSIC_BUTTON_UP)		? gKeyUp : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_LEFT	|| wpadHeld & WPAD_CLASSIC_BUTTON_DOWN)	? gKeyDown : 0;
	
	//Jump and shoot
	gKey |= (wpadHeld & WPAD_BUTTON_2		|| wpadHeld & WPAD_CLASSIC_BUTTON_B)		? gKeyJump : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_1		|| wpadHeld & WPAD_CLASSIC_BUTTON_Y)		? gKeyShot : 0;
	
	//Inventory and map system key
	gKey |= (wpadHeld & WPAD_BUTTON_PLUS	|| wpadHeld & WPAD_CLASSIC_BUTTON_PLUS)	? gKeyItem : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_MINUS	|| wpadHeld & WPAD_CLASSIC_BUTTON_MINUS)	? gKeyMap : 0;
	
	//Weapon switch keys
	if (wpadHeld & WPAD_BUTTON_B)
	{
		gKey |= (wpadHeld & WPAD_BUTTON_UP)	? gKeyArms : 0;
		gKey |= (wpadHeld & WPAD_BUTTON_DOWN)	? gKeyArmsRev : 0;
	}
	
	gKey |= (wpadHeld & WPAD_CLASSIC_BUTTON_FULL_L)	? gKeyArms : 0;
	gKey |= (wpadHeld & WPAD_CLASSIC_BUTTON_FULL_R)	? gKeyArmsRev : 0;
	return true;
}
