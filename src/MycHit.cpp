#include <stdint.h>

#include "WindowsWrapper.h"

#include "MyChar.h"
#include "Map.h"
#include "Sound.h"
#include "Caret.h"
#include "Back.h"
#include "KeyControl.h"

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
			
			//Spikes
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
