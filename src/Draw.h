#pragma once
#include <stdint.h>
#include "WindowsWrapper.h"
#include <SDL_render.h>

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

extern RECT grcGame;
extern RECT grcFull;

extern int magnification;
extern bool fullscreen;

enum Surface_Ids
{
	SURFACE_ID_TITLE = 0x0,
	SURFACE_ID_PIXEL = 0x1,
	SURFACE_ID_LEVEL_TILESET = 0x2,
	SURFACE_ID_FADE = 0x6,
	SURFACE_ID_ITEM_IMAGE = 0x8,
	SURFACE_ID_MAP = 0x9,
	SURFACE_ID_SCREEN_GRAB = 0xA,
	SURFACE_ID_ARMS = 0xB,
	SURFACE_ID_ARMS_IMAGE = 0xC,
	SURFACE_ID_ROOM_NAME = 0xD,
	SURFACE_ID_STAGE_ITEM = 0xE,
	SURFACE_ID_LOADING = 0xF,
	SURFACE_ID_MY_CHAR = 0x10,
	SURFACE_ID_BULLET = 0x11,
	SURFACE_ID_CARET = 0x13,
	SURFACE_ID_NPC_SYM = 0x14,
	SURFACE_ID_LEVEL_SPRITESET_1 = 0x15,
	SURFACE_ID_LEVEL_SPRITESET_2 = 0x16,
	SURFACE_ID_NPC_REGU = 0x17,
	SURFACE_ID_TEXT_BOX = 0x1A,
	SURFACE_ID_FACE = 0x1B,
	SURFACE_ID_LEVEL_BACKGROUND = 0x1C,
	SURFACE_ID_CREDIT_CAST = 0x23,
	SURFACE_ID_CREDITS_IMAGE = 0x24,
	SURFACE_ID_CASTS = 0x25,
	SURFACE_ID_MAX = 0x28,
};

struct SURFACE
{
	bool in_use;
	bool needs_updating;
	SDL_Surface *surface;
	SDL_Texture *texture;
};

extern SURFACE surf[SURFACE_ID_MAX];

bool Flip_SystemTask();
bool StartDirectDraw(int lMagnification, int lColourDepth);
void EndDirectDraw();
void ReleaseSurface(int s);
bool MakeSurface_File(const char *name, int surf_no);
bool MakeSurface_Resource(const char *res, int surf_no);
bool ReloadBitmap_File(const char *name, int surf_no);
bool ReloadBitmap_Resource(const char *res, int surf_no);
bool MakeSurface_Generic(int bxsize, int bysize, int surf_no);
void BackupSurface(int surf_no, RECT *rect);
void PutBitmap3(RECT *rcView, int x, int y, RECT *rect, int surf_no);
void PutBitmap4(RECT *rcView, int x, int y, RECT *rect, int surf_no);
void Surface2Surface(int x, int y, RECT *rect, int to, int from);
void CortBox(RECT *rect, uint32_t col);
void CortBox2(RECT *rect, uint32_t col, int surf_no);
void InitTextObject(const char *font_name);
void PutText(int x, int y, const char *text, uint32_t color);
void PutText2(int x, int y, const char *text, uint32_t color, int surf_no);
void EndTextObject();
