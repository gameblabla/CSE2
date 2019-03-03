#include "Back.h"

#include <stdio.h>

#include "WindowsWrapper.h"

#include "Draw.h"
#include "File.h"
#include "Frame.h"
#include "Game.h"
#include "Map.h"
#include "Stage.h"
#include "Tags.h"

BACK gBack;
int gWaterY;
static unsigned long color_black;

BOOL InitBack(const char *fName, int type)
{
	// Unused, hilariously
	color_black = GetCortBoxColor(RGB(0, 0, 0x10));

	//Get width and height
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s.pbm", gDataPath, fName);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
	{
		sprintf(path, "%s/%s.bmp", gDataPath, fName);
		fp = fopen(path, "rb");
		if (fp == NULL)
			return FALSE;
	}

#ifdef NONPORTABLE
	// This is ridiculously platform-dependant:
	// It should break on big-endian CPUs, and platforms
	// where short isn't 16-bit and long isn't 32-bit.
	short bmp_header_buffer[7];
	long bmp_header_buffer2[10];

	fread(bmp_header_buffer, 14, 1, fp);

	// Check if this is a valid bitmap file
	if (bmp_header_buffer[0] != 0x4D42)	// 'MB' (we use hex to prevent a compiler warning)
	{
#ifdef FIX_BUGS
		// The original game forgets to close fp
		fclose(fp);
#endif
		return FALSE;
	}

	fread(bmp_header_buffer2, 40, 1, fp);
	fclose(fp);

	gBack.partsW = bmp_header_buffer2[1];
	gBack.partsH = bmp_header_buffer2[2];
#else
	if (fgetc(fp) != 'B' || fgetc(fp) != 'M')
	{
		fclose(fp);
		return FALSE;
	}

	fseek(fp, 18, SEEK_SET);

	gBack.partsW = File_ReadLE32(fp);
	gBack.partsH = File_ReadLE32(fp);
	fclose(fp);
#endif

	//Set background stuff and load texture
	gBack.flag = 1;
	if (!ReloadBitmap_File(fName, SURFACE_ID_LEVEL_BACKGROUND))
		return FALSE;
	gBack.type = type;
	gWaterY = 0x1E0000;
	return TRUE;
}

void ActBack()
{
	switch (gBack.type)
	{
		case 5:
			gBack.fx += 0xC00;
			break;

		case 6:
		case 7:
			++gBack.fx;
			gBack.fx %= 640;
			break;
	}
}

void PutBack(int fx, int fy)
{
	RECT rect = {0, 0, gBack.partsW, gBack.partsH};
	RECT rcSkyFiller = {106, 0, 255, 88};
	
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
			//Sky
			static unsigned int fillNext;
			fillNext = 0;
			for (int y = 0; y < WINDOW_HEIGHT - 240 + 88; y += 88)
			{
				fillNext = ((fillNext) * 214013 + 2531011);
				for (int x = -(int)(fillNext % 149); x < WINDOW_WIDTH; x += 149)
				{
					PutBitmap4(&grcGame, x, y, &rcSkyFiller, SURFACE_ID_LEVEL_BACKGROUND);
				}
			}
			
			rect.top = 0;
			rect.bottom = 88;
			rect.left = 0;
			rect.right = 320;
			PutBitmap4(&grcGame, (WINDOW_WIDTH - 320) / 2, WINDOW_HEIGHT - 240, &rect, SURFACE_ID_LEVEL_BACKGROUND);

			//Cloud layer 1
			rect.top = 88;
			rect.bottom = 123;
			rect.left = gBack.fx / 2;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 88 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			rect.left = 0;
			for (int i = 0; i < ((WINDOW_WIDTH - 1) / 320) + 1; i++)
				PutBitmap4(&grcGame, (320 * (i + 1)) - gBack.fx / 2 % 320, 88 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			//Cloud layer 2
			rect.top = 123;
			rect.bottom = 146;
			rect.left = gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 123 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			rect.left = 0;
			for (int i = 0; i < ((WINDOW_WIDTH - 1) / 320) + 1; i++)
				PutBitmap4(&grcGame, (320 * (i + 1)) - gBack.fx % 320, 123 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			//Cloud layer 3
			rect.top = 146;
			rect.bottom = 176;
			rect.left = 2 * gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 146 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			rect.left = 0;
			for (int i = 0; i < ((WINDOW_WIDTH - 1) / 320) + 1; i++)
				PutBitmap4(&grcGame, (320 * (i + 1)) - 2 * gBack.fx % 320, 146 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			//Cloud layer 4
			rect.top = 176;
			rect.bottom = 240;
			rect.left = 4 * gBack.fx % 320;
			rect.right = 320;
			PutBitmap4(&grcGame, 0, 176 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			
			rect.left = 0;
			for (int i = 0; i < ((WINDOW_WIDTH - 1) / 320) + 1; i++)
				PutBitmap4(&grcGame, (320 * (i + 1)) - 4 * gBack.fx % 320, 176 + (WINDOW_HEIGHT - 240), &rect, SURFACE_ID_LEVEL_BACKGROUND);
			break;
			
		default:
			return;
	}
}

void PutFront(int fx, int fy)
{
	RECT rcWater[2] = {{0, 0, 32, 16}, {0, 16, 32, 48}};

	switch (gBack.type)
	{
		case 3:
			int x_1 = fx / 0x4000;
			int x_2 = x_1 + (((WINDOW_WIDTH + 31) >> 5) + 1);
			int y_1 = 0;
			int y_2 = y_1 + 32;
			
			for (int y = y_1; y < y_2; y++)
			{
				int ypos = (y * 0x20 * 0x200) / 0x200 - fy / 0x200 + gWaterY / 0x200;
				
				if (ypos < -32)
					break;

				if (ypos > WINDOW_HEIGHT)
					break;
					
				for (int x = x_1; x < x_2; x++)
				{
					int xpos = (x * 0x20 * 0x200) / 0x200 - fx / 0x200;
					PutBitmap3(&grcGame, xpos, ypos, &rcWater[1], SURFACE_ID_LEVEL_BACKGROUND);
					if (!y)
						PutBitmap3(&grcGame, xpos, ypos, rcWater, SURFACE_ID_LEVEL_BACKGROUND);
				}
			}

	}
	
	//Draw black bars
	if (!(g_GameFlags & 8)) //Detect if credits are running
	{
		const bool fromFocus = (gStageNo == 31); //Get if we should only draw around a 320x240 area of the focus point
		
		//Get focus rect
		int focusX = gFrame.x + (WINDOW_WIDTH << 8) - (320 << 8);
		int focusY = gFrame.y + (WINDOW_HEIGHT << 8) - (240 << 8);
		int focusR = focusX + (320 << 9);
		int focusB = focusY + (240 << 9);
		
		//Get borders
		const int barLeft = fromFocus ? focusX : -0x1000;
		const int barTop = fromFocus ? focusY : -0x1000;
		
		const int barRight = fromFocus ? focusR : (gMap.width << 13) - 0x1000;
		const int barBottom = fromFocus ? focusB : (gMap.length << 13) - 0x1000;
		
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
