#include "MapName.h"

#include <stdint.h>
#include <string.h>

#include "CommonDefines.h"
#include "Draw.h"

MAP_NAME gMapName;
RECT rc = { 0, 0, 160, 12 };

void ReadyMapName(const char *str)
{
	//Reset map name flags
	gMapName.flag = 0;
	gMapName.wait = 0;
	
	//Handle "Studio Pixel presents" text in the intro
	#ifdef JAPANESE
	char presentText[24] = "ŠJ”­ŽºPixel presents";
	#else
	char presentText[24] = "  Studio Pixel presents";
	#endif
	
	if (!strcmp(str, "u"))
	{
		/*for (i = 0; i < strlen(presentText); i++) //No need for this, we aren't encrypting the text
			++studio_pixel_presents_string[i];*/
		str = presentText;
	}
	
	//Copy map's name to the MapName
	strcpy(gMapName.name, str);
	
	//Draw the text to the surface
	int len = strlen(gMapName.name);
	
	CortBox2(&rc, 0, SURFACE_ID_ROOM_NAME);
	PutText2((-6 * len + 160) / 2 + 6, 1, gMapName.name, RGB(0x11, 0x00, 0x22), SURFACE_ID_ROOM_NAME);
	PutText2((-6 * len + 160) / 2 + 6, 0, gMapName.name, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_ROOM_NAME);
}

void PutMapName(bool bMini)
{
	if (bMini)
	{
		//Map system
		RECT rcBack = {0, 7, WINDOW_WIDTH, 24};
		CortBox(&rcBack, 0x000000);
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 172) / 2, 10, &rc, SURFACE_ID_ROOM_NAME);
	}
	else if (gMapName.flag)
	{
		//MNA
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 172) / 2, (WINDOW_HEIGHT - 80) / 2, &rc, SURFACE_ID_ROOM_NAME);
		if (++gMapName.wait > 160)
			gMapName.flag = 0;
	}
}

void StartMapName()
{
	gMapName.flag = 1;
	gMapName.wait = 0;
}

void RestoreMapName()
{
	int len = strlen(gMapName.name);
	
	CortBox2(&rc, 0, SURFACE_ID_ROOM_NAME);
	PutText2((-6 * len + 160) / 2 + 6, 1, gMapName.name, RGB(0x11, 0x00, 0x22), SURFACE_ID_ROOM_NAME);
	PutText2((-6 * len + 160) / 2 + 6, 0, gMapName.name, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_ROOM_NAME);
}
