#include "NpChar.h"
#include "Map.h"

void JadgeHitNpCharBlock(NPCHAR *npc, int x, int y)
{
	int hit = 0;

	if (npc->y - npc->hit.top < ((2 * y + 1) << 12) - 0x600
		&& npc->y + npc->hit.bottom > ((2 * y - 1) << 12) + 0x600
		&& npc->x - npc->hit.back < (2 * x + 1) << 12
		&& npc->x - npc->hit.back > x << 13)
	{
		npc->x = ((2 * x + 1) << 12) + npc->hit.back;
		hit |= 1;
	}
	
	if (npc->y - npc->hit.top < ((2 * y + 1) << 12) - 0x600
		&& npc->y + npc->hit.bottom > ((2 * y - 1) << 12) + 0x600
		&& npc->hit.back + npc->x > (2 * x - 1) << 12
		&& npc->hit.back + npc->x < x << 13)
	{
		npc->x = ((2 * x - 1) << 12) - npc->hit.back;
		hit |= 4;
	}
	
	if (npc->x - npc->hit.back < ((2 * x + 1) << 12) - 0x600
		&& npc->hit.back + npc->x > ((2 * x - 1) << 12) + 0x600
		&& npc->y - npc->hit.top < (2 * y + 1) << 12
		&& npc->y - npc->hit.top > y << 13)
	{
		npc->y = ((2 * y + 1) << 12) + npc->hit.top;
		npc->ym = 0;
		hit |= 2;
	}
	
	if (npc->x - npc->hit.back < ((2 * x + 1) << 12) - 0x600
		&& npc->hit.back + npc->x > ((2 * x - 1) << 12) + 0x600
		&& npc->y + npc->hit.bottom > (2 * y - 1) << 12
		&& npc->y + npc->hit.bottom < y << 13)
	{
		npc->y = ((2 * y - 1) << 12) - npc->hit.bottom;
		npc->ym = 0;
		hit |= 8;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleA(NPCHAR *npc, int x, int y)
{
	int hit = 0;
	
	if (npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y - npc->hit.top < (y << 13) - (-0x2000 * x + npc->x) / 2 + 0x800
		&& npc->y + npc->hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		npc->y = npc->hit.top + (y << 13) - (-0x2000 * x + npc->x) / 2 + 0x800;
		
		//Halt momentum
		if (npc->ym < 0)
			npc->ym = 0;
		
		//Set that hit a ceiling
		hit = 2;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleB(NPCHAR *npc, int x, int y)
{
	int hit = 0;
	
	if (npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y - npc->hit.top < (y << 13) - (-0x2000 * x + npc->x) / 2 - 0x800
		&& npc->y + npc->hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		npc->y = npc->hit.top + (y << 13) - (-0x2000 * x + npc->x) / 2 - 0x800;
		
		//Halt momentum
		if (npc->ym < 0)
			npc->ym = 0;
		
		//Set that hit a ceiling
		hit = 2;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleC(NPCHAR *npc, int x, int y)
{
	int hit = 0;
	
	if (npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y - npc->hit.top < (y << 13) + (-0x2000 * x + npc->x) / 2 - 0x800
		&& npc->y + npc->hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		npc->y = npc->hit.top + (y << 13) + (-0x2000 * x + npc->x) / 2 - 0x800;
		
		//Halt momentum
		if (npc->ym < 0)
			npc->ym = 0;
		
		//Set that hit a ceiling
		hit = 2;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleD(NPCHAR *npc, int x, int y)
{
	int hit = 0;
	
	if (npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y - npc->hit.top < (y << 13) + (-0x2000 * x + npc->x) / 2 + 0x800
		&& npc->y + npc->hit.bottom > (2 * y - 1) << 12)
	{
		//Clip
		npc->y = npc->hit.top + (y << 13) + (-0x2000 * x + npc->x) / 2 + 0x800;
		
		//Halt momentum
		if (npc->ym < 0)
			npc->ym = 0;
		
		//Set that hit a ceiling
		hit = 2;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleE(NPCHAR *npc, int x, int y)
{
	int hit = 0x10000;

	if ( npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y + npc->hit.bottom > (y << 13) + (-0x2000 * x + npc->x) / 2 - 0x800
		&& npc->y - npc->hit.top < (2 * y + 1) << 12 )
	{
		//Clip
		npc->y = (y << 13) + (-0x2000 * x + npc->x) / 2 - 0x800 - npc->hit.bottom;
		
		//Halt momentum
		if (npc->ym > 0)
			npc->ym = 0;
		
		//Set that hit this slope
		hit = 0x10028;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleF(NPCHAR *npc, int x, int y)
{
	int hit = 0x20000;

	if ( npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y + npc->hit.bottom > (y << 13) + (-0x2000 * x + npc->x) / 2 + 0x800
		&& npc->y - npc->hit.top < (2 * y + 1) << 12 )
	{
		//Clip
		npc->y = (y << 13) + (-0x2000 * x + npc->x) / 2 + 0x800 - npc->hit.bottom;
		
		//Halt momentum
		if (npc->ym > 0)
			npc->ym = 0;
		
		//Set that hit this slope
		hit = 0x20028;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleG(NPCHAR *npc, int x, int y)
{
	int hit = 0x40000;

	if ( npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y + npc->hit.bottom > (y << 13) - (-0x2000 * x + npc->x) / 2 + 0x800
		&& npc->y - npc->hit.top < (2 * y + 1) << 12 )
	{
		//Clip
		npc->y = (y << 13) - (-0x2000 * x + npc->x) / 2 + 0x800 - npc->hit.bottom;
		
		//Halt momentum
		if (npc->ym > 0)
			npc->ym = 0;
		
		//Set that hit this slope
		hit = 0x40018;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharTriangleH(NPCHAR *npc, int x, int y)
{
	int hit = 0x80000;

	if ( npc->x < (2 * x + 1) << 12
		&& npc->x > (2 * x - 1) << 12
		&& npc->y + npc->hit.bottom > (y << 13) - (-0x2000 * x + npc->x) / 2 - 0x800
		&& npc->y - npc->hit.top < (2 * y + 1) << 12 )
	{
		//Clip
		npc->y = (y << 13) - (-0x2000 * x + npc->x) / 2 - 0x800 - npc->hit.bottom;
		
		//Halt momentum
		if (npc->ym > 0)
			npc->ym = 0;
		
		//Set that hit this slope
		hit = 0x80018;
	}
	
	npc->flag |= hit;
}

void JudgeHitNpCharWater(NPCHAR *npc, int x, int y)
{
	int hit = 0;
	
	if (npc->x - npc->hit.back < (4 * (2 * x + 1) - 1) << 10
		&& npc->hit.back + npc->x > (4 * (2 * x - 1) + 1) << 10
		&& npc->y - npc->hit.top < (4 * (2 * y + 1) - 1) << 10
		&& npc->y + npc->hit.bottom > (4 * (2 * y - 1) + 1) << 10)
		hit = 0x100;
	
	npc->flag |= hit;
}

void HitNpCharMap()
{
	int offy[9];
	int offx[9];
	
	offx[0] = 0;
	offx[1] = 1;
	offx[2] = 0;
	offx[3] = 1;
	offx[4] = 2;
	offx[5] = 2;
	offx[6] = 2;
	offx[7] = 0;
	offx[8] = 1;
	
	offy[0] = 0;
	offy[1] = 0;
	offy[2] = 1;
	offy[3] = 1;
	offy[4] = 0;
	offy[5] = 1;
	offy[6] = 2;
	offy[7] = 2;
	offy[8] = 2;
	
	for (int i = 0; i < NPC_MAX; i++)
	{
		if ((gNPC[i].cond & 0x80) && !(gNPC[i].bits & 8))
		{
			int judg, x, y;
			if (gNPC[i].size <= 2)
			{
				judg = 4;
				x = gNPC[i].x / 0x2000;
				y = gNPC[i].y / 0x2000;
			}
			else
			{
				judg = 9;
				x = (gNPC[i].x - 0x1000) / 0x2000;
				y = (gNPC[i].y - 0x1000) / 0x2000;
			}
			
			gNPC[i].flag = 0;
			
			for (int j = 0; j < judg; j++)
			{
				switch (GetAttribute(x + offx[j], y + offy[j]))
				{
					//Water
					case 0x02:
					case 0x60:
					case 0x62:
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					//Block
					case 0x03:
					case 0x05:
					case 0x41:
					case 0x43:
						JadgeHitNpCharBlock(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					//Water block
					case 0x04:
					case 0x61:
					case 0x64:
						JadgeHitNpCharBlock(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					//No NPC block
					case 0x44:
						if (!(gNPC[i].bits & npc_ignore44))
							JadgeHitNpCharBlock(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					//Slopes
					case 0x50:
						JudgeHitNpCharTriangleA(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x51:
						JudgeHitNpCharTriangleB(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x52:
						JudgeHitNpCharTriangleC(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x53:
						JudgeHitNpCharTriangleD(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x54:
						JudgeHitNpCharTriangleE(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x55:
						JudgeHitNpCharTriangleF(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x56:
						JudgeHitNpCharTriangleG(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x57:
						JudgeHitNpCharTriangleH(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					//Water slopes
					case 0x70:
						JudgeHitNpCharTriangleA(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x71:
						JudgeHitNpCharTriangleB(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x72:
						JudgeHitNpCharTriangleC(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x73:
						JudgeHitNpCharTriangleD(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x74:
						JudgeHitNpCharTriangleE(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x75:
						JudgeHitNpCharTriangleF(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x76:
						JudgeHitNpCharTriangleG(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
						
					case 0x77:
						JudgeHitNpCharTriangleH(&gNPC[i], x + offx[j], y + offy[j]);
						JudgeHitNpCharWater(&gNPC[i], x + offx[j], y + offy[j]);
						break;
				}
			}
		}
	}
}