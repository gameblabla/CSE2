#pragma once

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "SelStage.h"

struct PROFILE
{
	char code[8];
	int stage;
	int music;
	int x;
	int y;
	int direct;
	short max_life;
	short star;
	short life;
	short a;
	int select_arms;
	int select_item;
	int equip;
	int unit;
	int counter;
	ARMS arms[8];
	ITEM items[32];
	PERMIT_STAGE permitstage[8];
	signed char permit_mapping[0x80];
	char FLAG[4];
	unsigned char flags[1000];
};

BOOL IsProfile();
BOOL SaveProfile(const char *name);
BOOL LoadProfile(const char *name);
BOOL InitializeGame(HWND hWnd);
