#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "Tags.h"
#include "Back.h"
#include "Frame.h"
#include "Game.h"
#include "Draw.h"
#include "Stage.h"
#include "Map.h"

BACK gBack;
int gWaterY;

bool InitBack(char *fName, int type)
{
	//Get width and height
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s.pbm", gDataPath, fName);
	
	SDL_Surface *temp = SDL_LoadBMP(path);
	if (!temp)
		return false;
	
	gBack.partsW = temp->w;
	gBack.partsH = temp->h;
	
	SDL_FreeSurface(temp);
	
	//Set background stuff and load texture
	gBack.flag = 1;
	if (!ReloadBitmap_File(fName, SURFACE_ID_LEVEL_BACKGROUND))
		return false;
	gBack.type = type;
	gWaterY = 0x1E0000;
	return true;
}

void ActBack()
{
	if (gBack.type == 5)
	{
		gBack.fx += 0xC00;
	}
	else if (gBack.type >= 5 && gBack.type <= 7)
	{
		++gBack.fx;
		gBack.fx %= 640;
	}
}

void PutBack(int fx, int fy)
{
	RECT rect = {0, 0, gBack.partsW, gBack.partsH};
	
	switch (gBack.type)
	{
		case 0:
			for (int y = 0; y < WINDOW_HEIGHT; y += gBack.partsH)
			{
				for (int x = 0; x < WINDOW_WIDTH; x += gBack.partsW)
					PutBitmap4(&grcGame, x, y, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			}
			break;
			
		case 1:
			for (int y = -(fy / 0x400 % gBack.partsH); y < WINDOW_HEIGHT; y += gBack.partsH)
			{
				for (int x = -(fx / 0x400 % gBack.partsW); x < WINDOW_WIDTH; x += gBack.partsW)
					PutBitmap4(&grcGame, x, y, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			}
			break;
			
		case 2:
			for (int y = -(fy / 0x200 % gBack.partsH); y < WINDOW_HEIGHT; y += gBack.partsH)
			{
				for (int x = -(fx / 0x200 % gBack.partsW); x < WINDOW_WIDTH; x += gBack.partsW)
					PutBitmap4(&grcGame, x, y, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			}
			break;
			
		case 5:
			for (int y = -gBack.partsH; y < WINDOW_HEIGHT; y += gBack.partsH)
			{
				for (int x = -(gBack.fx / 0x200 % gBack.partsW); x < WINDOW_WIDTH; x += gBack.partsW)
					PutBitmap4(&grcGame, x, y, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			}
			break;
			
		case 6:
		case 7:
			rect.top = 0;
			rect.bottom = 88;
			rect.left = 0;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 0, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.top = 88;
			rect.bottom = 123;
			rect.left = gBack.fx / 2;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 88, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.left = 0;
			PutBitmap4(&grcGame, 320 - gBack.fx / 2 % 320, 88, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.top = 123;
			rect.bottom = 146;
			rect.left = gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 123, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.left = 0;
			PutBitmap4(&grcGame, 320 - gBack.fx % 320, 123, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.top = 146;
			rect.bottom = 176;
			rect.left = 2 * gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 146, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.left = 0;
			PutBitmap4(&grcGame, 320 - 2 * gBack.fx % 320, 146, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.top = 176;
			rect.bottom = 240;
			rect.left = 4 * gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 176, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			rect.left = 0;
			PutBitmap4(&grcGame, 320 - 4 * gBack.fx % 320, 176, &rect, SURFACE_ID_LEVEL_BACKGROUND);
			break;
			
		default:
			return;
	}
}

void PutFront(int fx, int fy)
{
	RECT rcWater[2];
	rcWater[0] = {0, 0, 32, 16};
	rcWater[1] = {0, 16, 32, 48};

	if (gBack.type == 3)
	{
		int x_1 = fx / 0x4000;
		int x_2 = fx / 0x4000 + (((WINDOW_WIDTH + 31) >> 5) + 1);
		int y_1 = 0;
		int y_2 = 32;
		
		for (int y = 0; y < y_2; y++)
		{
			int ypos = (y << 14) / 0x200 - fy / 0x200 + gWaterY / 0x200;
			
			if (ypos >= -32)
			{
				if (ypos > WINDOW_HEIGHT)
					break;
				
				for (int x = x_1; x < x_2; x++)
				{
					int xpos = (x << 14) / 0x200 - fx / 0x200;
					PutBitmap3(&grcGame, xpos, ypos, &rcWater[1], SURFACE_ID_LEVEL_BACKGROUND);
					if (!y)
						PutBitmap3(&grcGame, xpos, ypos, rcWater, SURFACE_ID_LEVEL_BACKGROUND);
				}
			}
		}
	}
	
	//Draw black bars
	if (!(g_GameFlags & 8)) //Detect if credits are running
	{
		const bool fromFocus = (gStageNo == 31); //Get if we should only draw around a 320x240 area of the focus point
		
		//Get borders
		const int barLeft = fromFocus ? (*gFrame.tgt_x - (320 << 8)) : -0x1000;
		const int barTop = fromFocus ? (*gFrame.tgt_y - (240 << 8)) : -0x1000;
		
		const int barRight = fromFocus ? (*gFrame.tgt_x + (320 << 8)) : (gMap.width << 13) - 0x1000;
		const int barBottom = fromFocus ? (*gFrame.tgt_y + (240 << 8)) : (gMap.length << 13) - 0x1000;
		
		//Draw bars
		RECT barRect;
		
		//Left
		barRect.left = 0;
		barRect.top = 0;
		barRect.right = (barLeft - gFrame.x) >> 9;
		barRect.bottom = WINDOW_HEIGHT;
		CortBox(&barRect, 0x000000);
		
		//Top
		barRect.left = 0;
		barRect.top = 0;
		barRect.right = WINDOW_WIDTH;
		barRect.bottom = (barTop - gFrame.y) >> 9;
		CortBox(&barRect, 0x000000);
		
		//Right
		barRect.left = (barRight - gFrame.x) >> 9;
		barRect.top = 0;
		barRect.right = WINDOW_WIDTH;
		barRect.bottom = WINDOW_HEIGHT;
		CortBox(&barRect, 0x000000);
		
		//Bottom
		barRect.left = 0;
		barRect.top = (barBottom - gFrame.y) >> 9;
		barRect.right = WINDOW_WIDTH;
		barRect.bottom = WINDOW_HEIGHT;
		CortBox(&barRect, 0x000000);
	}
}
