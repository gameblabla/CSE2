#pragma once

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "MyChar.h"
#include "SelStage.h"
#include "Stage.h"

typedef struct PROFILE
{
	char code[8];
	int stage;
	MusicID music;
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

	// Custom
	char extra_code[0x10];
	// <MIM
	unsigned int MIMCurrentNum;
	// <PHY
	MYCHAR_PHYSICS physics_normal;
	MYCHAR_PHYSICS physics_underwater;
	int no_splash_or_air_limit_underwater;
} PROFILE;

extern const char* const gDefaultName;
extern const char* const gProfileCode;

BOOL IsProfile(void);
BOOL SaveProfile(const char *name);
BOOL LoadProfile(const char *name);
BOOL InitializeGame(void);
