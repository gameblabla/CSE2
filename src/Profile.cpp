#include "Profile.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "BossLife.h"
#include "Fade.h"
#include "Flags.h"
#include "Frame.h"
#include "Game.h"
#include "MiniMap.h"
#include "MyChar.h"
#include "NpChar.h"
#include "SelStage.h"
#include "Stage.h"
#include "Star.h"
#include "Tags.h"
#include "ValueView.h"

const char *gDefaultName = "Profile.dat";
const char *gProfileCode = "Do041220";

BOOL IsProfile()
{
	char path[MAX_PATH];
	sprintf(path, "%s\\%s", gModulePath, gDefaultName);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	fclose(fp);
	return TRUE;
}

BOOL SaveProfile(const char *name)
{
	PROFILE profile;
	FILE *fp;
	const char *FLAG = "FLAG";
	char path[MAX_PATH];

	// Get path
	if (name)
		sprintf(path, "%s\\%s", gModulePath, name);
	else
		sprintf(path, "%s\\%s", gModulePath, gDefaultName);

	// Open file
	fp = fopen(path, "wb");
	if (fp == NULL)
		return FALSE;

	// Set up profile
	memset(&profile, 0, sizeof(PROFILE));
	memcpy(profile.code, gProfileCode, sizeof(profile.code));
	memcpy(profile.FLAG, FLAG, sizeof(profile.FLAG));
	profile.stage = gStageNo;
	profile.music = gMusicNo;
	profile.x = gMC.x;
	profile.y = gMC.y;
	profile.direct = gMC.direct;
	profile.max_life = gMC.max_life;
	profile.life = gMC.life;
	profile.star = gMC.star;
	profile.select_arms = gSelectedArms;
	profile.select_item = gSelectedItem;
	profile.equip = gMC.equip;
	profile.unit = gMC.unit;
	profile.counter = gCounter;
	memcpy(profile.arms, gArmsData, sizeof(profile.arms));
	memcpy(profile.items, gItemData, sizeof(profile.items));
	memcpy(profile.permitstage, gPermitStage, sizeof(profile.permitstage));
	memcpy(profile.permit_mapping, gMapping, sizeof(profile.permit_mapping));
	memcpy(profile.flags, gFlagNPC, sizeof(profile.flags));

	// Write to file
	fwrite(&profile, sizeof(PROFILE), 1, fp);

	fclose(fp);
	return TRUE;
}

BOOL LoadProfile(const char *name)
{
	// Get path
	char path[MAX_PATH];

	if (name)
		sprintf(path, "%s", name);
	else
		sprintf(path, "%s\\%s", gModulePath, gDefaultName);

	// Open file
	PROFILE profile;

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	// Check header code
	fread(profile.code, 8, 1, fp);
	if (memcmp(profile.code, gProfileCode, 8))
	{
#ifdef FIX_BUGS
		fclose(fp);	// The original game forgets to close the file
#endif
		return FALSE;
	}

	// Read data
	fseek(fp, 0, SEEK_SET);
	memset(&profile, 0, sizeof(PROFILE));
	fread(&profile, sizeof(PROFILE), 1, fp);
	fclose(fp);

	// Set things
	gSelectedArms = profile.select_arms;
	gSelectedItem = profile.select_item;
	gCounter = profile.counter;

	memcpy(gArmsData, profile.arms, sizeof(gArmsData));
	memcpy(gItemData, profile.items, sizeof(gItemData));
	memcpy(gPermitStage, profile.permitstage, sizeof(gPermitStage));
	memcpy(gMapping, profile.permit_mapping, sizeof(gMapping));
	memcpy(gFlagNPC, profile.flags, sizeof(gFlagNPC));

	// Load stage
	ChangeMusic(profile.music);
	InitMyChar();
	if (!TransferStage(profile.stage, 0, 0, 1))
		return FALSE;

	// Set character properties
	gMC.equip = profile.equip;
	gMC.unit = profile.unit;
	gMC.direct = profile.direct;
	gMC.max_life = profile.max_life;
	gMC.life = profile.life;
	gMC.star = profile.star;
	gMC.cond = 0x80;
	gMC.air = 1000;
	gMC.lifeBr = profile.life;
	gMC.x = profile.x;
	gMC.y = profile.y;

	gMC.rect_arms.left = 24 * (gArmsData[gSelectedArms].code % 10);
	gMC.rect_arms.right = gMC.rect_arms.left + 24;
	gMC.rect_arms.top = 32 * (gArmsData[gSelectedArms].code / 10);
	gMC.rect_arms.bottom = gMC.rect_arms.top + 16;

	// Reset stuff
	ClearFade();
	SetFrameMyChar();
	SetFrameTargetMyChar(16);
	InitBossLife();
	CutNoise();
	InitStar();
	ClearValueView();
	gCurlyShoot_wait = 0;
	return TRUE;
}

BOOL InitializeGame(HWND hWnd)
{
	InitMyChar();
	gSelectedArms = 0;
	gSelectedItem = 0;
	gCounter = 0;
	ClearArmsData();
	ClearItemData();
	ClearPermitStage();
	StartMapping();
	InitFlags();
	if (!TransferStage(13, 200, 10, 8))
	{
		MessageBoxA(hWnd, "\x83\x58\x83\x65\x81\x5B\x83\x57\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9\x8E\xB8\x94\x73", "\x83\x47\x83\x89\x81\x5B", MB_OK);	// 'ステージの読み込みに失敗' and 'エラー' ('Failed to load stage' and 'Error') in Shift-JIS
		return FALSE;
	}

	ClearFade();
	SetFrameMyChar();
	SetFrameTargetMyChar(16);
	InitBossLife();
	CutNoise();
	ClearValueView();
	gCurlyShoot_wait = 0;
	SetFadeMask();
	SetFrameTargetMyChar(16);
	return TRUE;
}
