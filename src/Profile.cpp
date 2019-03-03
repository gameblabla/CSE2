#include "Profile.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "BossLife.h"
#include "CommonDefines.h"
#include "Fade.h"
#include "File.h"
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

bool IsProfile()
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gModulePath, gDefaultName);
	
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return false;
	
	fclose(fp);
	return true;
}

bool SaveProfile(char *name)
{
	//Get path
	char path[PATH_LENGTH];
	
	if (name)
		sprintf(path, "%s/%s", gModulePath, name);
	else
		sprintf(path, "%s/%s", gModulePath, gDefaultName);
	
	//Open file
	PROFILE profile;
	
	FILE *fp = fopen(path, "wb");
	if (fp == NULL)
		return false;
	
	//Set up profile
	memset(&profile, 0, sizeof(PROFILE));
	memcpy(profile.code, gProfileCode, sizeof(profile.code));
	memcpy(profile.FLAG, "FLAG", sizeof(profile.FLAG));
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
	
	//Write to file
	fwrite(profile.code, 8, 1, fp);
	File_WriteLE32(profile.stage, fp);
	File_WriteLE32(profile.music, fp);
	File_WriteLE32(profile.x, fp);
	File_WriteLE32(profile.y, fp);
	File_WriteLE32(profile.direct, fp);
	File_WriteLE16(profile.max_life, fp);
	File_WriteLE16(profile.star, fp);
	File_WriteLE16(profile.life, fp);
	File_WriteLE16(profile.a, fp);
	File_WriteLE32(profile.select_arms, fp);
	File_WriteLE32(profile.select_item, fp);
	File_WriteLE32(profile.equip, fp);
	File_WriteLE32(profile.unit, fp);
	File_WriteLE32(profile.counter, fp);
	for (int arm = 0; arm < 8; arm++)
	{
		File_WriteLE32(profile.arms[arm].code, fp);
		File_WriteLE32(profile.arms[arm].level, fp);
		File_WriteLE32(profile.arms[arm].exp, fp);
		File_WriteLE32(profile.arms[arm].max_num, fp);
		File_WriteLE32(profile.arms[arm].num, fp);
	}
	for (int item = 0; item < 32; item++)
		File_WriteLE32(profile.items[item].code, fp);
	for (int stage = 0; stage < 8; stage++)
	{
		File_WriteLE32(profile.permitstage[stage].index, fp);
		File_WriteLE32(profile.permitstage[stage].event, fp);
	}
	fwrite(profile.permit_mapping, 0x80, 1, fp);
	fwrite("FLAG", 4, 1, fp);
	fwrite(profile.flags, 1000, 1, fp);
	
	fclose(fp);
	return true;
}

bool LoadProfile(char *name)
{
	//Get path
	char path[PATH_LENGTH];
	
	if (name)
		strcpy(path, name);
	else
		sprintf(path, "%s/%s", gModulePath, gDefaultName);
	
	//Open file
	PROFILE profile;
	
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return false;
	
	//Check header code
	fread(profile.code, 8, 1, fp);
	if (memcmp(profile.code, gProfileCode, 8))
	{
#ifdef FIX_BUGS
		fclose(fp);	// The original game forgets to close the file
#endif
		return false;
	}
	
	//Read data
	fseek(fp, 0, SEEK_SET); //Pixel epic redundant code 😎😎😎
	fread(profile.code, 8, 1, fp);
	profile.stage = File_ReadLE32(fp);
	profile.music = File_ReadLE32(fp);
	profile.x = File_ReadLE32(fp);
	profile.y = File_ReadLE32(fp);
	profile.direct = File_ReadLE32(fp);
	profile.max_life = File_ReadLE16(fp);
	profile.star = File_ReadLE16(fp);
	profile.life = File_ReadLE16(fp);
	profile.a = File_ReadLE16(fp);
	profile.select_arms = File_ReadLE32(fp);
	profile.select_item = File_ReadLE32(fp);
	profile.equip = File_ReadLE32(fp);
	profile.unit = File_ReadLE32(fp);
	profile.counter = File_ReadLE32(fp);
	for (int arm = 0; arm < 8; arm++)
	{
		profile.arms[arm].code = File_ReadLE32(fp);
		profile.arms[arm].level = File_ReadLE32(fp);
		profile.arms[arm].exp = File_ReadLE32(fp);
		profile.arms[arm].max_num = File_ReadLE32(fp);
		profile.arms[arm].num = File_ReadLE32(fp);
	}
	for (int item = 0; item < 32; item++)
		profile.items[item].code = File_ReadLE32(fp);
	for (int stage = 0; stage < 8; stage++)
	{
		profile.permitstage[stage].index = File_ReadLE32(fp);
		profile.permitstage[stage].event = File_ReadLE32(fp);
	}
	fread(profile.permit_mapping, 0x80, 1, fp);
	fread(profile.FLAG, 4, 1, fp);
	fread(profile.flags, 1000, 1, fp);
	fclose(fp);
	
	//Set things
	gSelectedArms = profile.select_arms;
	gSelectedItem = profile.select_item;
	gCounter = profile.counter;

	memcpy(gArmsData, profile.arms, sizeof(gArmsData));
	memcpy(gItemData, profile.items, sizeof(gItemData));
	memcpy(gPermitStage, profile.permitstage, sizeof(gPermitStage));
	memcpy(gMapping, profile.permit_mapping, sizeof(gMapping));
	memcpy(gFlagNPC, profile.flags, sizeof(gFlagNPC));
	
	//Load stage
	ChangeMusic(profile.music);
	InitMyChar();
	if (!TransferStage(profile.stage, 0, 0, 1))
		return false;
	
	//Set character properties
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
	
	//Reset stuff
	ClearFade();
	SetFrameMyChar();
	SetFrameTargetMyChar(16);
	InitBossLife();
	CutNoise();
	InitStar();
	ClearValueView();
	gCurlyShoot_wait = 0;
	return true;
}

bool InitializeGame()
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
		return false;
	ClearFade();
	SetFrameMyChar();
	SetFrameTargetMyChar(16);
	InitBossLife();
	CutNoise();
	ClearValueView();
	gCurlyShoot_wait = 0;
	SetFadeMask();
	SetFrameTargetMyChar(16);
	return true;
}
