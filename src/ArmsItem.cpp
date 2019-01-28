#include <string.h>

#include "CommonDefines.h"
#include "TextScr.h"
#include "ArmsItem.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Escape.h"
#include "Sound.h"
#include "Main.h"
#include "Game.h"

int gArmsEnergyX = 16;

ARMS gArmsData[ARMS_MAX];
ITEM gItemData[ITEM_MAX];
int gSelectedArms;
int gSelectedItem;
int gCampTitleY;
int gCampActive;

void ClearArmsData()
{
	gArmsEnergyX = 32;
	memset(gArmsData, 0, sizeof(gArmsData));
}

void ClearItemData()
{
	memset(gItemData, 0, sizeof(gItemData));
}

bool AddArmsData(int code, int max_num)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (gArmsData[i].code == code || !gArmsData[i].code)
		{
			if (!gArmsData[i].code)
			{
				memset(&gArmsData[i], 0, sizeof(ARMS));
				gArmsData[i].level = 1;
			}
			
			gArmsData[i].code = code;
			gArmsData[i].max_num += max_num;
			gArmsData[i].num += max_num;
			if (gArmsData[i].num > gArmsData[i].max_num)
				gArmsData[i].num = gArmsData[i].max_num;
			return true;
		}
	}
	
	return false;
}

bool SubArmsData(int code)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (gArmsData[i].code == code)
		{
			//Shift all arms from the right to the left
			int ia;
			for (ia = i + 1; ia < ARMS_MAX; ++ia)
			{
				gArmsData[ia - 1].code = gArmsData[ia].code;
				gArmsData[ia - 1].level = gArmsData[ia].level;
				gArmsData[ia - 1].exp = gArmsData[ia].exp;
				gArmsData[ia - 1].max_num = gArmsData[ia].max_num;
				gArmsData[ia - 1].num = gArmsData[ia].num;
			}
			
			gArmsData[ia - 1].code = 0;
			gSelectedArms = 0;
			return true;
		}
	}
	
	return false;
}

bool TradeArms(int code1, int code2, int max_num)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (gArmsData[i].code == code1)
		{
			gArmsData[i].level = 1;
			gArmsData[i].code = code2;
			gArmsData[i].max_num += max_num;
			gArmsData[i].num += max_num;
			gArmsData[i].exp = 0;
			return true;
		}
	}
	
	return false;
}

bool AddItemData(int code)
{
	for (int i = 0; i < ITEM_MAX; i++)
	{
		if (gItemData[i].code == code || !gItemData[i].code)
		{
			gItemData[i].code = code;
			return true;
		}
	}
	
	return false;
}

bool SubItemData(int code)
{
	for (int i = 0; i < ITEM_MAX; i++)
	{
		if (gItemData[i].code == code)
		{
			//Shift all items from the right to the left
			int ia;
			for (ia = i + 1; ia < ITEM_MAX; ++ia)
				gItemData[ia - 1].code = gItemData[ia].code;
			
			gItemData[ia - 1].code = 0;
			gSelectedItem = 0;
			return true;
		}
	}
	
	return false;
}

int CampLoop()
{
	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	
	//Load the inventory script
	char old_script_path[PATH_LENGTH];
	GetTextScriptPath(old_script_path);
	
	char name[13];
	strcpy(name, "ArmsItem.tsc");
	LoadTextScript2(name);
	
	gCampTitleY = 24;
	gCampActive = 0;
	gSelectedItem = 0;
	
	//Run script
	int arms_num;
	for (arms_num = 0; gArmsData[arms_num].code != 0; arms_num++);
	
	if (arms_num)
		StartTextScript(gArmsData[gSelectedArms].code + 1000);
	else
		StartTextScript(gItemData[gSelectedItem].code + 5000);
	
	while (true)
	{
		GetTrg();
		
		if (gKeyTrg & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 1;
		}
		
		//if (g_GameFlags & 2)
		//	MoveCampCursor();
		
		int tscRet = TextScriptProc();
		if (tscRet == 0)
			return 0;
		if (tscRet == 2)
			return 2;
		
		PutBitmap4(&rcView, 0, 0, &rcView, 10);
		//PutCampObject();
		PutTextScript();
		PutFramePerSecound();
		
		if (!gCampActive && (gKeyItem | gKeyCancel | gKeyOk) & gKeyTrg)
			break;
		
		if (g_GameFlags & 2 && (gKeyItem | gKeyCancel) & gKeyTrg)
			break;
		
		if (!Flip_SystemTask())
			return 0;
	}
	
	//Resume original script
	StopTextScript();
	LoadTextScript_Stage(old_script_path);
	gArmsEnergyX = 32;
	return 1;
}

bool CheckItem(int a)
{
	for (int i = 0; i < ITEM_MAX; i++)
	{
		if (gItemData[i].code == a)
			return true;
	}
	
	return false;
}

bool CheckArms(int a)
{
  for (int i = 0; i < ARMS_MAX; i++)
	{
		if (gArmsData[i].code == a)
			return true;
	}
	
	return false;
}

bool UseArmsEnergy(int num)
{
	if (!gArmsData[gSelectedArms].max_num)
		return true;
	if (!gArmsData[gSelectedArms].num)
		return false;
	gArmsData[gSelectedArms].num -= num;
	if (gArmsData[gSelectedArms].num < 0)
		gArmsData[gSelectedArms].num = 0;
	return true;
}

bool ChargeArmsEnergy(int num)
{
	gArmsData[gSelectedArms].num += num;
	if (gArmsData[gSelectedArms].num > gArmsData[gSelectedArms].max_num)
		gArmsData[gSelectedArms].num = gArmsData[gSelectedArms].max_num;
	return true;
}

void FullArmsEnergy()
{
	for (int a = 0; a < ARMS_MAX; a++)
	{
		if (gArmsData[a].code)
			gArmsData[a].num = gArmsData[a].max_num;
	}
}

int RotationArms()
{
	int arms_num;
	for ( arms_num = 0; gArmsData[arms_num].code != 0; arms_num++);
	if (!arms_num)
		return 0;
	
	//ResetSpurCharge();
	
	++gSelectedArms;
	while (gSelectedArms < arms_num && !gArmsData[gSelectedArms].code)
		++gSelectedArms;
	
	if (gSelectedArms == arms_num)
		gSelectedArms = 0;
	
	gArmsEnergyX = 32;
	PlaySoundObject(4, 1);
	
	return gArmsData[gSelectedArms].code;
}

int RotationArmsRev()
{
	int arms_num;
	for (arms_num = 0; gArmsData[arms_num].code != 0; arms_num++);
	if (!arms_num)
		return 0;
	
	//ResetSpurCharge();
	
	if (--gSelectedArms < 0)
		gSelectedArms = arms_num - 1;
	while (gSelectedArms < arms_num && !gArmsData[gSelectedArms].code)
		--gSelectedArms;
	
	gArmsEnergyX = 0;
	PlaySoundObject(4, 1);
	
	return gArmsData[gSelectedArms].code;
}

void ChangeToFirstArms()
{
	gSelectedArms = 0;
	gArmsEnergyX = 32;
	PlaySoundObject(4, 1);
}