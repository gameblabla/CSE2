#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "MapName.h"
#include "Draw.h"
#include "Escape.h"
#include "Game.h"
#include "KeyControl.h"
#include "Map.h"
#include "MyChar.h"
#include "Main.h"

void WriteMiniMapLine(int line)
{
	RECT rcLevel[4];
	rcLevel[0] = {240, 24, 241, 25};
	rcLevel[1] = {241, 24, 242, 25};
	rcLevel[2] = {242, 24, 243, 25};
	rcLevel[3] = {243, 24, 244, 25};
	
	for (int x = 0; x < gMap.width; x++)
	{
		uint8_t a = GetAttribute(x, line);
				
		switch (a)
		{
			case 0:
				Surface2Surface(x, line, &rcLevel[0], 9, 26);
				break;

			case 68:
			case 1:
			case 64:
			case 128:
			case 129:
			case 130:
			case 131:
			case 81:
			case 82:
			case 85:
			case 86:
			case 2:
			case 96:
			case 113:
			case 114:
			case 117:
			case 118:
			case 160:
			case 161:
			case 162:
			case 163:
				Surface2Surface(x, line, &rcLevel[1], 9, 26);
				break;
				
			case 67:
			case 99:
			case 80:
			case 83:
			case 84:
			case 87:
			case 112:
			case 115:
			case 116:
			case 119:
				Surface2Surface(x, line, &rcLevel[2], 9, 26);
				break;
				
			default:
				Surface2Surface(x, line, &rcLevel[3], 9, 26);
				break;
		}
	}
	return;
}

int MiniMapLoop()
{
	RECT my_rect = {0, 57, 1, 58};
	int my_x = (gMC.x / 0x200 + 8) / 16;
	int my_y = (gMC.y / 0x200 + 8) / 16;
	
	RECT rcView;
	for (int f = 0; f <= 8; f++)
	{
		GetTrg();
		
		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 2;
		}
		
		PutBitmap4(&grcGame, 0, 0, &grcGame, 10);
		
		rcView = {(WINDOW_WIDTH / 2) - f * gMap.width / 16, (WINDOW_HEIGHT / 2) - f * gMap.length / 16, (WINDOW_WIDTH / 2) + f * gMap.width / 16, (WINDOW_HEIGHT / 2) + f * gMap.length / 16};
		PutMapName(true);
		CortBox(&rcView, 0);
		
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}
	
	RECT rcMiniMap = {0, 0, gMap.width, gMap.length};
	rcView.right = --rcView.left + gMap.width + 2;
	rcView.bottom = --rcView.top + gMap.length + 2;
	CortBox2(&rcMiniMap, 0, 9);
	
	int line = 0;
	uint8_t my_wait = 0;
	while (true)
	{
		GetTrg();
		
		if ((gKeyCancel | gKeyOk) & gKeyTrg)
			break;
		
		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 2;
		}
		
		PutBitmap4(&grcGame, 0, 0, &grcGame, 10);
		CortBox(&rcView, 0);
		
		if (gMap.length > line)
			WriteMiniMapLine(line++);
		if (gMap.length > line)
			WriteMiniMapLine(line++);
		PutBitmap3(&grcGame, rcView.left + 1, rcView.top + 1, &rcMiniMap, 9);
		
		PutMapName(true);
		
		if ((++my_wait >> 3) & 1)
			PutBitmap3(&grcGame, my_x + rcView.left + 1, my_y + rcView.top + 1, &my_rect, 26);
		
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}
	
	for (int f = 8; f >= -1; --f)
	{
		GetTrg();

		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 2;
		}

		PutBitmap4(&grcGame, 0, 0, &grcGame, 10);
		rcView.left = 160 - f * gMap.width / 16;
		rcView.right = f * gMap.width / 16 + 160;
		rcView.top = 120 - f * gMap.length / 16;
		rcView.bottom = f * gMap.length / 16 + 120;
		PutMapName(true);
		CortBox(&rcView, 0);

		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}

	return 1;
}