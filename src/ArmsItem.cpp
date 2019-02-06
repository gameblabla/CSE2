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
#include "Shoot.h"

int gArmsEnergyX = 16;

ARMS gArmsData[ARMS_MAX];
ITEM gItemData[ITEM_MAX];
int gSelectedArms;
int gSelectedItem;
int gCampTitleY;
bool gCampActive;

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

void MoveCampCursor()
{
	int arms_num = 0;
	int item_num = 0;
	while ( gArmsData[arms_num].code != 0 )
		++arms_num;
	while ( gItemData[item_num].code != 0 )
		++item_num;
	
	if (arms_num || item_num)
	{
		bool bChange = false;
		if (gCampActive)
		{
			if (gKeyTrg & gKeyLeft)
			{
				if (gSelectedItem % 6)
					--gSelectedItem;
				else
					gSelectedItem += 5;
				
				bChange = true;
			}
			
			if (gKeyTrg & gKeyRight)
			{
				if (item_num - 1 == gSelectedItem)
					gSelectedItem = 6 * (gSelectedItem / 6);
				else if (gSelectedItem % 6 == 5)
					gSelectedItem -= 5;
				else
					++gSelectedItem;
				
				bChange = true;
			}
			
			if (gKeyTrg & gKeyUp)
			{
				if (gSelectedItem + 5 > 10)
					gSelectedItem -= 6;
				else
					gCampActive = false;
				
				bChange = true;
			}
			
			if ( gKeyTrg & gKeyDown )
			{
				if ( gSelectedItem / 6 == (item_num - 1) / 6 )
					gCampActive = false;
				else
					gSelectedItem += 6;
				
				bChange = true;
			}
			
			if (gSelectedItem >= item_num)
				gSelectedItem = item_num - 1;
			
			if (gCampActive && gKeyTrg & gKeyOk)
				StartTextScript(gItemData[gSelectedItem].code + 6000);
		}
		else
		{
			if (gKeyTrg & gKeyLeft)
			{
				--gSelectedArms;
				bChange = true;
			}
			if (gKeyTrg & gKeyRight)
			{
				++gSelectedArms;
				bChange = true;
			}
			if ((gKeyDown | gKeyUp) & gKeyTrg)
			{
				if (item_num)
					gCampActive = true;
				bChange = true;
			}
			
			if (gSelectedArms < 0)
				gSelectedArms = arms_num - 1;
			if (arms_num - 1 < gSelectedArms)
				gSelectedArms = 0;
		}
		
		if (bChange)
		{
			if (gCampActive)
			{
				PlaySoundObject(1, 1);
				
				if (item_num)
					StartTextScript(gItemData[gSelectedItem].code + 5000);
				else
					StartTextScript(5000);
			}
			else
			{
				PlaySoundObject(4, 1);
				
				if (arms_num)
					StartTextScript(gArmsData[gSelectedArms].code + 1000);
				else
					StartTextScript(1000);
			}
		}
	}
}

void PutCampObject()
{
	//Get rects
	RECT rcPer = {72, 48, 80, 56};
	RECT rcNone = {80, 48, 96, 56};
	RECT rcLv = {80, 80, 96, 88};
	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	RECT rcCur1[2];
	RECT rcCur2[2];
	rcCur1[0] = {0, 88, 40, 128};
	rcCur1[1] = {40, 88, 80, 128};
	rcCur2[0] = {80, 88, 112, 104};
	rcCur2[1] = {80, 104, 112, 120};
	RECT rcTitle1 = {80, 48, 144, 56};
	RECT rcTitle2 = {80, 56, 144, 64};
	RECT rcBoxTop = {0, 0, 244, 8};
	RECT rcBoxBody = {0, 8, 244, 16};
	RECT rcBoxBottom = {0, 16, 244, 24};
	
	//Draw box
	int y;
	PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, (WINDOW_HEIGHT - 224) / 2, &rcBoxTop, 26);
	for (y = 1; y < 18; y++)
		PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, ((WINDOW_HEIGHT - 240) / 2) + (8 * (y + 1)), &rcBoxBody, 26);
	PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, ((WINDOW_HEIGHT - 240) / 2) + (8 * (y + 1)), &rcBoxBottom, 26);
	
	//Move titles
	if (gCampTitleY > (WINDOW_HEIGHT - 208) / 2)
		--gCampTitleY;
	
	//Draw titles
	PutBitmap3(&rcView, (WINDOW_WIDTH - 224) / 2, gCampTitleY, &rcTitle1, 26);
	PutBitmap3(&rcView, (WINDOW_WIDTH - 224) / 2, gCampTitleY + 52, &rcTitle2, 26);
	
	//Draw arms cursor
	static int flash;
	++flash;
	
	if (gCampActive)
		PutBitmap3(&rcView, 40 * gSelectedArms + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT / 2) - 96, &rcCur1[1], 26);
	else
		PutBitmap3(&rcView, 40 * gSelectedArms + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT / 2) - 96, &rcCur1[(flash >> 1) & 1], 26);
	
	//Draw arms
	for (int i = 0; i < ARMS_MAX && gArmsData[i].code; i++)
	{
		RECT rcArms;
		rcArms.left = 16 * (gArmsData[i].code % 16);
		rcArms.right = rcArms.left + 16;
		rcArms.top = 16 * (gArmsData[i].code / 16);
		rcArms.bottom = rcArms.top + 16;
		
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 192) / 2, &rcArms, 12);
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 128) / 2, &rcPer, 26);
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 160) / 2, &rcLv, 26);
		PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 160) / 2, gArmsData[i].level, 0);
		
		//Draw ammo
		if ( gArmsData[i].max_num )
		{
			PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 144) / 2, gArmsData[i].num, 0);
			PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 128) / 2, gArmsData[i].max_num, 0);
		}
		else
		{
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 144) / 2, &rcNone, 26);
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 128) / 2, &rcNone, 26);
		}
	}
	
	//Draw items cursor
	if (gCampActive)
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[(flash >> 1) & 1], 26);
	else
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[1], 26);

	for (int i = 0; i < ITEM_MAX && gItemData[i].code; i++)
	{
		RECT rcItem;
		rcItem.left = 32 * (gItemData[i].code % 8);
		rcItem.right = rcItem.left + 32;
		rcItem.top = 16 * (gItemData[i].code / 8);
		rcItem.bottom = rcItem.top + 16;
		
		PutBitmap3(&rcView, 32 * (i % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (i / 6) + (WINDOW_HEIGHT - 88) / 2, &rcItem, 8);
	}
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
	
	gCampTitleY = (WINDOW_HEIGHT - 192) / 2;
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
		
		if (g_GameFlags & 2)
			MoveCampCursor();
		
		int tscRet = TextScriptProc();
		if (tscRet == 0)
			return 0;
		if (tscRet == 2)
			return 2;
		
		PutBitmap4(&rcView, 0, 0, &rcView, 10);
		PutCampObject();
		PutTextScript();
		PutFramePerSecound();
		
		if (!gCampActive && (gKeyItem | gKeyCancel | gKeyOk) & gKeyTrg)
			break;
		else if (g_GameFlags & 2 && (gKeyItem | gKeyCancel) & gKeyTrg)
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
	
	ResetSpurCharge();
	
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
	
	ResetSpurCharge();
	
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