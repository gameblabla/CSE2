#pragma once

#include <stdint.h>

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
	int16_t max_life;
	int16_t star;
	int16_t life;
	int16_t a;
	int select_arms;
	int select_item;
	int equip;
	int unit;
	int counter;
	ARMS arms[8];
	ITEM items[32];
	PERMIT_STAGE permitstage[8];
	char permit_mapping[0x80];
	char FLAG[4];
	uint8_t flags[1000];
};

bool IsProfile();
bool SaveProfile(char *name);
bool LoadProfile(char *name);
bool InitializeGame();
