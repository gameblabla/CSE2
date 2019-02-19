#include "NpcHit.h"

#include "Back.h"
#include "Bullet.h"
#include "Caret.h"
#include "Flags.h"
#include "Game.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "TextScr.h"
#include "ValueView.h"

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
			
			if (gNPC[i].y > gWaterY + 0x800)
				gNPC[i].flag |= 0x100;
		}
	}
}

void LoseNpChar(NPCHAR *npc, bool bVanish)
{
	//Play death sound
	PlaySoundObject(npc->destroy_voice, 1);
	
	//Create smoke
	switch (npc->size)
	{
		case 1:
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 3);
			break;
		case 2:
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 7);
			break;
		case 3:
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 12);
			break;
	}
	
	//Create drop
	if (npc->exp)
	{
		int v3 = Random(1, 5);
		char v4;
		
		if (v3 == 1)
		{
			if (npc->exp <= 6)
				SetLifeObject(npc->x, npc->y, 2);
			else
				SetLifeObject(npc->x, npc->y, 6);
		}
		else if (v3 != 2 || (npc->exp <= 6 ? (v4 = SetBulletObject(npc->x, npc->y, 1)) : (v4 = SetBulletObject(npc->x, npc->y, 3)), !v4)) //TODO: what the FUCK
		{
			SetExpObjects(npc->x, npc->y, npc->exp);
		}
	}
	
	//Set flag
	SetNPCFlag(npc->code_flag);
	
	//Create value view
	if (!(npc->bits & npc_showDamage))
	{
		npc->cond = 0;
	}
	else
	{
		if ((npc->bits & npc_showDamage) && npc->damage_view)
			SetValueView(&npc->x, &npc->y, npc->damage_view);
		if (bVanish)
			VanishNpChar(npc);
	}
}

void HitNpCharBullet()
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80) && (!(gNPC[n].bits & npc_shootable) || !(gNPC[n].bits & npc_interact)))
		{
			for (int b = 0; b < BULLET_MAX; b++)
			{
				if (gBul[b].cond & 0x80 && gBul[b].damage != -1)
				{
					//Check if bullet touches npc
					bool bHit = false;
					if (gNPC[n].bits & npc_shootable
						&& gNPC[n].x - gNPC[n].hit.back < gBul[b].x + gBul[b].enemyXL
						&& gNPC[n].x + gNPC[n].hit.back > gBul[b].x - gBul[b].enemyXL
						&& gNPC[n].y - gNPC[n].hit.top < gBul[b].y + gBul[b].enemyYL
						&& gNPC[n].y + gNPC[n].hit.bottom > gBul[b].y - gBul[b].enemyYL)
						bHit = true;
					else if (gNPC[n].bits & npc_invulnerable
						&& gNPC[n].x - gNPC[n].hit.back < gBul[b].x + gBul[b].blockXL
						&& gNPC[n].x + gNPC[n].hit.back > gBul[b].x - gBul[b].blockXL
						&& gNPC[n].y - gNPC[n].hit.top < gBul[b].y + gBul[b].blockYL
						&& gNPC[n].y + gNPC[n].hit.bottom > gBul[b].y - gBul[b].blockYL)
						bHit = true;
						
					if (bHit)
					{
						//Damage NPC
						if (gNPC[n].bits & npc_shootable)
						{
							gNPC[n].life -= gBul[b].damage;
							
							if (gNPC[n].life > 0)
							{
								if (gNPC[n].shock < 14)
								{
									SetCaret((gBul[b].x + gNPC[n].x) / 2, (gBul[b].y + gNPC[n].y) / 2, 11, 0);
									SetCaret((gBul[b].x + gNPC[n].x) / 2, (gBul[b].y + gNPC[n].y) / 2, 11, 0);
									SetCaret((gBul[b].x + gNPC[n].x) / 2, (gBul[b].y + gNPC[n].y) / 2, 11, 0);
									PlaySoundObject(gNPC[n].hit_voice, 1);
									gNPC[n].shock = 16;
								}
								
								if (gNPC[n].bits & npc_showDamage)
									gNPC[n].damage_view -= gBul[b].damage;
							}
							else
							{
								gNPC[n].life = 0;
								
								if (gNPC[n].bits & npc_showDamage)
									gNPC[n].damage_view -= gBul[b].damage;
								
								if ((gMC.cond & 0x80) && gNPC[n].bits & npc_eventDie)
									StartTextScript(gNPC[n].code_event);
								else
									gNPC[n].cond |= 8;
							}
						}
						//Hit invulnerable NPC
						else if (gBul[b].code_bullet != 13
							&& gBul[b].code_bullet != 14
							&& gBul[b].code_bullet != 15
							&& gBul[b].code_bullet != 28
							&& gBul[b].code_bullet != 29
							&& gBul[b].code_bullet != 30
							&& !(gBul[b].bbits & 0x10))
						{
							SetCaret((gBul[b].x + gNPC[n].x) / 2, (gBul[b].y + gNPC[n].y) / 2, 2, 2);
							PlaySoundObject(31, 1);
							gBul[b].life = 0;
							continue;
						}
						
						--gBul[b].life;
					}
				}
			}
			
			if (gNPC[n].cond & 8)
				LoseNpChar(&gNPC[n], true);
		}
	}
}
