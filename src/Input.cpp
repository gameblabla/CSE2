#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define BOOL BOOL_OGC
#include <wiiuse/wpad.h>
#undef BOOL

#include "WindowsWrapper.h"

#include "Input.h"
#include "KeyControl.h"
#include "CommonDefines.h"
#include "Tags.h"
#include "Types.h"

#define STICK_DEADZONE 0x20
#define STICK_DEADSML  -STICK_DEADZONE
#define STICK_DEADBIG   STICK_DEADZONE

void ReleaseDirectInput()
{
	WPAD_Shutdown();
}

bool InitDirectInput()
{
	WPAD_Init();
	PAD_Init();
	return true;
}

bool UpdateInput()
{
	WPAD_ScanPads();
	PAD_ScanPads();
	
	uint32_t wpadHeld = WPAD_ButtonsHeld(0);
	uint32_t padHeld = PAD_ButtonsHeld(0);
	
	//Clear all keys
	gKey = 0;
	
	/* Wiimote and Wii Classic Controller */
	//Escape key
	if (wpadHeld & WPAD_BUTTON_HOME || wpadHeld & WPAD_CLASSIC_BUTTON_HOME)
		gKey |= KEY_ESCAPE;
	
	//Direction
	if (!(wpadHeld & WPAD_BUTTON_B))
	{
		gKey |= (wpadHeld & WPAD_BUTTON_UP)	? gKeyLeft : 0;
		gKey |= (wpadHeld & WPAD_BUTTON_DOWN)	? gKeyRight : 0;
	}
	
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_LEFT)		? gKeyLeft : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_RIGHT)	? gKeyRight : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_RIGHT	|| wpadHeld & WPAD_CLASSIC_BUTTON_UP)		? gKeyUp : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_LEFT	|| wpadHeld & WPAD_CLASSIC_BUTTON_DOWN)		? gKeyDown : 0;
	
	//Jump and shoot
	gKey |= (wpadHeld & WPAD_BUTTON_2		|| wpadHeld & WPAD_CLASSIC_BUTTON_B)		? gKeyJump : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_1		|| wpadHeld & WPAD_CLASSIC_BUTTON_Y)		? gKeyShot : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_A)		? gKeyJump : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_X)		? gKeyShot : 0;
	
	//Ok and cancel
	gKey |= (wpadHeld & WPAD_BUTTON_2		|| wpadHeld & WPAD_CLASSIC_BUTTON_B)		? gKeyOk : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_1		|| wpadHeld & WPAD_CLASSIC_BUTTON_Y)		? gKeyCancel : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_A)		? gKeyOk : 0;
	gKey |= 								  (wpadHeld & WPAD_CLASSIC_BUTTON_X)		? gKeyCancel : 0;
	
	//Inventory and map system key
	gKey |= (wpadHeld & WPAD_BUTTON_PLUS	|| wpadHeld & WPAD_CLASSIC_BUTTON_PLUS)		? gKeyItem : 0;
	gKey |= (wpadHeld & WPAD_BUTTON_MINUS	|| wpadHeld & WPAD_CLASSIC_BUTTON_MINUS)	? gKeyMap : 0;
	
	//Weapon switch keys
	if (wpadHeld & WPAD_BUTTON_B)
	{
		gKey |= (wpadHeld & WPAD_BUTTON_DOWN)	? gKeyArms : 0;
		gKey |= (wpadHeld & WPAD_BUTTON_UP)		? gKeyArmsRev : 0;
	}
	
	gKey |= (wpadHeld & WPAD_CLASSIC_BUTTON_FULL_R)	? gKeyArms : 0;
	gKey |= (wpadHeld & WPAD_CLASSIC_BUTTON_FULL_L)	? gKeyArmsRev : 0;
	
	/* Gamecube controller */
	if ((padHeld & PAD_BUTTON_START) && (padHeld & PAD_TRIGGER_Z))
		gKey |= KEY_ESCAPE;
	
	//Direction
	gKey |= (padHeld & PAD_BUTTON_LEFT)		? gKeyLeft : 0;
	gKey |= (padHeld & PAD_BUTTON_RIGHT)	? gKeyRight : 0;
	gKey |= (padHeld & PAD_BUTTON_UP)		? gKeyUp : 0;
	gKey |= (padHeld & PAD_BUTTON_DOWN)		? gKeyDown : 0;
	
	//Analogue stick
	gKey |= (PAD_StickX(0) < STICK_DEADSML)	? gKeyLeft : 0;
	gKey |= (PAD_StickX(0) > STICK_DEADBIG)	? gKeyRight : 0;
	gKey |= (PAD_StickY(0) > STICK_DEADBIG)	? gKeyUp : 0;
	gKey |= (PAD_StickY(0) < STICK_DEADSML)	? gKeyDown : 0;
	
	//Jump and shoot
	gKey |= (padHeld & PAD_BUTTON_A)		? gKeyJump : 0;
	gKey |= (padHeld & PAD_BUTTON_B)		? gKeyShot : 0;
	
	//Ok and cancel
	gKey |= (padHeld & PAD_BUTTON_A)		? gKeyOk : 0;
	gKey |= (padHeld & PAD_BUTTON_B)		? gKeyCancel : 0;
	
	//Inventory and map system key
	gKey |= (padHeld & PAD_BUTTON_START)	? gKeyItem : 0;
	gKey |= (padHeld & PAD_TRIGGER_Z)		? gKeyMap : 0;
	
	//Weapon switch keys
	gKey |= (padHeld & PAD_TRIGGER_R)		? gKeyArms : 0;
	gKey |= (padHeld & PAD_TRIGGER_L)		? gKeyArmsRev : 0;
	return true;
}
