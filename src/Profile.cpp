#include <stdint.h>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"
#include "Profile.h"
#include "Fade.h"
#include "ArmsItem.h"
#include "Flags.h"
#include "MiniMap.h"
#include "MyChar.h"
#include "Star.h"
#include "NpChar.h"
#include "Frame.h"
#include "SelStage.h"
#include "ValueView.h"
#include "Stage.h"
#include "Game.h"
#include "BossLife.h"
#include "SelStage.h"

const char *gDefaultName = "Profile.dat";
const char *gProfileCode = "Do041220";

bool IsProfile()
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gModulePath, gDefaultName);
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	SDL_RWclose(fp);
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
	
	SDL_RWops *fp = SDL_RWFromFile(path, "wb");
	if (!fp)
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
	SDL_RWwrite(fp, profile.code, 8, 1);
	SDL_WriteLE32(fp, profile.stage);
	SDL_WriteLE32(fp, profile.music);
	SDL_WriteLE32(fp, profile.x);
	SDL_WriteLE32(fp, profile.y);
	SDL_WriteLE32(fp, profile.direct);
	SDL_WriteLE16(fp, profile.max_life);
	SDL_WriteLE16(fp, profile.star);
	SDL_WriteLE16(fp, profile.life);
	SDL_WriteLE16(fp, profile.a);
	SDL_WriteLE32(fp, profile.select_arms);
	SDL_WriteLE32(fp, profile.select_item);
	SDL_WriteLE32(fp, profile.equip);
	SDL_WriteLE32(fp, profile.unit);
	SDL_WriteLE32(fp, profile.counter);
	for (int arm = 0; arm < 8; arm++)
	{
		SDL_WriteLE32(fp, profile.arms[arm].code);
		SDL_WriteLE32(fp, profile.arms[arm].level);
		SDL_WriteLE32(fp, profile.arms[arm].exp);
		SDL_WriteLE32(fp, profile.arms[arm].max_num);
		SDL_WriteLE32(fp, profile.arms[arm].num);
	}
	for (int item = 0; item < 32; item++)
		SDL_WriteLE32(fp, profile.items[item].code);
	for (int stage = 0; stage < 8; stage++)
	{
		SDL_WriteLE32(fp, profile.permitstage[stage].index);
		SDL_WriteLE32(fp, profile.permitstage[stage].event);
	}
	SDL_RWwrite(fp, profile.permit_mapping, 0x80, 1);
	SDL_RWwrite(fp, "FLAG", 4, 1);
	SDL_RWwrite(fp, profile.flags, 1000, 1);
	
	SDL_RWclose(fp);
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
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Check header code
	SDL_RWread(fp, profile.code, 8, 1);
	if (memcmp(profile.code, gProfileCode, 8))
		return false;
	
	//Read data
	SDL_RWseek(fp, 0, RW_SEEK_SET); //Pixel epic redundant code 😎😎😎
	SDL_RWread(fp, profile.code, 8, 1);
	profile.stage = SDL_ReadLE32(fp);
	profile.music = SDL_ReadLE32(fp);
	profile.x = SDL_ReadLE32(fp);
	profile.y = SDL_ReadLE32(fp);
	profile.direct = SDL_ReadLE32(fp);
	profile.max_life = SDL_ReadLE16(fp);
	profile.star = SDL_ReadLE16(fp);
	profile.life = SDL_ReadLE16(fp);
	profile.a = SDL_ReadLE16(fp);
	profile.select_arms = SDL_ReadLE32(fp);
	profile.select_item = SDL_ReadLE32(fp);
	profile.equip = SDL_ReadLE32(fp);
	profile.unit = SDL_ReadLE32(fp);
	profile.counter = SDL_ReadLE32(fp);
	for (int arm = 0; arm < 8; arm++)
	{
		profile.arms[arm].code = SDL_ReadLE32(fp);
		profile.arms[arm].level = SDL_ReadLE32(fp);
		profile.arms[arm].exp = SDL_ReadLE32(fp);
		profile.arms[arm].max_num = SDL_ReadLE32(fp);
		profile.arms[arm].num = SDL_ReadLE32(fp);
	}
	for (int item = 0; item < 32; item++)
		profile.items[item].code = SDL_ReadLE32(fp);
	for (int stage = 0; stage < 8; stage++)
	{
		profile.permitstage[stage].index = SDL_ReadLE32(fp);
		profile.permitstage[stage].event = SDL_ReadLE32(fp);
	}
	SDL_RWread(fp, profile.permit_mapping, 0x80, 1);
	SDL_RWread(fp, profile.FLAG, 4, 1);
	SDL_RWread(fp, profile.flags, 1000, 1);
	SDL_RWclose(fp);
	
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
