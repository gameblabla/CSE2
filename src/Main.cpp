#include "Main.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BOOL BOOL_OGC
#include <fat.h>
#undef BOOL

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Config.h"
#include "Draw.h"
#include "Game.h"
#include "Generic.h"
#include "Input.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "Organya.h"
#include "Profile.h"
#include "Resource.h"
#include "Sound.h"
#include "Triangle.h"
#include "Types.h"

char gModulePath[PATH_LENGTH];
char gDataPath[PATH_LENGTH];

int gJoystickButtonTable[8];

bool gbUseJoystick = false;
bool bFps = false;

bool bActive = true;

#ifdef JAPANESE
const char *lpWindowName = "洞窟物語エンジン2";
#else
const char *lpWindowName = "Cave Story Engine 2 ~ Doukutsu Monogatari Enjin 2";
#endif

//A replication of MSVC's rand algorithm
static uint32_t next = 1;

int rep_rand()
{
	next = ((next) * 214013 + 2531011);
    return ((next) >> 16) & 0x7FFF;
}

void rep_srand(uint32_t seed)
{
    next = seed;
}

//Framerate stuff
void PutFramePerSecound()
{
	if (bFps)
		PutNumber4(WINDOW_WIDTH - 40, 8, GetFramePerSecound(), false);
}

int GetFramePerSecound()
{
	return 60;
}

int main(int argc, char *argv[])
{
	//Get executable's path
	fatInitDefault();
	strcpy(gModulePath, "sd:/cse2");
	
	//Get path of the data folder
	strcpy(gDataPath, gModulePath);
	strcat(gDataPath, "/data");
	
	//Load configuration
	CONFIG config;
	
	if (!LoadConfigData(&config))
		DefaultConfigData(&config);
	
	//Apply keybinds
	//Swap X and Z buttons
	if (config.attack_button_mode)
	{
		if (config.attack_button_mode == 1)
		{
			gKeyJump = KEY_X;
			gKeyShot = KEY_Z;
		}
	}
	else
	{
		gKeyJump = KEY_Z;
		gKeyShot = KEY_X;
	}
	
	//Swap Okay and Cancel buttons
	if (config.ok_button_mode)
	{
		if (config.ok_button_mode == 1)
		{
			gKeyOk = gKeyShot;
			gKeyCancel = gKeyJump;
		}
	}
	else
	{
		gKeyOk = gKeyJump;
		gKeyCancel = gKeyShot;
	}
	
	//Swap left and right weapon switch keys
	if (CheckFileExists("s_reverse"))
	{
		gKeyArms = KEY_ARMSREV;
		gKeyArmsRev = KEY_ARMS;
	}
	
	//Alternate movement keys
	if (config.move_button_mode)
	{
		if (config.move_button_mode == 1)
		{
			gKeyLeft = KEY_ALT_LEFT;
			gKeyUp = KEY_ALT_UP;
			gKeyRight = KEY_ALT_RIGHT;
			gKeyDown = KEY_ALT_DOWN;
		}
	}
	else
	{
		gKeyLeft = KEY_LEFT;
		gKeyUp = KEY_UP;
		gKeyRight = KEY_RIGHT;
		gKeyDown = KEY_DOWN;
	}
	
	//Initialize rendering
	StartDirectDraw();
	
	//Initialize input
	InitDirectInput();
	
	//Check debug things
	if (CheckFileExists("fps"))
		bFps = true;

	//Set rects
	RECT loading_rect = {0, 0, 64, 8};
	RECT clip_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	
	//Load the "LOADING" text
	MakeSurface_File("Loading", SURFACE_ID_LOADING);
	
	//Draw loading screen
	CortBox(&clip_rect, 0x000000);
	PutBitmap3(&clip_rect, (WINDOW_WIDTH - 64) / 2, (WINDOW_HEIGHT - 8) / 2, &loading_rect, SURFACE_ID_LOADING);
	
	//Draw to screen
	if (Flip_SystemTask())
	{
		//Initialize sound
		InitDirectSound();
		
		//Initialize stuff
		InitTextObject();
		InitTriangleTable();
		
		//Run game code
		Game();
		
		//End stuff
		EndDirectSound();
		EndTextObject();
		EndDirectDraw();
	}
	
	return 0;
}

bool SystemTask()
{
	return true;
}
