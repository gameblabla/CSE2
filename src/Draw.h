#pragma once
#include <stdint.h>
#include "WindowsWrapper.h"

#ifndef RGB
#define RGB(r,g,b) ((r) | ((g) << 8) | ((b) << 16))
#endif

extern RECT grcGame;
extern RECT grcFull;

extern int magnification;
extern bool fullscreen;

typedef enum Surface_Ids
{
	SURFACE_ID_TITLE = 0,
	SURFACE_ID_PIXEL = 1,
	SURFACE_ID_LEVEL_TILESET = 2,
	SURFACE_ID_FADE = 6,
	SURFACE_ID_ITEM_IMAGE = 8,
	SURFACE_ID_MAP = 9,
	SURFACE_ID_SCREEN_GRAB = 10,
	SURFACE_ID_ARMS = 11,
	SURFACE_ID_ARMS_IMAGE = 12,
	SURFACE_ID_ROOM_NAME = 13,
	SURFACE_ID_STAGE_ITEM = 14,
	SURFACE_ID_LOADING = 15,
	SURFACE_ID_MY_CHAR = 16,
	SURFACE_ID_BULLET = 17,
	SURFACE_ID_CARET = 19,
	SURFACE_ID_NPC_SYM = 20,
	SURFACE_ID_LEVEL_SPRITESET_1 = 21,
	SURFACE_ID_LEVEL_SPRITESET_2 = 22,
	SURFACE_ID_NPC_REGU = 23,
	SURFACE_ID_TEXT_BOX = 26,
	SURFACE_ID_FACE = 27,
	SURFACE_ID_LEVEL_BACKGROUND = 28,
	SURFACE_ID_VALUE_VIEW = 29,
	SURFACE_ID_TEXT_LINE1 = 30,
	SURFACE_ID_TEXT_LINE2 = 31,
	SURFACE_ID_TEXT_LINE3 = 32,
	SURFACE_ID_TEXT_LINE4 = 33,
	SURFACE_ID_TEXT_LINE5 = 34,
	SURFACE_ID_CREDIT_CAST = 35,
	SURFACE_ID_CREDITS_IMAGE = 36,
	SURFACE_ID_CASTS = 37,
	SURFACE_ID_FONT = 40,
	SURFACE_ID_MAX = 41,
} Surface_Ids;

struct BUFFER_PIXEL
{
	uint8_t y, u, v;
};

struct SURFACE
{
	int w;
	int h;
	BUFFER_PIXEL *data;
};

extern SURFACE surf[SURFACE_ID_MAX];

BOOL Flip_SystemTask();
BOOL StartDirectDraw();
void EndDirectDraw();
void ReleaseSurface(int s);
BOOL MakeSurface_File(const char *name, Surface_Ids surf_no);
BOOL MakeSurface_Resource(const char *res, Surface_Ids surf_no);
BOOL ReloadBitmap_File(const char *name, Surface_Ids surf_no);
BOOL ReloadBitmap_Resource(const char *res, Surface_Ids surf_no);
BOOL MakeSurface_Generic(int bxsize, int bysize, Surface_Ids surf_no);
void BackupSurface(Surface_Ids surf_no, RECT *rect);
void PutBitmap3(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no);
void PutBitmap4(RECT *rcView, int x, int y, RECT *rect, Surface_Ids surf_no);
void Surface2Surface(int x, int y, RECT *rect, int to, int from);
unsigned long GetCortBoxColor(unsigned long col);
void CortBox(RECT *rect, uint32_t col);
void CortBox2(RECT *rect, uint32_t col, Surface_Ids surf_no);
void InitTextObject();
void PutText(int x, int y, const char *text, uint32_t color);
void PutText2(int x, int y, const char *text, uint32_t color, Surface_Ids surf_no);
void EndTextObject();
