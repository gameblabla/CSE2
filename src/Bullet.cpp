#include "Bullet.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "Draw.h"
#include "Caret.h"
#include "Game.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"

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
			
			PutBitmap3(&grcGame, x / 0x200 - fx / 0x200, y / 0x200 - fy / 0x200, &gBul[i].rect, SURFACE_ID_BULLET);
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

void ActBullet_Frontia1(BULLET *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->ani_no = Random(0, 2);
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x600;
					break;
				case 1:
					bul->ym = -0x600;
					break;
				case 2:
					bul->xm = 0x600;
					break;
				case 3:
					bul->ym = 0x600;
					break;
			}
		}

		if (++bul->ani_wait > 0)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 0;

		RECT rcLeft[4] = {
			{136, 80, 152, 80},
			{120, 80, 136, 96},
			{136, 64, 152, 80},
			{120, 64, 136, 80},
		};

		RECT rcRight[4] = {
			{120, 64, 136, 80},
			{136, 64, 152, 80},
			{120, 80, 136, 96},
			{136, 80, 152, 80},
		};

		if (bul->direct == 0)
			bul->rect = rcLeft[bul->ani_no];
		else
			bul->rect = rcRight[bul->ani_no];
	}
}

void ActBullet_Frontia2(BULLET *bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no)
		{
			switch (bul->direct)
			{
				case 0:
					bul->xm -= 0x80;
					break;
				case 1:
					bul->ym -= 0x80;
					break;
				case 2:
					bul->xm += 0x80;
					break;
				case 3:
					bul->ym += 0x80;
					break;
			}

			switch (bul->direct)
			{
				case 0:
				case 2:
					if (bul->count1 % 5 == 2)
					{
						if (bul->ym < 0)
							bul->ym = 0x400;
						else
							bul->ym = -0x400;
					}

					break;

				case 1u:
				case 3u:
					if (bul->count1 % 5 == 2)
					{
						if (bul->xm < 0)
							bul->xm = 0x400;
						else
							bul->xm = -0x400;
					}

					break;
			}

			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->ani_no = Random(0, 2);
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x200;
					break;
				case 1:
					bul->ym = -0x200;
					break;
				case 2:
					bul->xm = 0x200;
					break;
				case 3:
					bul->ym = 0x200;
					break;
			}

			static unsigned int inc;
			++inc;

			switch (bul->direct)
			{
				case 0:
				case 2:
					if (inc % 2)
						bul->ym = 0x400;
					else
						bul->ym = -0x400;

					break;

				case 1:
				case 3:
					if (inc % 2)
						bul->xm = -0x400;
					else
						bul->xm = 0x400;

					break;
			}
		}

		if ( ++bul->ani_wait > 0 )
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if ( bul->ani_no > 2 )
			bul->ani_no = 0;

		RECT rect[3] = {
			{192, 16, 208, 32},
			{208, 16, 224, 32},
			{224, 16, 240, 32},
		};

		bul->rect = rect[bul->ani_no];

		if (level == 2)
			SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no, 0, 0x100);
		else
			SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no + 3, 0, 0x100);
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
					{
						bul->rect.left = 128;
						bul->rect.top = 32;
						bul->rect.right = 144;
						bul->rect.bottom = 48;
					}
					else
					{
						bul->rect.left = 144;
						bul->rect.top = 32;
						bul->rect.right = 160;
						bul->rect.bottom = 48;
					}
					break;
				case 2:
					if (bul->direct != 1 && bul->direct != 3)
					{
						bul->rect.left = 160;
						bul->rect.top = 32;
						bul->rect.right = 176;
						bul->rect.bottom = 48;
					}
					else
					{
						bul->rect.left = 176;
						bul->rect.top = 32;
						bul->rect.right = 192;
						bul->rect.bottom = 48;

					}
					break;
				case 3:
					if (bul->direct != 1 && bul->direct != 3)
					{
						bul->rect.left = 128;
						bul->rect.top = 48;
						bul->rect.right = 144;
						bul->rect.bottom = 64;
					}
					else
					{
						bul->rect.left = 144;
						bul->rect.top = 48;
						bul->rect.right = 160;
						bul->rect.bottom = 64;
					}
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

			RECT rect_left1[4] = {
				{128, 0, 144, 16},
				{144, 0, 160, 16},
				{160, 0, 176, 16},
				{176, 0, 192, 16},
			};

			RECT rect_right1[4] = {
				{128, 16, 144, 32},
				{144, 16, 160, 32},
				{160, 16, 176, 32},
				{176, 16, 192, 32},
			};

			RECT rect_left2[3] = {
				{192, 16, 208, 32},
				{208, 16, 224, 32},
				{224, 16, 240, 32},
			};

			RECT rect_right2[3] = {
				{224, 16, 240, 32},
				{208, 16, 224, 32},
				{192, 16, 208, 32},
			};
			
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
	RECT rect1[4] = {
		{64, 0, 80, 16},
		{80, 0, 96, 16},
		{96, 0, 112, 16},
		{112, 0, 128, 16},
	};

	RECT rect2[4] = {
		{64, 16, 80, 32},
		{80, 16, 96, 32},
		{96, 16, 112, 32},
		{112, 16, 128, 32},
	};

	RECT rect3[4] = {
		{64, 32, 80, 48},
		{80, 32, 96, 48},
		{96, 32, 112, 48},
		{112, 32, 128, 48},
	};

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
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

				static unsigned int inc;
				
				switch (++inc % 3)
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

	RECT rect1[4] = {
		{0, 0, 16, 16},
		{16, 0, 32, 16},
		{32, 0, 48, 16},
		{48, 0, 64, 16},
	};

	RECT rect2[4] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{32, 16, 48, 32},
		{48, 16, 64, 32},
	};

	RECT rect3[4] = {
		{0, 32, 16, 48},
		{16, 32, 32, 48},
		{32, 32, 48, 48},
		{48, 32, 64, 48},
	};

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
			// Fallthrough
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

	if (level == 1)
	{
		 if (bul->act_wait % 3 == 0)
			SetDestroyNpCharUp(bul->x + (Random(-16, 16) * 0x200), bul->y + (Random(-16, 16) * 0x200), bul->enemyXL, 2);
	}
	else if (level == 2)
	{
		if (bul->act_wait % 3 == 0)
			SetDestroyNpCharUp(bul->x + (Random(-32, 32) * 0x200), bul->y + (Random(-32, 32) * 0x200), bul->enemyXL, 2);
	}
	else if (level == 3)
	{
		if (bul->act_wait % 3 == 0)
			SetDestroyNpCharUp(bul->x + (Random(-40, 40) * 0x200), bul->y + (Random(-40, 40) * 0x200), bul->enemyXL, 2);
	}

	if (--bul->act_wait < 0)
		bul->cond = 0;
}

void ActBullet_Bubblin1(BULLET *bul)
{
	if (bul->flag & 0x2FF)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x600;
					break;
				case 2:
					bul->xm = 0x600;
					break;
				case 1:
					bul->ym = -0x600;
					break;
				case 3:
					bul->ym = 0x600;
					break;
			}
		}

		switch (bul->direct)
		{
			case 0:
				bul->xm += 42;
				break;
			case 2:
				bul->xm -= 42;
				break;
			case 1:
				bul->ym += 42;
				break;
			case 3:
				bul->ym -= 42;
				break;
		}

		bul->x += bul->xm;
		bul->y += bul->ym;

		if (++bul->act_wait > 40)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 15, 0);
		}

		RECT rect[4] = {
			{192, 0, 200, 8},
			{200, 0, 208, 8},
			{208, 0, 216, 8},
			{216, 0, 224, 8},
		};

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
}

void ActBullet_Bubblin2(BULLET *bul)
{
	bool bDelete = false;

	if (bul->direct == 0 && bul->flag & 1)
		bDelete = true;
	if (bul->direct == 2 && bul->flag & 4)
		bDelete = true;
	if (bul->direct == 1 && bul->flag & 2)
		bDelete = true;
	if (bul->direct == 3 && bul->flag & 8)
		bDelete = true;

	if (bDelete)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x600;
					bul->ym = Random(-0x100, 0x100);
					break;
				case 2:
					bul->xm = 0x600;
					bul->ym = Random(-0x100, 0x100);
					break;
				case 1:
					bul->ym = -0x600;
					bul->xm = Random(-0x100, 0x100);
					break;
				case 3:
					bul->ym = 0x600;
					bul->xm = Random(-0x100, 0x100);
					break;
			}
		}

		switch (bul->direct)
		{
			case 0:
				bul->xm += 0x10;
				break;
			case 2:
				bul->xm -= 0x10;
				break;
			case 1:
				bul->ym += 0x10;
				break;
			case 3:
				bul->ym -= 0x10;
				break;
		}

		bul->x += bul->xm;
		bul->y += bul->ym;

		if (++bul->act_wait > 60)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 15, 0);
		}

		RECT rect[4] = {
			{192, 8, 200, 16},
			{200, 8, 208, 16},
			{208, 8, 216, 16},
			{216, 8, 224, 16},
		};

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
}

void ActBullet_Bubblin3(BULLET *bul)
{
	if (++bul->act_wait <= 100 && gKey & gKeyShot)
	{
		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = Random(-0x400, -0x200);
					bul->ym = (Random(-4, 4) * 0x200) / 2;
					break;
				case 2u:
					bul->xm = Random(0x200, 0x400);
					bul->ym = (Random(-4, 4) * 0x200) / 2;
					break;
				case 1u:
					bul->ym = Random(-0x400, -0x200);
					bul->xm = (Random(-4, 4) * 0x200) / 2;
					break;
				case 3u:
					bul->ym = Random(0x80, 0x100);
					bul->xm = (Random(-4, 4) * 0x200) / 2;
					break;
			}
		}

		if (gMC.x > bul->x)
			bul->xm += 0x20;
		if (gMC.x < bul->x)
			bul->xm -= 0x20;

		if (gMC.y > bul->y)
			bul->ym += 0x20;
		if (gMC.y < bul->y)
			bul->ym -= 0x20;

		if (bul->xm < 0 && bul->flag & 1)
			bul->xm = 0x400;
		if (bul->xm > 0 && bul->flag & 4)
			bul->xm = -0x400;

		if (bul->ym < 0 && bul->flag & 2)
			bul->ym = 0x400;
		if (bul->ym > 0 && bul->flag & 8)
			bul->ym = -0x400;

		bul->x += bul->xm;
		bul->y += bul->ym;

		RECT rect[4] = {
			{240, 16, 248, 24},
			{248, 16, 256, 24},
			{240, 24, 248, 32},
			{248, 24, 256, 32},
		};

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
	else
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
		PlaySoundObject(100, 1);

		if (gMC.up)
			SetBullet(22, bul->x, bul->y, 1);
		else if (gMC.down)
			SetBullet(22, bul->x, bul->y, 3);
		else
			SetBullet(22, bul->x, bul->y, gMC.direct);
	}
}

void ActBullet_Spine(BULLET *bul)
{
	if (++bul->count1 > bul->life_count || bul->flag & 8)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = (-0x200 * Random(10, 16)) / 2;
					break;
				case 1:
					bul->ym = (-0x200 * Random(10, 16)) / 2;
					break;
				case 2:
					bul->xm = (Random(10, 16) * 0x200) / 2;
					break;
				case 3:
					bul->ym = (Random(10, 16) * 0x200) / 2;
					break;
			}
		}

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 1)
			bul->ani_no = 0;

		RECT rcLeft[2] = {
			{224, 0, 232, 8},
			{232, 0, 240, 8},
		};

		RECT rcRight[2] = {
			{224, 0, 232, 8},
			{232, 0, 240, 8},
		};

		RECT rcDown[2] = {
			{224, 8, 232, 16},
			{232, 8, 240, 16},
		};

		switch (bul->direct)
		{
			case 0:
				bul->rect = rcLeft[bul->ani_no];
				break;
			case 1:
				bul->rect = rcDown[bul->ani_no];
				break;
			case 2:
				bul->rect = rcRight[bul->ani_no];
				break;
			case 3:
				bul->rect = rcDown[bul->ani_no];
				break;
		}
	}
}

void ActBullet_Sword1(BULLET *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->count1 == 3)
			bul->bbits &= ~4;

		if (bul->count1 % 5 == 1)
			PlaySoundObject(34, 1);

		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x800;
					break;
				case 1:
					bul->ym = -0x800;
					break;
				case 2:
					bul->xm = 0x800;
					break;
				case 3:
					bul->ym = 0x800;
					break;
			}
		}

		RECT rcLeft[4] = {
			{0, 48, 16, 64},
			{16, 48, 32, 64},
			{32, 48, 48, 64},
			{48, 48, 64, 64},
		};

		RECT rcRight[4] = {
			{64, 48, 80, 64},
			{80, 48, 96, 64},
			{96, 48, 112, 64},
			{112, 48, 128, 64},
		};

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 0;

		if (bul->direct == 0)
			bul->rect = rcLeft[bul->ani_no];
		else
			bul->rect = rcRight[bul->ani_no];
	}
}

void ActBullet_Sword2(BULLET *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->count1 == 3)
			bul->bbits &= ~4;

		if (bul->count1 % 7 == 1)
			PlaySoundObject(106, 1);

		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x800;
					break;
				case 1:
					bul->ym = -0x800;
					break;
				case 2:
					bul->xm = 0x800;
					break;
				case 3:
					bul->ym = 0x800;
					break;
			}
		}

		RECT rcLeft[4] = {
			{160, 48, 184, 72},
			{184, 48, 208, 72},
			{208, 48, 232, 72},
			{232, 48, 256, 72},
		};

		RECT rcRight[4] = {
			{160, 72, 184, 96},
			{184, 72, 208, 96},
			{208, 72, 232, 96},
			{232, 72, 256, 96},
		};

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 0;

		if (bul->direct == 0)
			bul->rect = rcLeft[bul->ani_no];
		else
			bul->rect = rcRight[bul->ani_no];
	}
}

void ActBullet_Sword3(BULLET *bul)
{
	RECT rcLeft[2] = {
		{272, 0, 296, 24},
		{296, 0, 320, 24},
	};

	RECT rcUp[2] = {
		{272, 48, 296, 72},
		{296, 0, 320, 24},
	};

	RECT rcRight[2] = {
		{272, 24, 296, 48},
		{296, 24, 320, 48},
	};

	RECT rcDown[2] = {
		{296, 48, 320, 72},
		{296, 24, 320, 48},
	};

	switch (bul->act_no)
	{
		case 0:
			bul->act_no = 1;
			bul->xm = 0;
			bul->ym = 0;
			// Fallthrough
		case 1:
			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x800;
					break;
				case 1:
					bul->ym = -0x800;
					break;
				case 2:
					bul->xm = 0x800;
					break;
				case 3:
					bul->ym = 0x800;
					break;
			}

			if (bul->life != 100)
			{
				bul->act_no = 2;
				bul->ani_no = 1;
				bul->damage = -1;
				bul->act_wait = 0;
			}

			if (++bul->act_wait % 4 == 1)
			{
				PlaySoundObject(106, 1);

				if (++bul->count1 % 2)
					SetBullet(23, bul->x, bul->y, 0);
				else
					SetBullet(23, bul->x, bul->y, 2);
			}

			if ( ++bul->count1 == 5 )
				bul->bbits &= ~4u;

			if (bul->count1 > bul->life_count)
			{
				bul->cond = 0;
				SetCaret(bul->x, bul->y, 3, 0);
				return;
			}

			break;

		case 2:
			bul->xm = 0;
			bul->ym = 0;
			++bul->act_wait;

			if (Random(-1, 1) == 0)
			{
				PlaySoundObject(106, 1);

				if (Random(0, 1) % 2)
					SetBullet(23, bul->x + (Random(-0x40, 0x40) * 0x200), bul->y + (Random(-0x40, 0x40) * 0x200), 0);
				else
					SetBullet(23, bul->x + (Random(-0x40, 0x40) * 0x200), bul->y + (Random(-0x40, 0x40) * 0x200), 2);
			}

			if (bul->act_wait > 50)
				bul->cond = 0;
	}

	bul->x += bul->xm;
	bul->y += bul->ym;

	switch (bul->direct)
	{
		case 0:
			bul->rect = rcLeft[bul->ani_no];
			break;
		case 1:
			bul->rect = rcUp[bul->ani_no];
			break;
		case 2:
			bul->rect = rcRight[bul->ani_no];
			break;
		case 3:
			bul->rect = rcDown[bul->ani_no];
			break;
	}

	if (bul->act_wait % 2)
		bul->rect.right = 0;
}

void ActBullet_Edge(BULLET *bul)
{
	switch (bul->act_no)
	{
		case 0:
			bul->act_no = 1;
			bul->y -= 0x1800;

			if (bul->direct == 0)
				bul->x += 0x2000;
			else
				bul->x -= 0x2000;
			// Fallthrough
		case 1:
			if (++bul->ani_wait > 2)
			{
				bul->ani_wait = 0;
				++bul->ani_no;
			}

			if (bul->direct == 0)
				bul->x -= 0x400;
			else
				bul->x += 0x400;

			bul->y += 0x400;

			if (bul->ani_no == 1)
				bul->damage = 2;
			else
				bul->damage = 1;

			if (bul->ani_no > 4)
				bul->cond = 0;

			break;
	}

	RECT rcLeft[5] = {
		{0, 64, 24, 88},
		{24, 64, 48, 88},
		{48, 64, 72, 88},
		{72, 64, 96, 88},
		{96, 64, 120, 88},
	};

	RECT rcRight[5] = {
		{0, 88, 24, 112},
		{24, 88, 48, 112},
		{48, 88, 72, 112},
		{72, 88, 96, 112},
		{96, 88, 120, 112},
	};

	if (bul->direct == 0)
		bul->rect = rcLeft[bul->ani_no];
	else
		bul->rect = rcRight[bul->ani_no];
}

void ActBullet_Drop(BULLET *bul)
{
	RECT rc[1] = {0, 0, 0, 0};

	if (++bul->act_wait > 2)
		bul->cond = 0;

	bul->rect = rc[0];
}

void ActBullet_SuperMissile(BULLET *bul, int level)
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
		SetBullet(level + 30, bul->x, bul->y, 0);
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
					bul->enemyXL = 0x1000;
					bul->blockXL = 0x1000;
					break;

				case 1:
				case 3:
					bul->tgt_x = bul->x;
					bul->enemyYL = 0x1000;
					bul->blockYL = 0x1000;
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

				static unsigned int inc;

				switch (++inc % 3)
				{
					case 0:
						bul->ani_no = 0x200;
						break;
					case 1:
						bul->ani_no = 0x100;
						break;
					case 2:
						bul->ani_no = 0xAA;
						break;
				}
			}
			else
			{
				bul->ani_no = 0x200;
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
							bul->ym += 0x40;
						else
							bul->ym -= 0x40;

						break;
					case 1:
					case 3:
						if (bul->tgt_x > bul->x)
							bul->xm += 0x40;
						else
							bul->xm -= 0x40;

						break;
				}
			}

			if (bul->xm < -0x1400)
				bul->xm = -0x1400;
			if (bul->xm > 0x1400 )
				bul->xm = 0x1400;

			if (bul->ym < -0x1400)
				bul->ym = -0x1400;
			if (bul->ym > 0x1400)
				bul->ym = 0x1400;

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

	RECT rect1[4] = {
		{120, 96, 136, 112},
		{136, 96, 152, 112},
		{152, 96, 168, 112},
		{168, 96, 184, 112},
	};

	RECT rect2[4] = {
		{184, 96, 200, 112},
		{200, 96, 216, 112},
		{216, 96, 232, 112},
		{232, 96, 248, 112},
	};

	switch (level)
	{
		case 1:
			bul->rect = rect1[bul->direct];
			break;
		case 2:
			bul->rect = rect2[bul->direct];
			break;
		case 3:
			bul->rect = rect1[bul->direct];
			break;
	}
}

void ActBullet_SuperBom(BULLET *bul, int level)
{
	switch (bul->act_no)
	{
		case 0:
			bul->act_no = 1;

			switch (level)
			{
				case 1:
					bul->act_wait = 10;
					break;
				case 2:
					bul->act_wait = 14;
					break;
				case 3:
					bul->act_wait = 6;
					break;
			}

			PlaySoundObject(44, 1);
			// Fallthrough
		case 1:
			if (level == 1)
			{
				if (bul->act_wait % 3 == 0)
					SetDestroyNpCharUp(bul->x + (Random(-16, 16) * 0x200), bul->y + (Random(-16, 16) * 0x200), bul->enemyXL, 2);
			}
			else if (level == 2)
			{
				if (bul->act_wait % 3 == 0)
					SetDestroyNpCharUp(bul->x + (Random(-32, 32) * 0x200), bul->y + (Random(-32, 32) * 0x200), bul->enemyXL, 2);
			}
			else if (level == 3)
			{
				if (bul->act_wait % 3 == 0)
					SetDestroyNpCharUp(bul->x + (Random(-40, 40) * 0x200), bul->y + (Random(-40, 40) * 0x200), bul->enemyXL, 2);
			}

			if (--bul->act_wait < 0)
				bul->cond = 0;

			break;
	}
}

void ActBullet_Nemesis(BULLET *bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no)
		{
			if (level == 1 && bul->count1 % 4 == 1)
			{
				switch (bul->direct)
				{
					case 0:
						SetNpChar(4, bul->x, bul->y, -0x200, Random(-0x200, 0x200), 2, 0, 0x100);
						break;
					case 1:
						SetNpChar(4, bul->x, bul->y, Random(-0x200, 0x200), -0x200, 2, 0, 0x100);
						break;
					case 2:
						SetNpChar(4, bul->x, bul->y, 0x200, Random(-0x200, 0x200), 2, 0, 0x100);
						break;
					case 3:
						SetNpChar(4, bul->x, bul->y, Random(-0x200, 0x200), 0x200, 2, 0, 0x100);
						break;
				}
			}

			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;
			bul->count1 = 0;

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

			if (level == 3)
			{
				bul->xm /= 3;
				bul->ym /= 3;
			}
		}

		if (++bul->ani_no > 1)
			bul->ani_no = 0;

		RECT rcL[2] = {
			{0, 112, 32, 128},
			{0, 128, 32, 144},
		};

		RECT rcU[2] = {
			{32, 112, 48, 144},
			{48, 112, 64, 144},
		};

		RECT rcR[2] = {
			{64, 112, 96, 128},
			{64, 128, 96, 144},
		};

		RECT rcD[2] = {
			{96, 112, 112, 144},
			{112, 112, 128, 144},
		};

		switch (bul->direct)
		{
			case 0:
				bul->rect = rcL[bul->ani_no];
				break;
			case 1:
				bul->rect = rcU[bul->ani_no];
				break;
			case 2:
				bul->rect = rcR[bul->ani_no];
				break;
			case 3:
				bul->rect = rcD[bul->ani_no];
				break;
		}

		bul->rect.top += 32 * ((level - 1) / 2);
		bul->rect.bottom += 32 * ((level - 1) / 2);
		bul->rect.left += (level - 1) % 2 << 7;
		bul->rect.right += (level - 1) % 2 << 7;
	}
}

void ActBullet_Spur(BULLET *bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->damage && bul->life != 100)
			bul->damage = 0;

		if (bul->act_no)
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}
		else
		{
			bul->act_no = 1;

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
		}

		RECT rect1[2] = {
			{128, 32, 144, 48},
			{144, 32, 160, 48},
		};

		RECT rect2[2] = {
			{160, 32, 176, 48},
			{176, 32, 192, 48},
		};

		RECT rect3[2] = {
			{128, 48, 144, 64},
			{144, 48, 160, 64},
		};

		bul->damage = bul->life;

		switch (level)
		{
			case 1:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect1[1];
				else
					bul->rect = rect1[0];

				break;

			case 2:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect2[1];
				else
					bul->rect = rect2[0];

				break;

			case 3:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect3[1];
				else
					bul->rect = rect3[0];

				break;
		}

		SetBullet(level + 39, bul->x, bul->y, bul->direct);
	}
}

void ActBullet_SpurTail(BULLET *bul, int level)
{
	if ( ++bul->count1 > 20 )
		bul->ani_no = bul->count1 - 20;
	if ( bul->ani_no > 2 )
		bul->cond = 0;
	if ( bul->damage && bul->life != 100 )
		bul->damage = 0;

	RECT rc_h_lv1[3] = {
		{192, 32, 200, 40},
		{200, 32, 208, 40},
		{208, 32, 216, 40},
	};

	RECT rc_v_lv1[3] = {
		{192, 40, 200, 48},
		{200, 40, 208, 48},
		{208, 40, 216, 48},
	};

	RECT rc_h_lv2[3] = {
		{216, 32, 224, 40},
		{224, 32, 232, 40},
		{232, 32, 240, 40},
	};

	RECT rc_v_lv2[3] = {
		{216, 40, 224, 48},
		{224, 40, 232, 48},
		{232, 40, 240, 48},
	};

	RECT rc_h_lv3[3] = {
		{240, 32, 248, 40},
		{248, 32, 256, 40},
		{256, 32, 264, 40},
	};

	RECT rc_v_lv3[3] = {
		{240, 32, 248, 40},
		{248, 32, 256, 40},
		{256, 32, 264, 40},
	};

	switch (level)
	{
		case 1:
			if (bul->direct == 0 || bul->direct == 2)
				bul->rect = rc_h_lv1[bul->ani_no];
			else
				bul->rect = rc_v_lv1[bul->ani_no];

			break;

		case 2:
			if (bul->direct == 0 || bul->direct == 2)
				bul->rect = rc_h_lv2[bul->ani_no];
			else
				bul->rect = rc_v_lv2[bul->ani_no];

			break;

		case 3:
			if (bul->direct == 0 || bul->direct == 2)
				bul->rect = rc_h_lv3[bul->ani_no];
			else
				bul->rect = rc_v_lv3[bul->ani_no];

			break;
	}
}

void ActBullet_EnemyClear(BULLET *bul)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
	}
	else
	{
		bul->damage = 10000;
		bul->enemyXL = 0xC8000;
		bul->enemyYL = 0xC8000;
	}
}

void ActBullet_Star(BULLET *bul)
{
	if (++bul->count1 > bul->life_count)
		bul->cond = 0;
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
					case 1:
						ActBullet_Frontia1(&gBul[i]);
						break;
					case 2:
						ActBullet_Frontia2(&gBul[i], 2);
						break;
					case 3:
						ActBullet_Frontia2(&gBul[i], 3);
						break;
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
					case 19:
						ActBullet_Bubblin1(&gBul[i]);
						break;
					case 20:
						ActBullet_Bubblin2(&gBul[i]);
						break;
					case 21:
						ActBullet_Bubblin3(&gBul[i]);
						break;
					case 22:
						ActBullet_Spine(&gBul[i]);
						break;
					case 23:
						ActBullet_Edge(&gBul[i]);
						break;
					case 24:
						ActBullet_Drop(&gBul[i]);
						break;
					case 25:
						ActBullet_Sword1(&gBul[i]);
						break;
					case 26:
						ActBullet_Sword2(&gBul[i]);
						break;
					case 27:
						ActBullet_Sword3(&gBul[i]);
						break;
					case 28:
						ActBullet_SuperMissile(&gBul[i], 1);
						break;
					case 29:
						ActBullet_SuperMissile(&gBul[i], 2);
						break;
					case 30:
						ActBullet_SuperMissile(&gBul[i], 3);
						break;
					case 31:
						ActBullet_SuperBom(&gBul[i], 1);
						break;
					case 32:
						ActBullet_SuperBom(&gBul[i], 2);
						break;
					case 33:
						ActBullet_SuperBom(&gBul[i], 3);
						break;
					case 34:
						ActBullet_Nemesis(&gBul[i], 1);
						break;
					case 35:
						ActBullet_Nemesis(&gBul[i], 2);
						break;
					case 36:
						ActBullet_Nemesis(&gBul[i], 3);
						break;
					case 37:
						ActBullet_Spur(&gBul[i], 1);
						break;
					case 38:
						ActBullet_Spur(&gBul[i], 2);
						break;
					case 39:
						ActBullet_Spur(&gBul[i], 3);
						break;
					case 40:
						ActBullet_SpurTail(&gBul[i], 1);
						break;
					case 41:
						ActBullet_SpurTail(&gBul[i], 2);
						break;
					case 42:
						ActBullet_SpurTail(&gBul[i], 3);
						break;
					case 43:
						ActBullet_Nemesis(&gBul[i], 1);
						break;
					case 44:
						ActBullet_EnemyClear(&gBul[i]);
						break;
					case 45:
						ActBullet_Star(&gBul[i]);
						break;
					default:
						continue;
				}
			}
			else
			{
				gBul[i].cond = 0;
			}
		}
	}
}

BOOL IsActiveSomeBullet(void)
{
	for (int i = 0; i < 0x40; ++i)
	{
		if (gBul[i].cond & 0x80)
		{
			switch (gBul[i].code_bullet)
			{
				case 0xD:
				case 0xE:
				case 0xF:
				case 0x10:
				case 0x11:
				case 0x12:
				case 0x17:
				case 0x19:
				case 0x1A:
				case 0x1B:
				case 0x1C:
				case 0x1D:
				case 0x1E:
				case 0x1F:
				case 0x20:
				case 0x21:
					return TRUE;
			}
		}
	}

	return FALSE;
}