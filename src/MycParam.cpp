#include <math.h>

#include "MycParam.h"

#include "ArmsItem.h"
#include "CommonDefines.h"
#include "Caret.h"
#include "Draw.h"
#include "Game.h"
#include "File.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Tags.h"
#include "TextScr.h"
#include "ValueView.h"

ARMS_LEVEL gArmsLevelTable[14] =
{
	{{0,  0,  100}},
	{{30, 40, 16}},
	{{10, 20, 10}},
	{{10, 20, 20}},
	{{30, 40, 10}},
	{{10, 20, 10}},
	{{10, 20, 30}},
	{{10, 20, 5}},
	{{10, 20, 100}},
	{{30, 60, 0}},
	{{30, 60, 10}},
	{{10, 20, 100}},
	{{1,  1,  1}},
	{{40, 60, 200}}
};

double time_count;

void AddExpMyChar(int x)
{
	int lv = gArmsData[gSelectedArms].level - 1;
	int arms_code = gArmsData[gSelectedArms].code;
	
	gArmsData[gSelectedArms].exp += x;
	
	if (lv == 2)
	{
		if (gArmsData[gSelectedArms].exp >= gArmsLevelTable[0].exp[3 * arms_code + 2])
		{
			gArmsData[gSelectedArms].exp = gArmsLevelTable[0].exp[3 * arms_code + 2];
			
			if (gMC.equip & 0x80)
			{
				if (gMC.star < 3)
					++gMC.star;
			}
		}
	}
	else
	{
		while (lv <= 1)
		{
			if (gArmsData[gSelectedArms].exp >= gArmsLevelTable[0].exp[lv + 3 * arms_code])
			{
				++gArmsData[gSelectedArms].level;
				gArmsData[gSelectedArms].exp = 0;
				
				if (gArmsData[gSelectedArms].code != 13)
				{
					PlaySoundObject(27, 1);
					SetCaret(gMC.x, gMC.y, 10, 0);
				}
			}
			
			++lv;
		}
	}
	
	if (gArmsData[gSelectedArms].code == 13)
	{
		gMC.exp_wait = 10;
	}
	else
	{
		gMC.exp_count += x;
		gMC.exp_wait = 30;
	}
}

void ZeroExpMyChar()
{
	gArmsData[gSelectedArms].level = 1;
	gArmsData[gSelectedArms].exp = 0;
}

bool IsMaxExpMyChar()
{
	return gArmsData[gSelectedArms].level == 3
		&& gArmsData[gSelectedArms].exp >= gArmsLevelTable[gArmsData[gSelectedArms].code].exp[2];
}

void DamageMyChar(int damage)
{
	if (!gMC.shock)
	{
		//Damage player
		PlaySoundObject(16, 1);
		gMC.cond &= ~1;
		gMC.shock = 128;
		if (gMC.unit != 1)
			gMC.ym = -0x400;
		gMC.life -= damage;
		
		//Lose a whimsical star
		if (gMC.equip & 0x80 && gMC.star > 0)
			--gMC.star;
		
		//Lose experience
		if (gMC.equip & 4)
			gArmsData[gSelectedArms].exp -= damage;
		else
			gArmsData[gSelectedArms].exp -= 2 * damage;
		
		while (gArmsData[gSelectedArms].exp < 0)
		{
			if (gArmsData[gSelectedArms].level <= 1)
			{
				gArmsData[gSelectedArms].exp = 0;
			}
			else
			{
				gArmsData[gSelectedArms].exp += gArmsLevelTable[0].exp[--gArmsData[gSelectedArms].level - 1 + 3 * gArmsData[gSelectedArms].code];
				if (gMC.life > 0 && gArmsData[gSelectedArms].code != 13)
					SetCaret(gMC.x, gMC.y, 10, 2);
			}
		}
		
		//Tell player how much damage was taken
		SetValueView(&gMC.x, &gMC.y, -damage);
		
		//Death
		if (gMC.life <= 0)
		{
			PlaySoundObject(17, 1);
			gMC.cond = 0;
			SetDestroyNpChar(gMC.x, gMC.y, 0x1400, 0x40);
			StartTextScript(40);
		}
	}
}

void ZeroArmsEnergy_All()
{
	for (int a = 0; a < ARMS_MAX; a++)
	{
		gArmsData[a].level = 1;
		gArmsData[a].exp = 0;
	}
}

void AddBulletMyChar(int no, int val)
{
	//Missile Launcher
	for (int a = 0; a < ARMS_MAX; a++)
	{
		if (gArmsData[a].code == 5)
		{
			gArmsData[a].num += val;
			if (gArmsData[a].num > gArmsData[a].max_num)
				gArmsData[a].num = gArmsData[a].max_num;
			break;
		}
	}
	
	//Super Missile Launcher
	for (int a = 0; a < ARMS_MAX; a++)
	{
		if (gArmsData[a].code == 10)
		{
			gArmsData[a].num += val;
			if (gArmsData[a].num > gArmsData[a].max_num)
				gArmsData[a].num = gArmsData[a].max_num;
			break;
		}
	}
}

void AddLifeMyChar(int x)
{
	gMC.life += x;
	if (gMC.life > gMC.max_life)
		gMC.life = gMC.max_life;
	gMC.lifeBr = gMC.life;
}

void AddMaxLifeMyChar(int val)
{
	gMC.max_life += val;
	if (gMC.max_life > 232)
		gMC.max_life = 232;
	gMC.life += val;
	gMC.lifeBr = gMC.life;
}

void PutArmsEnergy(bool flash)
{
	RECT rcPer = {72, 48, 80, 56};
	RECT rcLv = {80, 80, 96, 88};
	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	RECT rcNone = {80, 48, 96, 56};
	
	if (gArmsEnergyX > 16)
		gArmsEnergyX -= 2;
	if (gArmsEnergyX < 16)
		gArmsEnergyX += 2;
	
	//Draw max ammo
	if (gArmsData[gSelectedArms].max_num)
	{
		PutNumber4(gArmsEnergyX + 32, 16, gArmsData[gSelectedArms].num, 0);
		PutNumber4(gArmsEnergyX + 32, 24, gArmsData[gSelectedArms].max_num, 0);
	}
	else
	{
		PutBitmap3(&rcView, gArmsEnergyX + 48, 16, &rcNone, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&rcView, gArmsEnergyX + 48, 24, &rcNone, SURFACE_ID_TEXT_BOX);
	}
	
	//Draw experience and ammo
	if (!flash || !((gMC.shock >> 1) & 1))
	{
		PutBitmap3(&rcView, gArmsEnergyX + 32, 24, &rcPer, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&rcView, gArmsEnergyX, 32, &rcLv, SURFACE_ID_TEXT_BOX);
		PutNumber4(gArmsEnergyX - 8, 32, gArmsData[gSelectedArms].level, 0);
		
		RECT rcExpBox = {0, 72, 40, 80};
		RECT rcExpVal = {0, 80, 0, 88};
		RECT rcExpMax = {40, 72, 80, 80};
		RECT rcExpFlash = {40, 80, 80, 88};
		
		int lv = gArmsData[gSelectedArms].level - 1;
		int arms_code = gArmsData[gSelectedArms].code;
		int exp_now = gArmsData[gSelectedArms].exp;
		int exp_next = gArmsLevelTable[0].exp[lv + 3 * arms_code];
		
		PutBitmap3(&rcView, gArmsEnergyX + 24, 32, &rcExpBox, SURFACE_ID_TEXT_BOX);
		
		if (lv != 2 || gArmsData[gSelectedArms].exp != gArmsLevelTable[0].exp[3 * arms_code + 2])
		{
			if (exp_next)
				rcExpVal.right += 40 * exp_now / exp_next;
			else
				rcExpVal.right = 0;
			
			PutBitmap3(&rcView, gArmsEnergyX + 24, 32, &rcExpVal, SURFACE_ID_TEXT_BOX);
		}
		else
		{
			PutBitmap3(&rcView, gArmsEnergyX + 24, 32, &rcExpMax, SURFACE_ID_TEXT_BOX);
		}
		
		static int add_flash = true;
		if (gMC.exp_wait && ((add_flash++ >> 1) & 1))
			PutBitmap3(&rcView, gArmsEnergyX + 24, 32, &rcExpFlash, SURFACE_ID_TEXT_BOX);
	}
}

void PutActiveArmsList()
{
	RECT rect = {0, 0, 0, 16};

	int arms_num;
	for (arms_num = 0; gArmsData[arms_num].code != 0; ++arms_num);
	
	if (arms_num)
	{
		for (int a = 0; a < arms_num; a++)
		{
			//Get X position to draw at
			int x = 16 * (a - gSelectedArms) + gArmsEnergyX;
			
			if (x >= 8)
			{
				if (x >= 24)
					x += 48;
			}
			else
			{
				x += 16 * (arms_num + 3);
			}
			
			if (8 * (2 * (arms_num + 3) + 1) <= x)
				x += 16 * (-3 - arms_num);
			if (x < 72 && x >= 24)
				x -= 48;
			
			//Draw icon
			rect.left = 16 * gArmsData[a].code;
			rect.right = rect.left + 16;
			PutBitmap3(&grcGame, x, 16, &rect, SURFACE_ID_ARMS_IMAGE);
		}
	}
}

void PutMyLife(bool flash)
{
	RECT rcCase = {0, 40, 232, 48};
	RECT rcLife = {0, 24, 232, 32};
	RECT rcBr = {0, 32, 232, 40};
	
	if (!flash || !((gMC.shock >> 1) & 1))
	{
		if (gMC.lifeBr < gMC.life)
			gMC.lifeBr = gMC.life;
		
		if (gMC.lifeBr <= gMC.life)
			gMC.lifeBr_count = 0;
		else if (++gMC.lifeBr_count > 30)
			--gMC.lifeBr;
		
		//Draw bar
		rcCase.right = 64;
		rcLife.right = 40 * gMC.life / gMC.max_life - 1;
		rcBr.right = 40 * gMC.lifeBr / gMC.max_life - 1;
		
		PutBitmap3(&grcGame, 16, 40, &rcCase, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcGame, 40, 40, &rcBr, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcGame, 40, 40, &rcLife, SURFACE_ID_TEXT_BOX);
		PutNumber4(8, 40, gMC.lifeBr, 0);
	}
}

void PutMyAir(int x, int y)
{
	RECT rcAir[2] = {
		{112, 72, 144, 80},
		{112, 80, 144, 88},
	};

	if (!(gMC.equip & 0x10) && gMC.air_get)
	{
		//Draw how much air is left
		if (gMC.air_get % 6 <= 3)
			PutNumber4(x + 32, y, gMC.air / 10, 0);
		
		//Draw "AIR" text
		if (gMC.air % 30 <= 10)
			PutBitmap3(&grcGame, x, y, &rcAir[1], SURFACE_ID_TEXT_BOX);
		else
			PutBitmap3(&grcGame, x, y, &rcAir[0], SURFACE_ID_TEXT_BOX);
	}
}

void PutTimeCounter(int x, int y)
{
	RECT rcTime[3] = {
		{112, 104, 120, 112},
		{120, 104, 128, 112},
		{128, 104, 160, 112},
	};

	if (gMC.equip & 0x100)
	{
		//Draw clock and increase time
		if (g_GameFlags & 2)
		{
			if (time_count < 300000.0)
				time_count += 50.0 / 60.0;
			
			if ((int)time_count % 30 <= 10)
				PutBitmap3(&grcGame, x, y, &rcTime[1], SURFACE_ID_TEXT_BOX);
			else
				PutBitmap3(&grcGame, x, y, &rcTime[0], SURFACE_ID_TEXT_BOX);
		}
		else
		{
			PutBitmap3(&grcGame, x, y, &rcTime[0], SURFACE_ID_TEXT_BOX);
		}
		
		//Draw time
		PutNumber4(x,		y, (int)time_count / 3000,	false);
		PutNumber4(x + 20,	y, (int)time_count % 60,	true);
		PutNumber4(x + 32,	y, (int)time_count % 10,	false);
		PutBitmap3(&grcGame, x + 30, y, &rcTime[2], SURFACE_ID_TEXT_BOX);
	}
	else
	{
		time_count = 0.0;
	}
}

bool SaveTimeCounter()
{
	REC rec;

	//Quit if player doesn't have the Nikumaru Counter
	if (!(gMC.equip & 0x100))
		return true;
	
	//Get last time
	char path[PATH_LENGTH];
	sprintf(path, "%s/290.rec", gModulePath);
	
	FILE *fp = fopen(path, "rb");
	if (fp)
	{
		//Read data
		rec.counter[0] = File_ReadLE32(fp);
		rec.counter[1] = File_ReadLE32(fp);
		rec.counter[2] = File_ReadLE32(fp);
		rec.counter[3] = File_ReadLE32(fp);
		rec.random[0] = fgetc(fp);
		rec.random[1] = fgetc(fp);
		rec.random[2] = fgetc(fp);
		rec.random[3] = fgetc(fp);
		fclose(fp);

		uint8_t *p = (uint8_t*)&rec.counter[0];
		p[0] -= rec.random[0] >> 1;
		p[1] -= rec.random[0];
		p[2] -= rec.random[0];
		p[3] -= rec.random[0];
		
		//If this is faster than our new time, quit
		if (rec.counter[0] < time_count)
			return true;
	}
	
	//Save new time
	for (int i = 0; i < 4; i++)
	{
		rec.counter[i] = (int)time_count;
		rec.random[i] = Random(0, 250) + i;
		
		uint8_t *p = (uint8_t*)&rec.counter[i];
		p[0] += rec.random[i] >> 1;
		p[1] += rec.random[i];
		p[2] += rec.random[i];
		p[3] += rec.random[i];
	}
	
	fp = fopen(path, "wb");
	if (fp == NULL)
		return false;
	
	File_WriteLE32(rec.counter[0], fp);
	File_WriteLE32(rec.counter[1], fp);
	File_WriteLE32(rec.counter[2], fp);
	File_WriteLE32(rec.counter[3], fp);
	fputc(rec.random[0], fp);
	fputc(rec.random[1], fp);
	fputc(rec.random[2], fp);
	fputc(rec.random[3], fp);
	fclose(fp);
	return true;
}

int LoadTimeCounter()
{
	//Open file
	char path[PATH_LENGTH];
	sprintf(path, "%s/290.rec", gModulePath);
	
	FILE *fp = fopen(path, "rb");
	if (!fp)
		return 0;
	
	REC rec;
	
	//Read data
	rec.counter[0] = File_ReadLE32(fp);
	rec.counter[1] = File_ReadLE32(fp);
	rec.counter[2] = File_ReadLE32(fp);
	rec.counter[3] = File_ReadLE32(fp);
	rec.random[0] = fgetc(fp);
	rec.random[1] = fgetc(fp);
	rec.random[2] = fgetc(fp);
	rec.random[3] = fgetc(fp);
	fclose(fp);
	
	//Decode from checksum
	for (int i = 0; i < 4; i++)
	{
		uint8_t *p = (uint8_t*)&rec.counter[i];
		p[0] -= rec.random[i] >> 1;
		p[1] -= rec.random[i];
		p[2] -= rec.random[i];
		p[3] -= rec.random[i];
	}
	
	//Verify checksum's result
	if (rec.counter[0] == rec.counter[1] && rec.counter[0] == rec.counter[2])
	{
		time_count = (double)rec.counter[0];
		return rec.counter[0];
	}
	else
	{
		time_count = 0.0;
		return 0;
	}
}
