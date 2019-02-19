#include "BulHit.h"

#include <stdio.h>
#include <string.h>

#include "Bullet.h"
#include "Caret.h"
#include "Game.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"

void Vanish(BULLET *bul)
{
	if (bul->code_bullet != 37 && bul->code_bullet != 38 && bul->code_bullet != 39)
		PlaySoundObject(28, 1);
	else
		SetCaret(bul->x, bul->y, 2, 1);
	bul->cond = 0;
	SetCaret(bul->x, bul->y, 2, 2);
}

int JudgeHitBulletBlock(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x - bul->blockXL < (2 * x + 1) << 12
		&& bul->blockXL + bul->x > (2 * x - 1) << 12
		&& bul->y - bul->blockYL < (2 * y + 1) << 12
		&& bul->blockYL + bul->y > (2 * y - 1) << 12)
		hit = 0x200;
	
	if (hit && (bul->bbits & 0x60) && GetAttribute(x, y) == 0x43)
	{
		if (!(bul->bbits & 0x40))
			bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
		PlaySoundObject(12, 1);
		for (int i = 0; i < 4; i++)
			SetNpChar(4, x << 13, y << 13, Random(-0x200, 0x200), Random(-0x200, 0x200), 0, 0, 256);
		ShiftMapParts(x, y);
	}
	
	return hit;
}

int JudgeHitBulletBlock2(int x, int y, uint8_t *atrb, BULLET *bul)
{
	int hit = 0;
	
	int block[4];
	if (bul->bbits & 0x40)
	{
		for (int i = 0; i < 4; i++)
		{
			block[i] = *atrb == 0x41 || *atrb == 0x61;
			++atrb;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			block[i] = *atrb == 0x41 || *atrb == 0x43 || *atrb == 0x61;
			++atrb;
		}
	}
	
	int workX = (2 * x + 1) << 12;
	int workY = (2 * y + 1) << 12;
	
	//Left wall
	if (block[0] && block[2])
	{
		if (bul->x - bul->blockXL < workX)
			hit |= 1;
	}
	else if (!block[0] || block[2])
	{
		if (!block[0] && block[2] && bul->x - bul->blockXL < workX && bul->blockYL + bul->y > workY + 0x600)
			hit |= 1;
	}
	else if (bul->x - bul->blockXL < workX && bul->y - bul->blockYL < workY - 0x600)
	{
		hit |= 1;
	}
	
	//Right wall
	if (block[1] && block[3])
	{
		if (bul->x + bul->blockXL > workX)
			hit |= 4;
	}
	else if (!block[1] || block[3])
	{
		if (!block[1] && block[3] && bul->x + bul->blockXL > workX && bul->blockYL + bul->y > workY + 0x600)
			hit |= 4;
	}
	else if (bul->x + bul->blockXL > workX && bul->y - bul->blockYL < workY - 0x600)
	{
		hit |= 4;
	}
	
	//Ceiling
	if (block[0] && block[1])
	{
		if (bul->y - bul->blockYL < workY)
			hit |= 2;
	}
	else if (!block[0] || block[1])
	{
		if (!block[0] && block[1] && bul->y - bul->blockYL < workY && bul->blockXL + bul->x > workX + 0x600)
			hit |= 2;
	}
	else if (bul->y - bul->blockYL < workY && bul->x - bul->blockXL < workX - 0x600)
	{
		hit |= 2;
	}
	
	//Ground
	if (block[2] && block[3])
	{
		if (bul->y + bul->blockYL > workY)
			hit |= 8;
	}
	else if (!block[2] || block[3])
	{
		if (!block[2] && block[3] && bul->y + bul->blockYL > workY && bul->blockXL + bul->x > workX + 0x600)
			hit |= 8;
	}
	else if (bul->y + bul->blockYL > workY && bul->x - bul->blockXL < workX - 0x600)
	{
		hit |= 8;
	}
	
	//Clip
	if (bul->bbits & 8)
	{
		if (hit & 1)
			bul->x = workX + bul->blockXL;
		else if (hit & 4)
			bul->x = workX - bul->blockXL;
		else if (hit & 2)
			bul->y = workY + bul->blockYL;
		else if (hit & 8)
			bul->y = workY - bul->blockYL;
	}
	else if (hit & 0xF)
	{
		Vanish(bul);
	}
	
	return hit;
}

int JudgeHitBulletTriangleA(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 + 0xC00;
		else
			Vanish(bul);
		hit = 0x82;
	}
	return hit;
}

int JudgeHitBulletTriangleB(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x400;
		else
			Vanish(bul);
		hit = 0x82;
	}
	return hit;
}

int JudgeHitBulletTriangleC(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) + (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 - 0x400;
		else
			Vanish(bul);
		hit = 0x82;
	}
	return hit;
}

int JudgeHitBulletTriangleD(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y - 0x400 < (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y + 0x400 > (2 * y - 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 + 0xC00;
		else
			Vanish(bul);
		hit = 0x82;
	}
	return hit;
}

int JudgeHitBulletTriangleE(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x - 0x200 > (2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) + (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12 )
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 - 0xC00;
		else
			Vanish(bul);
		hit = 0x28;
	}
	return hit;
}

int JudgeHitBulletTriangleF(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) + (-0x2000 * x + bul->x) / 2 + 0x400;
		else
			Vanish(bul);
		hit = 0x28;
	}
	return hit;
}

int JudgeHitBulletTriangleG(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12)
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 + 0x400;
		else
			Vanish(bul);
		hit = 0x18;
	}
	return hit;
}

int JudgeHitBulletTriangleH(int x, int y, BULLET *bul)
{
	int hit = 0;
	if (bul->x < (2 * x + 1) << 12
		&& bul->x > (2 * x - 1) << 12
		&& bul->y + 0x400 > (y << 13) - (-0x2000 * x + bul->x) / 2 - 0x800
		&& bul->y - 0x400 < (2 * y + 1) << 12 )
	{
		if (bul->bbits & 8)
			bul->y = (y << 13) - (-0x2000 * x + bul->x) / 2 - 0xC00;
		else
			Vanish(bul);
		hit = 0x18;
	}
	return hit;
}

void HitBulletMap()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80)
		{
			int x = gBul[i].x / 0x2000;
			int y = gBul[i].y / 0x2000;
			
			//Get surrounding tiles
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
			atrb[0] = GetAttribute(x, y);
			atrb[1] = GetAttribute(x + 1, y);
			atrb[2] = GetAttribute(x, y + 1);
			atrb[3] = GetAttribute(x + 1, y + 1);
			
			//Clear hit tiles
			gBul[i].flag = 0;
			
			if (!(gBul[i].bbits & 4))
			{
				for (int j = 0; j < 4; j++)
				{
					if (gBul[i].cond & 0x80)
					{
						switch (atrb[j])
						{
							case 0x41:
							case 0x43:
							case 0x44:
							case 0x61:
							case 0x64:
								gBul[i].flag |= JudgeHitBulletBlock(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x50:
							case 0x70:
								gBul[i].flag |= JudgeHitBulletTriangleA(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x51:
							case 0x71:
								gBul[i].flag |= JudgeHitBulletTriangleB(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x52:
							case 0x72:
								gBul[i].flag |= JudgeHitBulletTriangleC(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x53:
							case 0x73:
								gBul[i].flag |= JudgeHitBulletTriangleD(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x54:
							case 0x74:
								gBul[i].flag |= JudgeHitBulletTriangleE(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x55:
							case 0x75:
								gBul[i].flag |= JudgeHitBulletTriangleF(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x56:
							case 0x76:
								gBul[i].flag |= JudgeHitBulletTriangleG(x + offx[j], y + offy[j], &gBul[i]);
								break;
							case 0x57:
							case 0x77:
								gBul[i].flag |= JudgeHitBulletTriangleH(x + offx[j], y + offy[j], &gBul[i]);
								break;
							default:
								break;
						}
					}
				}
				
				gBul[i].flag |= JudgeHitBulletBlock2(x, y, atrb, &gBul[i]);
			}
		}
	}
}
