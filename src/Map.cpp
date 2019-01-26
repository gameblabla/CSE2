#include <stdint.h>
#include <string>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Map.h"
#include "Tags.h"
#include "Draw.h"

#define PXM_BUFFER_SIZE 0x4B000

MAP_DATA gMap;

bool InitMapData2()
{
	gMap.data = (uint8_t*)malloc(PXM_BUFFER_SIZE);
	return true;
}

bool LoadMapData2(char *path_map)
{
	//Get path
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, path_map);
	
	//Open file
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Make sure file begins with "PXM"
	char check[3];
	fp->read(fp, check, 1, 3);
	
	if (!memcmp(check, "PXM", 3))
	{
		uint8_t nul;
		fp->read(fp, &nul, 1, 1);
		
		//Get width and height
		gMap.width = SDL_ReadLE16(fp);
		gMap.length = SDL_ReadLE16(fp);
		
		if (gMap.data)
		{
			//Read tiledata
			fp->read(fp, gMap.data, 1, gMap.length * gMap.width);
			SDL_RWclose(fp);
			return true;
		}
		else
		{
			SDL_RWclose(fp);
			return false;
		}
	}
	else
	{
		SDL_RWclose(fp);
		return false;
	}
	
	return false;
}

bool LoadAttributeData(char *path_atrb)
{
	//Open file
	char path[260];
	sprintf(path, "%s/%s", gDataPath, path_atrb);
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Read data
	fp->read(fp, gMap.atrb, 1, 0x100);
	SDL_RWclose(fp);
	return true;
}

void EndMapData()
{
	if (gMap.data)
		free(gMap.data);
}

void ReleasePartsImage()
{
	ReleaseSurface(SURFACE_ID_LEVEL_TILESET);
}

void GetMapData(uint8_t **data, int16_t *mw, int16_t *ml)
{
	if (data)
		*data = gMap.data;
	if (mw)
		*mw = gMap.width;
	if (ml)
		*ml = gMap.length;
}

int GetAttribute(int x, int y)
{
	if (x >= 0 && y >= 0 && gMap.width > x && gMap.length > y)
		return gMap.atrb[gMap.data[y * gMap.width + x]];
	return false;
}

void DeleteMapParts(int x, int y)
{
  gMap.data[y * gMap.width + x] = 0;
}

void ShiftMapParts(int x, int y)
{
	--gMap.data[y * gMap.width + x];
}

bool ChangeMapParts(int x, int y, uint8_t no)
{
	if ( gMap.data[y * gMap.width + x] == no )
		return false;
	gMap.data[y * gMap.width + x] = no;
	//for (int i = 0; i <= 2; ++i )
	//	SetNpChar(4, x << 13, y << 13, 0, 0, 0, 0, 0);
	return true;
}

void PutStage_Back(int fx, int fy)
{
	//Get range to draw
	int num_x = (WINDOW_WIDTH >> 4) + 1;
	int num_y = (WINDOW_HEIGHT >> 4) + 1;
	int put_x = (fx / 0x200 + 8) / 16;
	int put_y = (fy / 0x200 + 8) / 16;
	
	for (int j = put_y; put_y + num_y > j; j++)
	{
		for (int i = put_x; put_x + num_x > i; i++)
		{
			//Get attribute
			int offset = i + j * gMap.width;
			int atrb = GetAttribute(i, j);
			
			if (atrb < 0x20)
			{
				//Draw tile
				RECT rect;
				rect.left = 16 * (gMap.data[offset] & 0xF);
				rect.top = 16 * (gMap.data[offset] >> 4);
				rect.right = rect.left + 16;
				rect.bottom = rect.top + 16;
				
				PutBitmap3(&grcGame, 8 * (2 * i - 1) - fx / 0x200, 8 * (2 * j - 1) - fy / 0x200, &rect, SURFACE_ID_LEVEL_TILESET);
			}
		}
	}
}

void PutStage_Front(int fx, int fy)
{
	RECT rcSnack = {256, 48, 272, 64};
	
	//Get range to draw
	int num_x = (WINDOW_WIDTH >> 4) + 1;
	int num_y = (WINDOW_HEIGHT >> 4) + 1;
	int put_x = (fx / 0x200 + 8) / 16;
	int put_y = (fy / 0x200 + 8) / 16;
	
	for (int j = put_y; put_y + num_y > j; j++)
	{
		for (int i = put_x; put_x + num_x > i; i++)
		{
			//Get attribute
			int offset = i + j * gMap.width;
			int atrb = GetAttribute(i, j);
			
			if (atrb >= 0x40 && atrb < 0x80)
			{
				//Draw tile
				RECT rect;
				rect.left = 16 * (gMap.data[offset] & 0xF);
				rect.top = 16 * (gMap.data[offset] >> 4);
				rect.right = rect.left + 16;
				rect.bottom = rect.top + 16;
				
				PutBitmap3(&grcGame, 8 * (2 * i - 1) - fx / 0x200, 8 * (2 * j - 1) - fy / 0x200, &rect, SURFACE_ID_LEVEL_TILESET);
				
				if (atrb == 0x43)
					PutBitmap3(&grcGame, 8 * (2 * i - 1) - fx / 0x200, 8 * (2 * j - 1) - fy / 0x200, &rcSnack, SURFACE_ID_NPC_SYM);
			}
		}
	}
}

void PutMapDataVector(int fx, int fy)
{
	//Get range to draw
	int num_x = (WINDOW_WIDTH >> 4) + 1;
	int num_y = (WINDOW_HEIGHT >> 4) + 1;
	int put_x = (fx / 0x200 + 8) / 16;
	int put_y = (fy / 0x200 + 8) / 16;
	
	//Animate the wind
	static int count = 0;
	count++;
	
	for (int j = put_y; put_y + num_y > j; j++)
	{
		for (int i = put_x; put_x + num_x > i; i++)
		{
			//Get attribute
			int offset = i + j * gMap.width;
			int atrb = GetAttribute(i, j);
			
			if (   atrb == 0x80
				|| atrb == 0x81
				|| atrb == 0x82
				|| atrb == 0x83
				|| atrb == 0xA0
				|| atrb == 0xA1
				|| atrb == 0xA2
				|| atrb == 0xA3)
			{
				RECT rect;
				
				switch ( atrb )
				{
					case 128:
					case 160:
						rect.left = (count & 0xF) + 224;
						rect.right = (count & 0xF) + 240;
						rect.top = 48;
						rect.bottom = 64;
						break;
					case 129:
					case 161:
						rect.left = 224;
						rect.right = 240;
						rect.top = (count & 0xF) + 48;
						rect.bottom = (count & 0xF) + 64;
						break;
					case 130:
					case 162:
						rect.left = 240 - (count & 0xF);
						rect.right = rect.left + 16;
						rect.top = 48;
						rect.bottom = 64;
						break;
					case 131:
					case 163:
						rect.left = 224;
						rect.right = 240;
						rect.top = 64 - (count & 0xF);
						rect.bottom = rect.top + 16;
						break;
					default:
						break;
				}
				
				PutBitmap3(&grcGame, 8 * (2 * i - 1) - fx / 0x200, 8 * (2 * j - 1) - fy / 0x200, &rect, SURFACE_ID_CARET);
			}
		}
	}
}
