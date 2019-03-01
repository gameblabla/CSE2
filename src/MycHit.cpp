#include "MycHit.h"

#include <stdint.h>

#include "WindowsWrapper.h"

#include "Back.h"
#include "Boss.h"
#include "Caret.h"
#include "Game.h"
#include "KeyControl.h"
#include "Map.h"
#include "MyChar.h"
#include "MycParam.h"
#include "NpChar.h"
#include "Sound.h"
#include "TextScr.h"

void ResetMyCharFlag()
{
	gMC.flag = 0;
}

void PutlittleStar()
{
	if (!(gMC.cond & 2) && gMC.ym < -0x200)
	{
		PlaySoundObject(3, 1);
		SetCaret(gMC.x, gMC.y - gMC.hit.top, 13, 0);
		SetCaret(gMC.x, gMC.y - gMC.hit.top, 13, 0);
	}
}

int JudgeHitMyCharBlock(int x, int y)
{
	int hit = 0;
	
	//Left wall
	if (gMC.y - gMC.hit.top < (2 * (2 * y + 1) - 1) << 11
		&& gMC.y + gMC.hit.bottom > (2 * (2 * y - 1) + 1) << 11
		&& gMC.x - gMC.hit.left < (2 * x + 1) << 12
		&& gMC.x - gMC.hit.left > x << 13)
	{
		//Clip
		gMC.x = ((2 * x + 1) << 12) + gMC.hit.left;
		
		//Halt momentum
		if (gMC.xm < -0x180)
			gMC.xm = -0x180;
		if (!(gKey & gKeyLeft) && gMC.xm < 0)
			gMC.xm = 0;
		
		//Set that a left wall was hit
		hit |= 1;
	}
	
	//Right wall
	if (gMC.y - gMC.hit.top < (2 * (2 * y + 1) - 1) << 11
		&& gMC.y + gMC.hit.bottom > (2 * (2 * y - 1) + 1) << 11
		&& gMC.x + gMC.hit.right > (2 * x - 1) << 12
		&& gMC.x + gMC.hit.left < x << 13)
	{
		//Clip
		gMC.x = ((2 * x - 1) << 12) - gMC.hit.right;
		
		//Halt momentum
		if (gMC.xm > 0x180)
			gMC.xm = 0x180;
		if (!(gKey & gKeyRight) && gMC.xm > 0)
			gMC.xm = 0;
		
		//Set that a right wall was hit
		hit |= 4;
	}
	
	//Ceiling
	if (gMC.x - gMC.hit.right < ((2 * x + 1) << 12) - 0x600
		&& gMC.x + gMC.hit.right > ((2 * x - 1) << 12) + 0x600
		&& gMC.y - gMC.hit.top < (2 * y + 1) << 12
		&& gMC.y - gMC.hit.top > y << 13)
	{
		//Clip
		gMC.y = ((2 * y + 1) << 12) + gMC.hit.top;
		
		//Halt momentum
		if (!(gMC.cond & 2) && gMC.ym < -0x200)
			PutlittleStar();
		if (gMC.ym < 0)
			gMC.ym = 0;
		
		//Set that a ceiling was hit
		hit |= 2;
	}
	
	//Floor
	if (gMC.x - gMC.hit.right < ((2 * x + 1) << 12) - 0x600
		&& gMC.x + gMC.hit.right > ((2 * x - 1) << 12) + 0x600
		&& gMC.y + gMC.hit.bottom > (2 * y - 1) << 12
		&& gMC.y + gMC.hit.bottom < y << 13)
	{
		//Clip
		gMC.y = ((2 * y - 1) << 12) - gMC.hit.bottom;
		
		//Halt momentum
		if (gMC.ym > 0x400)
			PlaySoundObject(23, 1);
		if (gMC.ym > 0)
			gMC.ym = 0;
		
		//Set that a floor was hit
		hit |= 8;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleA(int x, int y)
{
	int hit = 0;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y - gMC.hit.top < (y << 13) - (-0x2000 * x + gMC.x) / 2 + 0x800
		&& gMC.y + gMC.hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) - (-0x2000 * x + gMC.x) / 2 + 0x800 + gMC.hit.top;
		
		//Halt momentum
		if (!(gMC.cond & 2) && gMC.ym < -0x200)
			PutlittleStar();
		if (gMC.ym < 0)
			gMC.ym = 0;
		
		//Set that hit a ceiling
		hit |= 2;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleB(int x, int y)
{
	int hit = 0;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y - gMC.hit.top < (y << 13) - (-0x2000 * x + gMC.x) / 2 - 0x800
		&& gMC.y + gMC.hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) - (-0x2000 * x + gMC.x) / 2 - 0x800 + gMC.hit.top;
		
		//Halt momentum
		if (!(gMC.cond & 2) && gMC.ym < -0x200)
			PutlittleStar();
		if (gMC.ym < 0)
			gMC.ym = 0;
		
		//Set that hit a ceiling
		hit |= 2;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleC(int x, int y)
{
	int hit = 0;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y - gMC.hit.top < (y << 13) + (-0x2000 * x + gMC.x) / 2 - 0x800
		&& gMC.y + gMC.hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) + (-0x2000 * x + gMC.x) / 2 - 0x800 + gMC.hit.top;
		
		//Halt momentum
		if (!(gMC.cond & 2) && gMC.ym < -0x200)
			PutlittleStar();
		if (gMC.ym < 0)
			gMC.ym = 0;
		
		//Set that hit a ceiling
		hit |= 2;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleD(int x, int y)
{
	int hit = 0;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y - gMC.hit.top < (y << 13) + (-0x2000 * x + gMC.x) / 2 + 0x800
		&& gMC.y + gMC.hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) + (-0x2000 * x + gMC.x) / 2 + 0x800 + gMC.hit.top;
		
		//Halt momentum
		if (!(gMC.cond & 2) && gMC.ym < -0x200)
			PutlittleStar();
		if (gMC.ym < 0)
			gMC.ym = 0;
		
		//Set that hit a ceiling
		hit |= 2;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleE(int x, int y)
{
	int hit = 0x10000;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y + gMC.hit.bottom > (y << 13) + (-0x2000 * x + gMC.x) / 2 - 0x800
		&& gMC.y - gMC.hit.top < (2 * y + 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) + (-0x2000 * x + gMC.x) / 2 - 0x800 - gMC.hit.bottom;
		
		//Halt momentum
		if (gMC.ym > 0x400)
			PlaySoundObject(23, 1);
		if (gMC.ym > 0)
			gMC.ym = 0;
		
		//Set that hit this slope
		hit = 0x10028;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleF(int x, int y)
{
	int hit = 0x20000;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y + gMC.hit.bottom > (y << 13) + (-0x2000 * x + gMC.x) / 2 + 0x800
		&& gMC.y - gMC.hit.top < (2 * y + 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) + (-0x2000 * x + gMC.x) / 2 + 0x800 - gMC.hit.bottom;
		
		//Halt momentum
		if (gMC.ym > 0x400)
			PlaySoundObject(23, 1);
		if (gMC.ym > 0)
			gMC.ym = 0;
		
		//Set that hit this slope
		hit = 0x20028;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleG(int x, int y)
{
	int hit = 0x40000;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y + gMC.hit.bottom > (y << 13) - (-0x2000 * x + gMC.x) / 2 + 0x800
		&& gMC.y - gMC.hit.top < (2 * y + 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) - (-0x2000 * x + gMC.x) / 2 + 0x800 - gMC.hit.bottom;
		
		//Halt momentum
		if (gMC.ym > 0x400)
			PlaySoundObject(23, 1);
		if (gMC.ym > 0)
			gMC.ym = 0;
		
		//Set that hit this slope
		hit = 0x40018;
	}
	
	return hit;
}

int JudgeHitMyCharTriangleH(int x, int y)
{
	int hit = 0x80000;
	
	if (gMC.x < (2 * x + 1) << 12
		&& gMC.x > (2 * x - 1) << 12
		&& gMC.y + gMC.hit.bottom > (y << 13) - (-0x2000 * x + gMC.x) / 2 - 0x800
		&& gMC.y - gMC.hit.top < (2 * y + 1) << 12)
	{
		//Clip
		gMC.y = (y << 13) - (-0x2000 * x + gMC.x) / 2 - 0x800 - gMC.hit.bottom;
		
		//Halt momentum
		if (gMC.ym > 0x400)
			PlaySoundObject(23, 1);
		if (gMC.ym > 0)
			gMC.ym = 0;
		
		//Set that hit this slope
		hit = 0x80018;
	}
	
	return hit;
}

int JudgeHitMyCharWater(int x, int y)
{
	int hit = 0;
	
	if (gMC.x - gMC.hit.right < ((2 * x + 1) << 12) - 0x600
		&& gMC.x + gMC.hit.right > ((2 * x - 1) << 12) + 0x600
		&& gMC.y - gMC.hit.top < ((2 * y + 1) << 12) - 0x600
		&& gMC.y + gMC.hit.bottom > y << 13)
		hit = 0x100;
	
	return hit;
}

int JudgeHitMyCharDamage(int x, int y)
{
	int hit = 0;
	
	if (gMC.x - 0x800 < (4 * x + 1) << 11
		&& gMC.x + 0x800 > (4 * x - 1) << 11
		&& gMC.y - 0x800 < (y << 13) + 0x600
		&& gMC.y + 0x800 > (y << 13) - 0x600)
		hit = 0x400;
	
	return hit;
}

int JudgeHitMyCharDamageW(int x, int y)
{
	int hit = 0;
	
	if (gMC.x - 0x800 < (4 * x + 1) << 11
		&& gMC.x + 0x800 > (4 * x - 1) << 11
		&& gMC.y - 0x800 < (y << 13) + 0x600
		&& gMC.y + 0x800 > (y << 13) - 0x600)
		hit = 0xD00;
	
	return hit;
}

int JudgeHitMyCharVectLeft(int x, int y)
{
	int hit = 0;
	if (gMC.x - gMC.hit.right < (4 * (2 * x + 1) - 1) << 10
		&& gMC.x + gMC.hit.right > (4 * (2 * x - 1) + 1) << 10
		&& gMC.y - gMC.hit.top < (4 * (2 * y + 1) - 1) << 10
		&& gMC.y + gMC.hit.bottom > (4 * (2 * y - 1) + 1) << 10)
		hit = 0x1000;
	
	return hit;
}

int JudgeHitMyCharVectUp(int x, int y)
{
	int hit = 0;
	if (gMC.x - gMC.hit.right < (4 * (2 * x + 1) - 1) << 10
		&& gMC.x + gMC.hit.right > (4 * (2 * x - 1) + 1) << 10
		&& gMC.y - gMC.hit.top < (4 * (2 * y + 1) - 1) << 10
		&& gMC.y + gMC.hit.bottom > (4 * (2 * y - 1) + 1) << 10)
		hit = 0x2000;
	
	return hit;
}

int JudgeHitMyCharVectRight(int x, int y)
{
	int hit = 0;
	if (gMC.x - gMC.hit.right < (4 * (2 * x + 1) - 1) << 10
		&& gMC.x + gMC.hit.right > (4 * (2 * x - 1) + 1) << 10
		&& gMC.y - gMC.hit.top < (4 * (2 * y + 1) - 1) << 10
		&& gMC.y + gMC.hit.bottom > (4 * (2 * y - 1) + 1) << 10)
		hit = 0x4000;
	
	return hit;
}

int JudgeHitMyCharVectDown(int x, int y)
{
	int hit = 0;
	if (gMC.x - gMC.hit.right < (4 * (2 * x + 1) - 1) << 10
		&& gMC.x + gMC.hit.right > (4 * (2 * x - 1) + 1) << 10
		&& gMC.y - gMC.hit.top < (4 * (2 * y + 1) - 1) << 10
		&& gMC.y + gMC.hit.bottom > (4 * (2 * y - 1) + 1) << 10)
		hit = 0x8000;
	
	return hit;
}

void HitMyCharMap()
{
	int x = gMC.x / 0x2000;
	int y = gMC.y / 0x2000;
	
	int offy[4];
	int offx[4];
	offx[0] = 0;
	offx[1] = 1;
	offx[2] = 0;
	offx[3] = 1;
	offy[0] = 0;
	offy[1] = 0;
	offy[2] = 1;
	offy[3] = 1;
	
	uint8_t atrb[4];
	for (int i = 0; i < 4; i++)
	{
		atrb[i] = GetAttribute(x + offx[i], y + offy[i]);
		
		switch (atrb[i])
		{
			//Water
			case 0x02:
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			//Block
			case 0x05:
			case 0x41:
			case 0x43:
			case 0x46:
				gMC.flag |= JudgeHitMyCharBlock(x + offx[i], y + offy[i]);
				break;
				
			//Spikes
			case 0x42:
				gMC.flag |= JudgeHitMyCharDamage(x + offx[i], y + offy[i]);
				break;
				
			//Slopes
			case 0x50:
				gMC.flag |= JudgeHitMyCharTriangleA(x + offx[i], y + offy[i]);
				break;
				
			case 0x51:
				gMC.flag |= JudgeHitMyCharTriangleB(x + offx[i], y + offy[i]);
				break;
				
			case 0x52:
				gMC.flag |= JudgeHitMyCharTriangleC(x + offx[i], y + offy[i]);
				break;
				
			case 0x53:
				gMC.flag |= JudgeHitMyCharTriangleD(x + offx[i], y + offy[i]);
				break;
				
			case 0x54:
				gMC.flag |= JudgeHitMyCharTriangleE(x + offx[i], y + offy[i]);
				break;
				
			case 0x55:
				gMC.flag |= JudgeHitMyCharTriangleF(x + offx[i], y + offy[i]);
				break;
				
			case 0x56:
				gMC.flag |= JudgeHitMyCharTriangleG(x + offx[i], y + offy[i]);
				break;
				
			case 0x57:
				gMC.flag |= JudgeHitMyCharTriangleH(x + offx[i], y + offy[i]);
				break;
				
			//Water and water blocks
			case 0x60:
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x61:
				gMC.flag |= JudgeHitMyCharBlock(x + offx[i], y + offy[i]);
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
			
			//Water spikes
			case 0x62:
				gMC.flag |= JudgeHitMyCharDamageW(x + offx[i], y + offy[i]);
				break;
				
			//Water slopes
			case 0x70:
				gMC.flag |=  JudgeHitMyCharTriangleA(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x71:
				gMC.flag |=  JudgeHitMyCharTriangleB(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x72:
				gMC.flag |=  JudgeHitMyCharTriangleC(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x73:
				gMC.flag |=  JudgeHitMyCharTriangleD(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x74:
				gMC.flag |=  JudgeHitMyCharTriangleE(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x75:
				gMC.flag |=  JudgeHitMyCharTriangleF(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x76:
				gMC.flag |=  JudgeHitMyCharTriangleG(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0x77:
				gMC.flag |=  JudgeHitMyCharTriangleH(x + offx[i], y + offy[i]);
				gMC.flag |=  JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			//Wind
			case 0x80:
				gMC.flag |= JudgeHitMyCharVectLeft(x + offx[i], y + offy[i]);
				break;
				
			case 0x81:
				gMC.flag |= JudgeHitMyCharVectUp(x + offx[i], y + offy[i]);
				break;
				
			case 0x82:
				gMC.flag |= JudgeHitMyCharVectRight(x + offx[i], y + offy[i]);
				break;
				
			case 0x83:
				gMC.flag |= JudgeHitMyCharVectDown(x + offx[i], y + offy[i]);
				break;
				
			//Water current
			case 0xA0:
				gMC.flag |= JudgeHitMyCharVectLeft(x + offx[i], y + offy[i]);
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			case 0xA1:
				gMC.flag |= JudgeHitMyCharVectUp(x + offx[i], y + offy[i]);
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
			
			case 0xA2:
				gMC.flag |= JudgeHitMyCharVectRight(x + offx[i], y + offy[i]);
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
			
			case 0xA3:
				gMC.flag |= JudgeHitMyCharVectDown(x + offx[i], y + offy[i]);
				gMC.flag |= JudgeHitMyCharWater(x + offx[i], y + offy[i]);
				break;
				
			default:
				break;
		}
	}
	
	if (gMC.y > gWaterY + 0x800)
		gMC.flag |= 0x100;
}

int JudgeHitMyCharNPC(NPCHAR *npc)
{
	int hit = 0;
	
	if (gMC.y - gMC.hit.top < npc->y + npc->hit.bottom - 0x600
		&& gMC.y + gMC.hit.bottom > npc->y - npc->hit.top + 0x600
		&& gMC.x - gMC.hit.right < npc->x + npc->hit.back
		&& gMC.x - gMC.hit.right > npc->x)
	{
		if (gMC.xm < 0x200)
			gMC.xm += 0x200;
		hit |= 1;
	}
	
	if (gMC.y - gMC.hit.top < npc->y + npc->hit.bottom - 0x600
		&& gMC.y + gMC.hit.bottom > npc->y - npc->hit.top + 0x600
		&& gMC.x + gMC.hit.right - 0x200 > npc->x - npc->hit.back
		&& gMC.x + gMC.hit.right - 0x200 < npc->x)
	{
		if (gMC.xm > -0x200)
			gMC.xm -= 0x200;
		hit |= 4;
	}
	
	if (gMC.x - gMC.hit.right < npc->x + npc->hit.back - 0x600
		&& gMC.x + gMC.hit.right > npc->x - npc->hit.back + 0x600
		&& gMC.y - gMC.hit.top < npc->y + npc->hit.bottom
		&& gMC.y - gMC.hit.top > npc->y)
	{
		if (gMC.ym < 0)
			gMC.ym = 0;
		hit |= 2;
	}
	
	if (gMC.x - gMC.hit.right < npc->x + npc->hit.back - 0x600
		&& gMC.x + gMC.hit.right > npc->x - npc->hit.back + 0x600
		&& gMC.y + gMC.hit.bottom > npc->y - npc->hit.top
		&& gMC.hit.bottom + gMC.y < npc->y + 0x600)
	{
		if (npc->bits & npc_bouncy)
		{
			gMC.ym = npc->ym - 0x200;
			hit |= 8;
		}
		else if (!(gMC.flag & 8) && gMC.ym > npc->ym)
		{
			gMC.y = npc->y - npc->hit.top - gMC.hit.bottom + 0x200;
			gMC.ym = npc->ym;
			gMC.x += npc->xm;
			hit |= 8;
		}
	}
	return hit;
}

int JudgeHitMyCharNPC3(NPCHAR *npc)
{
	if (npc->direct)
	{
		if (gMC.x + 0x400 > npc->x - npc->hit.back
			&& gMC.x - 0x400 < npc->x + npc->hit.front
			&& gMC.y + 0x400 > npc->y - npc->hit.top
			&& gMC.y - 0x400 < npc->y + npc->hit.bottom)
			return 1;
	}
	else
	{
		if (gMC.x + 0x400 > npc->x - npc->hit.front
			&& gMC.x - 0x400 < npc->x + npc->hit.back
			&& gMC.y + 0x400 > npc->y - npc->hit.top
			&& gMC.y - 0x400 < npc->y + npc->hit.bottom)
			return 1;
	}
	
	return 0;
}

int JudgeHitMyCharNPC4(NPCHAR *npc)
{
	//TODO: comment this
	int hit = 0;
	long double v1, v2;
	
	if (npc->x <= gMC.x)
		v1 = (long double)(gMC.x - npc->x);
	else
		v1 = (long double)(npc->x - gMC.x);
	
	float fx1 = (float)v1;
	
	if (npc->y <= gMC.y)
		v2 = (long double)(gMC.y - npc->y);
	else
		v2 = (long double)(npc->y - gMC.y);
	
	float fx2 = (float)npc->hit.back;
	if (0.0 == fx1)
		fx1 = 1.0;
	if (0.0 == fx2)
		fx2 = 1.0;
	
	float fy1 = (float)v2;
	float fy2 = (float)npc->hit.top;
	
	if (fy1 / fx1 <= fy2 / fx2)
	{
		if (gMC.y - gMC.hit.top < npc->y + npc->hit.bottom && gMC.y + gMC.hit.bottom > npc->y - npc->hit.top)
		{
			if (gMC.x - gMC.hit.right < npc->x + npc->hit.back && gMC.x - gMC.hit.right > npc->x)
			{
				if ( gMC.xm < npc->xm )
					gMC.xm = npc->xm;
				gMC.x = npc->hit.back + npc->x + gMC.hit.right;
				hit |= 1;
			}
			
			if (gMC.x + gMC.hit.right > npc->x - npc->hit.back && gMC.hit.right + gMC.x < npc->x)
			{
				if ( gMC.xm > npc->xm )
					gMC.xm = npc->xm;
				gMC.x = npc->x - npc->hit.back - gMC.hit.right;
				hit |= 4;
			}
		}
	}
	else if (gMC.x - gMC.hit.right < npc->x + npc->hit.back && gMC.x + gMC.hit.right > npc->x - npc->hit.back)
	{
		if (gMC.y - gMC.hit.top < npc->y + npc->hit.bottom && gMC.y - gMC.hit.top > npc->y)
		{
			if (gMC.ym >= npc->ym)
			{
				if (gMC.ym < 0)
					gMC.ym = 0;
			}
			else
			{
				gMC.y = npc->hit.bottom + npc->y + gMC.hit.top + 0x200;
				gMC.ym = npc->ym;
			}
			
			hit |= 2;
		}
		
		if (gMC.y + gMC.hit.bottom > npc->y - npc->hit.top && gMC.hit.bottom + gMC.y < npc->y + 0x600)
		{
			if (gMC.ym - npc->ym > 0x400)
				PlaySoundObject(23, 1);
			
			if (gMC.unit == 1)
			{
				gMC.y = npc->y - npc->hit.top - gMC.hit.bottom + 0x200;
				hit |= 8;
			}
			else if (npc->bits & npc_bouncy)
			{
				gMC.ym = npc->ym - 0x200;
				hit |= 8;
			}
			else if (!(gMC.flag & 8) && gMC.ym > npc->ym)
			{
				gMC.y = npc->y - npc->hit.top - gMC.hit.bottom + 0x200;
				gMC.ym = npc->ym;
				gMC.x += npc->xm;
				hit |= 8;
			}
		}
	}
	
	return hit;
}

void HitMyCharNpChar()
{
	if ((gMC.cond & 0x80) && !(gMC.cond & 2))
	{
		int hit;
		
		for (int i = 0; i < NPC_MAX; i++)
		{
			if (gNPC[i].cond & 0x80)
			{
				if (gNPC[i].bits & npc_solidSoft)
				{
					hit = JudgeHitMyCharNPC(&gNPC[i]);
					gMC.flag |= hit;
				}
				else if (gNPC[i].bits & npc_solidHard)
				{
					hit = JudgeHitMyCharNPC4(&gNPC[i]);
					gMC.flag |= hit;
				}
				else
				{
					hit = JudgeHitMyCharNPC3(&gNPC[i]);
				}
				
				//Special NPCs (pickups)
				if (hit && gNPC[i].code_char == 1)
				{
					PlaySoundObject(14, 1);
					AddExpMyChar(gNPC[i].exp);
					gNPC[i].cond = 0;
				}
				
				if (hit && gNPC[i].code_char == 86)
				{
					PlaySoundObject(42, 1);
					AddBulletMyChar(gNPC[i].code_event, gNPC[i].exp);
					gNPC[i].cond = 0;
				}
				
				if (hit && gNPC[i].code_char == 87)
				{
					PlaySoundObject(20, 1);
					AddLifeMyChar(gNPC[i].exp);
					gNPC[i].cond = 0;
				}
				
				//Run event on contact
				if (!(g_GameFlags & 4) && hit && gNPC[i].bits & npc_eventTouch)
					StartTextScript(gNPC[i].code_event);
				
				//NPC damage
				if (g_GameFlags & 2 && !(gNPC[i].bits & npc_interact))
				{
					if (gNPC[i].bits & npc_rearTop)
					{
						if (hit & 4 && gNPC[i].xm < 0)
							DamageMyChar(gNPC[i].damage);
						if (hit & 1 && gNPC[i].xm > 0)
							DamageMyChar(gNPC[i].damage);
						if (hit & 8 && gNPC[i].ym < 0)
							DamageMyChar(gNPC[i].damage);
						if (hit & 2 && gNPC[i].ym > 0)
							DamageMyChar(gNPC[i].damage);
					}
					else if (hit && gNPC[i].damage && !(g_GameFlags & 4))
					{
						DamageMyChar(gNPC[i].damage);
					}
				}
				
				//Interaction
				if (!(g_GameFlags & 4) && hit && gMC.cond & 1 && gNPC[i].bits & npc_interact)
				{
					StartTextScript(gNPC[i].code_event);
					gMC.xm = 0;
					gMC.ques = 0;
				}
			}
		}
		
		//Create question mark when NPC hasn't been interacted with
		if (gMC.ques)
			SetCaret(gMC.x, gMC.y, 9, 0);
	}
}

void HitMyCharBoss()
{
	if ((gMC.cond & 0x80) && !(gMC.cond & 2))
	{
		for (int b = 0; b < BOSS_MAX; b++)
		{
			if (gBoss[b].cond & 0x80)
			{
				int hit;
				if (gBoss[b].bits & npc_solidSoft)
				{
					hit = JudgeHitMyCharNPC(&gBoss[b]);
					gMC.flag |= hit;
				}
				else if (gBoss[b].bits & npc_solidHard)
				{
					hit = JudgeHitMyCharNPC4(&gBoss[b]);
					gMC.flag |= hit;
				}
				else
				{
					hit = JudgeHitMyCharNPC3(&gBoss[b]);
				}
				
				if (!(g_GameFlags & 4) && hit && gBoss[b].bits & npc_eventTouch)
				{
					StartTextScript(gBoss[b].code_event);
					gMC.ques = 0;
				}
				
				if (gBoss[b].bits & npc_rearTop)
				{
					if (hit & 4 && gBoss[b].xm < 0)
						DamageMyChar(gBoss[b].damage);
					if (hit & 1 && gBoss[b].xm > 0)
						DamageMyChar(gBoss[b].damage);
				}
				else if (hit && gBoss[b].damage && !(g_GameFlags & 4))
				{
					DamageMyChar(gBoss[b].damage);
				}
				
				if (!(g_GameFlags & 4) && hit && (gMC.cond & 1) && gBoss[b].bits & npc_interact)
				{
					StartTextScript(gBoss[b].code_event);
					gMC.xm = 0;
					gMC.ques = 0;
				}
			}
		}
		
		if (gMC.ques)
			SetCaret(gMC.x, gMC.y, 9, 0);
	}
}