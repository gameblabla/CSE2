#include "Bullet.h"
#include "Draw.h"
#include "Caret.h"
#include "NpChar.h"
#include "MyChar.h"
#include "Sound.h"
#include "Game.h"

BULLET_TABLE gBulTbl[46] =
{
	{0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}},
	{4, 1, 20, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
	{6, 1, 23, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
	{8, 1, 30, 36, 4, 4, 2, 2, {8, 8, 8, 8}},
	{1, 1, 8, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
	{2, 1, 12, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
	{4, 1, 16, 32, 6, 6, 2, 2, {8, 8, 8, 8}},
	{2, 2, 100, 8, 8, 16, 4, 2, {8, 8, 8, 8}},
	{3, 2, 100, 8, 4, 4, 4, 2, {8, 8, 8, 8}},
	{3, 2, 100, 8, 4, 4, 4, 2, {8, 8, 8, 8}},
	{2, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
	{4, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
	{6, 1, 20, 32, 2, 2, 2, 2, {8, 8, 8, 8}},
	{0, 10, 50, 40, 2, 2, 2, 2, {8, 8, 8, 8}},
	{0, 10, 70, 40, 4, 4, 4, 4, {8, 8, 8, 8}},
	{0, 10, 90, 40, 4, 4, 0, 0, {8, 8, 8, 8}},
	{1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
	{1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
	{1, 100, 100, 20, 16, 16, 0, 0, {0, 0, 0, 0}},
	{1, 1, 20, 8, 2, 2, 2, 2, {4, 4, 4, 4}},
	{2, 1, 20, 8, 2, 2, 2, 2, {4, 4, 4, 4}},
	{2, 1, 20, 8, 4, 4, 4, 4, {4, 4, 4, 4}},
	{3, 1, 32, 32, 2, 2, 2, 2, {4, 4, 4, 4}},
	{0, 100, 0, 36, 8, 8, 8, 8, {12, 12, 12, 12}},
	{127, 1, 2, 4, 8, 4, 8, 4, {0, 0, 0, 0}},
	{15, 1, 30, 36, 8, 8, 4, 2, {8, 8, 8, 8}},
	{6, 3, 18, 36, 10, 10, 4, 2, {12, 12, 12, 12}},
	{1, 100, 30, 36, 6, 6, 4, 4, {12, 12, 12, 12}},
	{0, 10, 30, 40, 2, 2, 2, 2, {8, 8, 8, 8}},
	{0, 10, 40, 40, 4, 4, 4, 4, {8, 8, 8, 8}},
	{0, 10, 40, 40, 4, 4, 0, 0, {8, 8, 8, 8}},
	{2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
	{2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
	{2, 100, 100, 20, 12, 12, 0, 0, {0, 0, 0, 0}},
	{4, 4, 20, 32, 4, 4, 3, 3, {8, 8, 24, 8}},
	{4, 2, 20, 32, 2, 2, 2, 2, {8, 8, 24, 8}},
	{1, 1, 20, 32, 2, 2, 2, 2, {8, 8, 24, 8}},
	{4, 4, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
	{8, 8, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
	{12, 12, 30, 64, 6, 6, 3, 3, {8, 8, 8, 8}},
	{3, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
	{6, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
	{11, 100, 30, 32, 6, 6, 3, 3, {4, 4, 4, 4}},
	{4, 4, 20, 32, 4, 4, 3, 3, {8, 8, 24, 8}},
	{0, 4, 4, 4, 0, 0, 0, 0, {0, 0, 0, 0}},
	{1, 1, 1, 36, 1, 1, 1, 1, {1, 1, 1, 1}}
};

BULLET gBul[BULLET_MAX];

void InitBullet()
{
	for (int i = 0; i < BULLET_MAX; i++)
		gBul[i].cond = 0;
}

int CountArmsBullet(int arms_code)
{
	int count = 0;
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80 && (gBul[i].code_bullet + 2) / 3 == arms_code)
			++count;
	}
	
	return count;
}

int CountBulletNum(int bullet_code)
{
	int count = 0;
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80 && gBul[i].code_bullet == bullet_code)
			++count;
	}
	
	return count;
}

void DeleteBullet(int code)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80)
		{
			if ((gBul[i].code_bullet + 2) / 3 == code)
				gBul[i].cond = 0;
		}
	}
}

void ClearBullet()
{
	for (int i = 0; i < BULLET_MAX; i++)
		gBul[i].cond = 0;
}

void PutBullet(int fx, int fy)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80)
		{
			int x, y;
			
			switch (gBul[i].direct)
			{
				case 0:
					x = gBul[i].x - gBul[i].view.front;
					y = gBul[i].y - gBul[i].view.top;
					break;
				case 1:
					x = gBul[i].x - gBul[i].view.top;
					y = gBul[i].y - gBul[i].view.front;
					break;
				case 2:
					x = gBul[i].x - gBul[i].view.back;
					y = gBul[i].y - gBul[i].view.top;
					break;
				case 3:
					x = gBul[i].x - gBul[i].view.top;
					y = gBul[i].y - gBul[i].view.back;
					break;
			}
			
			PutBitmap3(&grcGame, x / 0x200 - fx / 0x200, y / 0x200 - fy / 0x200, &gBul[i].rect, 17);
		}
	}
}

void SetBullet(int no, int x, int y, int dir)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (!(gBul[i].cond & 0x80))
		{
			memset(&gBul[i], 0, sizeof(BULLET));
			gBul[i].code_bullet = no;
			gBul[i].cond = 0x80;
			gBul[i].direct = dir;
			gBul[i].damage = gBulTbl[no].damage;
			gBul[i].life = gBulTbl[no].life;
			gBul[i].life_count = gBulTbl[no].life_count;
			gBul[i].bbits = gBulTbl[no].bbits;
			gBul[i].enemyXL = gBulTbl[no].enemyXL << 9;
			gBul[i].enemyYL = gBulTbl[no].enemyYL << 9;
			gBul[i].blockXL = gBulTbl[no].blockXL << 9;
			gBul[i].blockYL = gBulTbl[no].blockYL << 9;
			gBul[i].view.back = gBulTbl[no].view.back << 9;
			gBul[i].view.front = gBulTbl[no].view.front << 9;
			gBul[i].view.top = gBulTbl[no].view.top << 9;
			gBul[i].view.bottom = gBulTbl[no].view.bottom << 9;
			gBul[i].x = x;
			gBul[i].y = y;
			break;
		}
	}
}

void ActBullet_PoleStar(BULLET *bul, int level)
{
	if (++bul->count1 <= bul->life_count)
	{
		if (bul->act_no)
		{
			//Move
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;
			
			//Set speed
			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x1000;
					break;
				case 1:
					bul->ym = -0x1000;
					break;
				case 2:
					bul->xm = 0x1000;
					break;
				case 3:
					bul->ym = 0x1000;
					break;
			}
			
			//Set hitbox
			if (level == 1)
			{
				switch (bul->direct)
				{
					case 0:
						bul->enemyYL = 0x400;
						break;
					case 1:
						bul->enemyXL = 0x400;
						break;
					case 2:
						bul->enemyYL = 0x400;
						break;
					case 3:
						bul->enemyXL = 0x400;
						break;
				}
			}
			else if (level == 2)
			{
				switch (bul->direct)
				{
					case 0:
						bul->enemyYL = 0x800;
						break;
					case 1:
						bul->enemyXL = 0x800;
						break;
					case 2:
						bul->enemyYL = 0x800;
						break;
					case 3:
						bul->enemyXL = 0x800;
						break;
				}
			}
			
			//Set framerect
			switch (level)
			{
				case 1:
					if (bul->direct != 1 && bul->direct != 3)
						bul->rect = {128, 32, 144, 48};
					else
						bul->rect = {144, 32, 160, 48};
					break;
				case 2:
					if (bul->direct != 1 && bul->direct != 3)
						bul->rect = {160, 32, 176, 48};
					else
						bul->rect = {176, 32, 192, 48};
					break;
				case 3:
					if (bul->direct != 1 && bul->direct != 3)
						bul->rect = {128, 48, 144, 64};
					else
						bul->rect = {144, 48, 160, 64};
					break;
			}
		}
	}
	else
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
}

void ActBullet_FireBall(BULLET *bul, int level)
{
	if (++bul->count1 <= bul->life_count)
	{
		bool bBreak = false;
		if (bul->flag & 2 && bul->flag & 8)
			bBreak = true;
		if (bul->flag & 1 && bul->flag & 4)
			bBreak = true;
	
		if (!bul->direct && bul->flag & 1)
			bul->direct = 2;
		if (bul->direct == 2 && bul->flag & 4)
			bul->direct = 0;
		
		if (bBreak)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 2, 0);
			PlaySoundObject(28, 1);
		}
		else
		{
			if (bul->act_no)
			{
				if (bul->flag & 8)
					bul->ym = -0x400;
				else if (bul->flag & 1)
					bul->xm = 0x400;
				else if (bul->flag & 4)
					bul->xm = -0x400;
				
				bul->ym += 85;
				if (bul->ym >= 0x400)
					bul->ym = 0x400;
				
				bul->x += bul->xm;
				bul->y += bul->ym;
				
				if (bul->flag & 0xD)
					PlaySoundObject(34, 1);
			}
			else
			{
				bul->act_no = 1;
				
				switch (bul->direct)
				{
					case 0:
						bul->xm = -0x400;
						break;
					case 1:
						bul->xm = gMC.xm;
						
						if (gMC.xm >= 0)
							bul->direct = 2;
						else
							bul->direct = 0;
						
						if (gMC.direct)
							bul->xm += 0x80;
						else
							bul->xm -= 0x80;
						
						bul->ym = -0x5FF;
						break;
					case 2:
						bul->xm = 0x400;
						break;
					case 3:
						bul->xm = gMC.xm;
						if (gMC.xm >= 0)
							bul->direct = 2;
						else
							bul->direct = 0;
						bul->ym = 0x5FF;
						break;
				}
			}

			RECT rect_left1[4];
			RECT rect_right1[4];
			RECT rect_left2[3];
			RECT rect_right2[3];
			rect_left1[0] = {128, 0, 144, 16};
			rect_left1[1] = {144, 0, 160, 16};
			rect_left1[2] = {160, 0, 176, 16};
			rect_left1[3] = {176, 0, 192, 16};
			rect_right1[0] = {128, 16, 144, 32};
			rect_right1[1] = {144, 16, 160, 32};
			rect_right1[2] = {160, 16, 176, 32};
			rect_right1[3] = {176, 16, 192, 32};
			rect_left2[0] = {192, 16, 208, 32};
			rect_left2[1] = {208, 16, 224, 32};
			rect_left2[2] = {224, 16, 240, 32};
			rect_right2[0] = {224, 16, 240, 32};
			rect_right2[1] = {208, 16, 224, 32};
			rect_right2[2] = {192, 16, 208, 32};
			
			bul->ani_no++;
			
			if (level == 1)
			{
				if (bul->ani_no > 3)
					bul->ani_no = 0;
				
				if (bul->direct)
					bul->rect = rect_right1[bul->ani_no];
				else
					bul->rect = rect_left1[bul->ani_no];
			}
			else
			{
				if (bul->ani_no > 2)
					bul->ani_no = 0;
				
				if (bul->direct)
					bul->rect = rect_right2[bul->ani_no];
				else
					bul->rect = rect_left2[bul->ani_no];
				
				if (level == 2)
					SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no, 0, 0x100);
				else
					SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no + 3, 0, 0x100);
			}
		}
	}
	else
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
}

void ActBullet_MachineGun(BULLET *bul, int level)
{
	RECT rect1[4];
	RECT rect2[4];
	RECT rect3[4];
	
	rect1[0] = {64, 0, 80, 16};
	rect1[1] = {80, 0, 96, 16};
	rect1[2] = {96, 0, 112, 16};
	rect1[3] = {112, 0, 128, 16};
	rect2[0] = {64, 16, 80, 32};
	rect2[1] = {80, 16, 96, 32};
	rect2[2] = {96, 16, 112, 32};
	rect2[3] = {112, 16, 128, 32};
	rect3[0] = {64, 32, 80, 48};
	rect3[1] = {80, 32, 96, 48};
	rect3[2] = {96, 32, 112, 48};
	rect3[3] = {112, 32, 128, 48};
	
	if (++bul->count1 <= bul->life_count)
	{
		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
			
			switch ( level )
			{
				case 1:
					bul->rect = rect1[bul->direct];
					break;
				case 2:
					bul->rect = rect2[bul->direct];
					if (bul->direct != 1 && bul->direct != 3)
						SetNpChar(127, bul->x, bul->y, 0, 0, 0, 0, 256);
					else
						SetNpChar(127, bul->x, bul->y, 0, 0, 1, 0, 256);
					break;
				case 3:
					bul->rect = rect3[bul->direct];
					SetNpChar(128, bul->x, bul->y, 0, 0, bul->direct, 0, 256);
					break;
			}
		}
		else
		{
			int move;
			switch (level)
			{
				case 1:
					move = 0x1000;
					break;
				case 2:
					move = 0x1000;
					break;
				case 3:
					move = 0x1000;
					break;
			}
			
			bul->act_no = 1;
			
			switch (bul->direct)
			{
				case 0:
					bul->xm = -move;
					bul->ym = Random(-0xAA, 0xAA);
					break;
				case 1:
					bul->xm = Random(-0xAA, 0xAA);
					bul->ym = -move;
					break;
				case 2:
					bul->xm = move;
					bul->ym = Random(-0xAA, 0xAA);
					break;
				case 3:
					bul->xm = Random(-0xAA, 0xAA);
					bul->ym = move;
					break;
			}
		}
	}
	else
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
}

void ActBullet_Missile(BULLET *bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
		return;
	}

	bool bHit = false;

	if (bul->life != 10)
		bHit = true;
	if (bul->direct == 0 && bul->flag & 1)
		bHit = true;
	if (bul->direct == 2 && bul->flag & 4)
		bHit = true;
	if (bul->direct == 1 && bul->flag & 2)
		bHit = true;
	if (bul->direct == 3 && bul->flag & 8)
		bHit = true;
	if (bul->direct == 0 && bul->flag & 0x80)
		bHit = true;
	if (bul->direct == 0 && bul->flag & 0x20)
		bHit = true;
	if (bul->direct == 2 && bul->flag & 0x40)
		bHit = true;
	if (bul->direct == 2 && bul->flag & 0x10)
		bHit = true;

	if (bHit)
	{
		SetBullet(level + 15, bul->x, bul->y, 0);
		bul->cond = 0;
	}

	switch (bul->act_no)
	{
		case 0:
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
				case 2:
					bul->tgt_y = bul->y;
					break;
				case 1:
				case 3:
					bul->tgt_x = bul->x;
					break;
			}

			if (level == 3)
			{
				switch (bul->direct)
				{
					case 0:
					case 2:
						if (gMC.y < bul->y)
							bul->ym = 0x100;
						else
							bul->ym = -0x100;

						bul->xm = Random(-0x200, 0x200);
						break;

					case 1:
					case 3:
						if (gMC.x < bul->x)
							bul->xm = 0x100;
						else
							bul->xm = -0x100;

						bul->ym = Random(-0x200, 0x200);
						break;
				}

				unsigned int inc;
				
				switch (++inc % 3u)
				{
					case 0:
						bul->ani_no = 0x80;
						break;
					case 1:
						bul->ani_no = 0x40;
						break;
					case 2:
						bul->ani_no = 0x33;
						break;
				}
			}
			else
			{
				bul->ani_no = 0x80;
			}
			// Fallthrough
		case 1:
			switch (bul->direct)
			{
				case 0:
					bul->xm -= bul->ani_no;
					break;
				case 1:
					bul->ym -= bul->ani_no;
					break;
				case 2:
					bul->xm += bul->ani_no;
					break;
				case 3:
					bul->ym += bul->ani_no;
					break;
			}

			if (level == 3)
			{
				switch (bul->direct)
				{
					case 0:
					case 2:
						if (bul->tgt_y > bul->y)
							bul->ym += 0x20;
						else
							bul->ym -= 0x20;

						break;

					case 1:
					case 3:
						if (bul->tgt_x > bul->x)
							bul->xm += 0x20;
						else
							bul->xm -= 0x20;
						break;
				}
			}

			if (bul->xm < -0xA00)
				bul->xm = -0xA00;
			if (bul->xm > 0xA00)
				bul->xm = 0xA00;

			if (bul->ym < -0xA00)
				bul->ym = -0xA00;
			if (bul->ym > 0xA00)
				bul->ym = 0xA00;

			bul->x += bul->xm;
			bul->y += bul->ym;

			break;
	}

	if (++bul->count2 > 2)
	{
		bul->count2 = 0;

		switch (bul->direct)
		{
			case 0:
				SetCaret(bul->x + 0x1000, bul->y, 7, 2);
				break;
			case 1:
				SetCaret(bul->x, bul->y + 0x1000, 7, 3);
				break;
			case 2:
				SetCaret(bul->x - 0x1000, bul->y, 7, 0);
				break;
			case 3:
				SetCaret(bul->x, bul->y - 0x1000, 7, 1);
				break;
		}
	}

	RECT rect1[4];
	RECT rect2[4];
	RECT rect3[4];

	rect1[0] = {0, 0, 16, 16};
	rect1[1] = {16, 0, 32, 16};
	rect1[2] = {32, 0, 48, 16};
	rect1[3] = {48, 0, 64, 16};

	rect2[0] = {0, 16, 16, 32};
	rect2[1] = {16, 16, 32, 32};
	rect2[2] = {32, 16, 48, 32};
	rect2[3] = {48, 16, 64, 32};

	rect3[0] = {0, 32, 16, 48};
	rect3[1] = {16, 32, 32, 48};
	rect3[2] = {32, 32, 48, 48};
	rect3[3] = {48, 32, 64, 48};

	switch (level)
	{
		case 1:
			bul->rect = rect1[bul->direct];
			break;
		case 2:
			bul->rect = rect2[bul->direct];
			break;
		case 3:
			bul->rect = rect3[bul->direct];
			break;
	}
}

void ActBullet_Bom(BULLET *bul, int level)
{
	switch (bul->act_no)
	{
		case 0:
			bul->act_no = 1;
			
			switch ( level )
			{
				case 2:
					bul->act_wait = 15;
					break;
				case 3:
					bul->act_wait = 5;
					break;
				case 1:
					bul->act_wait = 10;
					break;
			}
			
			PlaySoundObject(44, 1);
			
		case 1:
			if (level == 1)
			{
				if (!(bul->act_wait % 3))
					SetDestroyNpCharUp(bul->x + (Random(-16, 16) << 9), bul->y + (Random(-16, 16) << 9), bul->enemyXL, 2);
			}
			else if (level == 2)
			{
				if (!(bul->act_wait % 3))
					SetDestroyNpCharUp(bul->x + (Random(-32, 32) << 9), bul->y + (Random(-32, 32) << 9), bul->enemyXL, 2);
			}
			else if (level == 3)
			{
				if (!(bul->act_wait % 3))
					SetDestroyNpCharUp(bul->x + (Random(-40, 40) << 9), bul->y + (Random(-40, 40) << 9), bul->enemyXL, 2);
			}
			
			if (--bul->act_wait < 0)
				bul->cond = 0;
			break;
	}
	
}

void ActBullet()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (gBul[i].cond & 0x80)
		{
			if (gBul[i].life > 0)
			{
				switch (gBul[i].code_bullet)
				{
					case 4:
						ActBullet_PoleStar(&gBul[i], 1);
						break;
					case 5:
						ActBullet_PoleStar(&gBul[i], 2);
						break;
					case 6:
						ActBullet_PoleStar(&gBul[i], 3);
						break;
					case 7:
						ActBullet_FireBall(&gBul[i], 1);
						break;
					case 8:
						ActBullet_FireBall(&gBul[i], 2);
						break;
					case 9:
						ActBullet_FireBall(&gBul[i], 3);
						break;
					case 10:
						ActBullet_MachineGun(&gBul[i], 1);
						break;
					case 11:
						ActBullet_MachineGun(&gBul[i], 2);
						break;
					case 12:
						ActBullet_MachineGun(&gBul[i], 3);
						break;
					case 13:
						ActBullet_Missile(&gBul[i], 1);
						break;
					case 14:
						ActBullet_Missile(&gBul[i], 2);
						break;
					case 15:
						ActBullet_Missile(&gBul[i], 3);
						break;
					case 16:
						ActBullet_Bom(&gBul[i], 1);
						break;
					case 17:
						ActBullet_Bom(&gBul[i], 2);
						break;
					case 18:
						ActBullet_Bom(&gBul[i], 3);
						break;
				}
			}
			else
			{
				gBul[i].cond = 0;
			}
		}
	}
}
