#include "Map.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "File.h"
#include "NpChar.h"
#include "Tags.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define PXM_BUFFER_SIZE 0x4B000

MAP_DATA gMap;

static const char *code_pxma = "PXM";

BOOL InitMapData2(void)
{
	gMap.data = (unsigned char*)malloc(PXM_BUFFER_SIZE);
	return TRUE;
}

BOOL LoadMapData2(const char *path_map)
{
	unsigned char dum;

	// Get path
	char path[MAX_PATH];
	sprintf(path, "%s/%s", gDataPath, path_map);

	// Open file
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Make sure file begins with "PXM"
	char check[3];
	fread(check, 1, 3, fp);

	if (memcmp(check, code_pxma, 3))
	{
		fclose(fp);
		return FALSE;
	}

	fread(&dum, 1, 1, fp);
	// Get width and height
	gMap.width = File_ReadLE16(fp);
	gMap.length = File_ReadLE16(fp);

	if (gMap.data == NULL)
	{
		fclose(fp);
		return FALSE;
	}

	// Read tile data
	fread(gMap.data, 1, gMap.width * gMap.length, fp);
	fclose(fp);
	return TRUE;
}

BOOL LoadAttributeData(const char *path_atrb)
{
	// Open file
	char path[MAX_PATH];
	sprintf(path, "%s/%s", gDataPath, path_atrb);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Read data
	fread(gMap.atrb, 1, sizeof(gMap.atrb), fp);
	fclose(fp);
	return TRUE;
}

void EndMapData(void)
{
	free(gMap.data);
}

void ReleasePartsImage(void)
{
	ReleaseSurface(SURFACE_ID_LEVEL_TILESET);
}

void GetMapData(unsigned char **data, short *mw, short *ml)
{
	if (data != NULL)
		*data = gMap.data;

	if (mw != NULL)
		*mw = gMap.width;

	if (ml != NULL)
		*ml = gMap.length;
}

unsigned char GetAttribute(int x, int y)
{
	if (x < 0 || y < 0 || x >= gMap.width || y >= gMap.length)
		return 0;

	const size_t a = *(gMap.data + x + (y * gMap.width));	// Yes, the original code really does do this instead of a regular array access
	return gMap.atrb[a];
}

void DeleteMapParts(int x, int y)
{
	*(gMap.data + x + (y * gMap.width)) = 0;
}

void ShiftMapParts(int x, int y)
{
	*(gMap.data + x + (y * gMap.width)) -= 1;
}

BOOL ChangeMapParts(int x, int y, unsigned char no)
{
	int i;

	if (*(gMap.data + x + (y * gMap.width)) == no)
		return FALSE;

	*(gMap.data + x + (y * gMap.width)) = no;

	for (i = 0; i < 3; ++i)
		SetNpChar(4, x * 0x200 * 0x10, y * 0x200 * 0x10, 0, 0, 0, NULL, 0);

	return TRUE;
}

void PutStage_Back(int fx, int fy)
{
	int num_y;
	int put_x;
	int put_y;
	int i;
	int j;
	int offset;
	int atrb;
	RECT rect;
	int num_x;

	// Get range to draw
	num_x = MIN(gMap.width, ((WINDOW_WIDTH + (16 - 1)) / 16) + 1);
	num_y = MIN(gMap.length, ((WINDOW_HEIGHT + (16 - 1)) / 16) + 1);
	put_x = MAX(0, ((fx / 0x200) + 8) / 16);
	put_y = MAX(0, ((fy / 0x200) + 8) / 16);

	for (j = put_y; j < put_y + num_y; ++j)
	{
		for (i = put_x; i < put_x + num_x; ++i)
		{
			// Get attribute
			offset = (j * gMap.width) + i;
			atrb = GetAttribute(i, j);

			if (atrb >= 0x20)
				continue;

			// Draw tile
			rect.left = (gMap.data[offset] % 16) * 16;
			rect.top = (gMap.data[offset] / 16) * 16;
			rect.right = rect.left + 16;
			rect.bottom = rect.top + 16;

			PutBitmap3(&grcGame, PixelToScreenCoord((i * 16) - 8) - SubpixelToScreenCoord(fx), PixelToScreenCoord((j * 16) - 8) - SubpixelToScreenCoord(fy), &rect, SURFACE_ID_LEVEL_TILESET);
		}
	}
}

void PutStage_Front(int fx, int fy)
{
	RECT rcSnack = {256, 48, 272, 64};
	int num_y;
	int put_x;
	int put_y;
	int j;
	int i;
	int offset;
	int atrb;
	RECT rect;
	int num_x;

	// Get range to draw
	num_x = MIN(gMap.width, ((WINDOW_WIDTH + (16 - 1)) / 16) + 1);
	num_y = MIN(gMap.length, ((WINDOW_HEIGHT + (16 - 1)) / 16) + 1);
	put_x = MAX(0, ((fx / 0x200) + 8) / 16);
	put_y = MAX(0, ((fy / 0x200) + 8) / 16);

	for (j = put_y; j < put_y + num_y; ++j)
	{
		for (i = put_x; i < put_x + num_x; ++i)
		{
			// Get attribute
			offset = (j * gMap.width) + i;
			atrb = GetAttribute(i, j);

			if (atrb < 0x40 || atrb >= 0x80)
				continue;

			// Draw tile
			rect.left = (gMap.data[offset] % 16) * 16;
			rect.top = (gMap.data[offset] / 16) * 16;
			rect.right = rect.left + 16;
			rect.bottom = rect.top + 16;

			PutBitmap3(&grcGame, PixelToScreenCoord((i * 16) - 8) - SubpixelToScreenCoord(fx), PixelToScreenCoord((j * 16) - 8) - SubpixelToScreenCoord(fy), &rect, SURFACE_ID_LEVEL_TILESET);

			if (atrb == 0x43)
				PutBitmap3(&grcGame, PixelToScreenCoord((i * 16) - 8) - SubpixelToScreenCoord(fx), PixelToScreenCoord((j * 16) - 8) - SubpixelToScreenCoord(fy), &rcSnack, SURFACE_ID_NPC_SYM);
		}
	}
}

void PutMapDataVector(int fx, int fy)
{
	int num_y;
	int put_x;
	int put_y;
	int i;
	int j;
	int offset;
	int atrb;
	RECT rect;
	int num_x;

	// Animate the wind
	static unsigned char count = 0;
	count += 2;

	// Get range to draw
	num_x = MIN(gMap.width, ((WINDOW_WIDTH + (16 - 1)) / 16) + 1);
	num_y = MIN(gMap.length, ((WINDOW_HEIGHT + (16 - 1)) / 16) + 1);
	put_x = MAX(0, ((fx / 0x200) + 8) / 16);
	put_y = MAX(0, ((fy / 0x200) + 8) / 16);

	for (j = put_y; j < put_y + num_y; ++j)
	{
		for (i = put_x; i < put_x + num_x; ++i)
		{
			// Get attribute
			offset = (j * gMap.width) + i;
			atrb = GetAttribute(i, j);

			if (atrb != 0x80
				&& atrb != 0x81
				&& atrb != 0x82
				&& atrb != 0x83
				&& atrb != 0xA0
				&& atrb != 0xA1
				&& atrb != 0xA2
				&& atrb != 0xA3)
				continue;

			switch (atrb)
			{
				case 128:
				case 160:
					rect.left = 224 + (count % 16);
					rect.right = rect.left + 16;
					rect.top = 48;
					rect.bottom = rect.top + 16;
					break;

				case 129:
				case 161:
					rect.left = 224;
					rect.right = rect.left + 16;
					rect.top = 48 + (count % 16);
					rect.bottom = rect.top + 16;
					break;

				case 130:
				case 162:
					rect.left = 240 - (count % 16);
					rect.right = rect.left + 16;
					rect.top = 48;
					rect.bottom = rect.top + 16;
					break;

				case 131:
				case 163:
					rect.left = 224;
					rect.right = rect.left + 16;
					rect.top = 64 - (count % 16);
					rect.bottom = rect.top + 16;
					break;
			}

			PutBitmap3(&grcGame, PixelToScreenCoord((i * 16) - 8) - SubpixelToScreenCoord(fx), PixelToScreenCoord((j * 16) - 8) - SubpixelToScreenCoord(fy), &rect, SURFACE_ID_CARET);
		}
	}
}
