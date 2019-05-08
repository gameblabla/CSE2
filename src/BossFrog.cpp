#include "BossFrog.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

// Balfrog's mouth
static void ActBossChar02_01(void)
{
	NPCHAR *boss;
	int minus;

	if (gBoss[0].direct == 0)
		minus = 1;
	else
		minus = -1;

	boss = &gBoss[1];

	switch (gBoss[0].ani_no)
	{
		case 0:
			boss->hit_voice = 52;
			boss->hit.front = 0x2000;
			boss->hit.top = 0x2000;
			boss->hit.back = 0x2000;
			boss->hit.bottom = 0x2000;
			boss->size = 3;
			boss->bits = 4;
			break;

		case 1:
			boss->x = gBoss[0].x + -0x3000 * minus;
			boss->y = gBoss[0].y - 0x3000;
			break;

		case 2:
			boss->x = gBoss[0].x + -0x3000 * minus;
			boss->y = gBoss[0].y - 0x2800;
			break;

		case 3:
		case 4:
			boss->x = gBoss[0].x + -0x3000 * minus;
			boss->y = gBoss[0].y - 0x2000;
			break;

		case 5:
			boss->x = gBoss[0].x + -0x3000 * minus;
			boss->y = gBoss[0].y - 0x5600;
			break;
	}
}

static void ActBossChar02_02(void)
{
	NPCHAR *boss = &gBoss[2];

	switch (gBoss[0].ani_no)
	{
		case 0:
			boss->hit_voice = 52;
			boss->hit.front = 0x3000;
			boss->hit.top = 0x2000;
			boss->hit.back = 0x3000;
			boss->hit.bottom = 0x2000;
			boss->size = 3;
			boss->bits = 4;
			break;

		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			boss->x = gBoss[0].x;
			boss->y = gBoss[0].y;
			break;
	}
}

// Main boss AI
void ActBossChar_Frog(void)
{
	int i;
	unsigned char deg;
	int ym;
	int xm;

	// Rects 1-4 are for when Balfrog is a frog, 5-8 for when he reverts and goes into the ceiling
	RECT rcLeft[9] = {
		{0, 0, 0, 0},
		{0, 48, 80, 112},
		{0, 112, 80, 176},
		{0, 176, 80, 240},
		{160, 48, 240, 112},
		{160, 112, 240, 200},
		{200, 0, 240, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
	};

	RECT rcRight[9] = {
		{0, 0, 0, 0},
		{80, 48, 160, 112},
		{80, 112, 160, 176},
		{80, 176, 160, 240},
		{240, 48, 320, 112},
		{240, 112, 320, 200},
		{200, 24, 240, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
	};

	NPCHAR *boss = gBoss;

	switch (boss->act_no)
	{
		case 0:
			boss->x = 0xC000;
			boss->y = 0x19000;
			boss->direct = 2;
			boss->view.front = 0x6000;
			boss->view.top = 0x6000;
			boss->view.back = 0x4000;
			boss->view.bottom = 0x2000;
			boss->hit_voice = 52;
			boss->hit.front = 0x3000;
			boss->hit.top = 0x2000;
			boss->hit.back = 0x3000;
			boss->hit.bottom = 0x2000;
			boss->size = 3;
			boss->exp = 1;
			boss->code_event = 1000;
			boss->bits |= 0x8200;
			boss->life = 300;
			break;

		case 10:
			boss->act_no = 11;
			boss->ani_no = 3;
			boss->cond = 0x80;
			boss->rect = rcRight[0];
			gBoss[1].cond = -112;
			gBoss[1].code_event = 1000;
			gBoss[2].cond = 0x80;
			gBoss[1].damage = 5;
			gBoss[2].damage = 5;

			for (i = 0; i < 8; ++i)
				SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			break;

		case 20:
			boss->act_no = 21;
			boss->act_wait = 0;
			// Fallthrough
		case 21:
			if (++boss->act_wait / 2 % 2)
				boss->ani_no = 3;
			else
				boss->ani_no = 0;

			break;

		case 100:
			boss->act_no = 101;
			boss->act_wait = 0;
			boss->ani_no = 1;
			boss->xm = 0;
			// Fallthrough
		case 101:
			if (++boss->act_wait > 50)
			{
				boss->act_no = 102;
				boss->ani_wait = 0;
				boss->ani_no = 2;
			}

			break;

		case 102:
			if (++boss->ani_wait > 10)
			{
				boss->act_no = 103;
				boss->ani_wait = 0;
				boss->ani_no = 1;
			}

			break;

		case 103:
			if (++boss->ani_wait > 4)
			{
				boss->act_no = 104;
				boss->ani_no = 5;
				boss->ym = -0x400;
				PlaySoundObject(25, 1);

				if (boss->direct == 0)
					boss->xm = -0x200;
				else
					boss->xm = 0x200;

				boss->view.top = 0x8000;
				boss->view.bottom = 0x3000;
			}

			break;

		case 104:
			if (boss->direct == 0 && boss->flag & 1)
			{
				boss->direct = 2;
				boss->xm = 0x200;
			}

			if (boss->direct == 2 && boss->flag & 4)
			{
				boss->direct = 0;
				boss->xm = -0x200;
			}

			if (boss->flag & 8)
			{
				PlaySoundObject(26, 1);
				SetQuake(30);
				boss->act_no = 100;
				boss->ani_no = 1;
				boss->view.top = 0x6000;
				boss->view.bottom = 0x2000;

				if (boss->direct == 0 && boss->x < gMC.x)
				{
					boss->direct = 2;
					boss->act_no = 110;
				}

				if (boss->direct == 2 && boss->x > gMC.x)
				{
					boss->direct = 0;
					boss->act_no = 110;
				}

				SetNpChar(110, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (i = 0; i < 4; ++i)
					SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + boss->hit.bottom, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			break;

		case 110:
			boss->ani_no = 1;
			boss->act_wait = 0;
			boss->act_no = 111;
			// Fallthrough
		case 111:
			++boss->act_wait;
			boss->xm = 8 * boss->xm / 9;

			if (boss->act_wait > 50)
			{
				boss->ani_no = 2;
				boss->ani_wait = 0;
				boss->act_no = 112;
			}

			break;

		case 112:
			if (++boss->ani_wait > 4)
			{
				boss->act_no = 113;
				boss->act_wait = 0;
				boss->ani_no = 3;
				boss->count1 = 16;
				gBoss[1].bits |= 0x20;
				boss->tgt_x = boss->life;
			}

			break;

		case 113:
			if (boss->shock)
			{
				if (boss->count2++ / 2 % 2)
					boss->ani_no = 4;
				else
					boss->ani_no = 3;
			}
			else
			{
				boss->count2 = 0;
				boss->ani_no = 3;
			}

			boss->xm = 10 * boss->xm / 11;

			if (++boss->act_wait > 16)
			{
				boss->act_wait = 0;
				--boss->count1;

				if (boss->direct == 0)
					deg = GetArktan(boss->x - 0x4000 - gMC.x, boss->y - 0x1000 - gMC.y);
				else
					deg = GetArktan(boss->x + 0x4000 - gMC.x, boss->y - 0x1000 - gMC.y);

				deg += (unsigned char)Random(-16, 16);

				ym = GetSin(deg);
				xm = GetCos(deg);

				if (boss->direct == 0)
					SetNpChar(108, boss->x - 0x4000, boss->y - 0x1000, xm, ym, 0, 0, 0x100);
				else
					SetNpChar(108, boss->x + 0x4000, boss->y - 0x1000, xm, ym, 0, 0, 0x100);

				PlaySoundObject(39, 1);

				if (boss->count1 == 0 || boss->life < boss->tgt_x - 90)
				{
					boss->act_no = 114;
					boss->act_wait = 0;
					boss->ani_no = 2;
					boss->ani_wait = 0;
					gBoss[1].bits &= ~0x20;
				}
			}

			break;

		case 114:
			if (++boss->ani_wait > 10)
			{
				if (++gBoss[1].count1 > 2)
				{
					gBoss[1].count1 = 0;
					boss->act_no = 120;
				}
				else
				{
					boss->act_no = 100;
				}

				boss->ani_wait = 0;
				boss->ani_no = 1;
			}

			break;

		case 120:
			boss->act_no = 121;
			boss->act_wait = 0;
			boss->ani_no = 1;
			boss->xm = 0;
			// Fallthrough
		case 121:
			if (++boss->act_wait > 50)
			{
				boss->act_no = 122;
				boss->ani_wait = 0;
				boss->ani_no = 2;
			}

			break;

		case 122:
			if (++boss->ani_wait > 20)
			{
				boss->act_no = 123;
				boss->ani_wait = 0;
				boss->ani_no = 1;
			}

			break;

		case 123:
			if (++boss->ani_wait > 4)
			{
				boss->act_no = 124;
				boss->ani_no = 5;
				boss->ym = -0xA00;
				boss->view.top = 0x8000;
				boss->view.bottom = 0x3000;
				PlaySoundObject(25, 1);
			}

			break;

		case 124:
			if (boss->flag & 8)
			{
				PlaySoundObject(26, 1);
				SetQuake(60);
				boss->act_no = 100;
				boss->ani_no = 1;
				boss->view.top = 0x6000;
				boss->view.bottom = 0x2000;

				for (i = 0; i < 2; ++i)
					SetNpChar(104, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (i = 0; i < 6; ++i)
					SetNpChar(110, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (i = 0; i < 8; ++i)
					SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + boss->hit.bottom, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

				if (boss->direct == 0 && boss->x < gMC.x)
				{
					boss->direct = 2;
					boss->act_no = 110;
				}

				if (boss->direct == 2 && boss->x > gMC.x)
				{
					boss->direct = 0;
					boss->act_no = 110;
				}
			}

			break;

		case 130:
			boss->act_no = 131;
			boss->ani_no = 3;
			boss->act_wait = 0;
			boss->xm = 0;
			PlaySoundObject(72, 1);

			for (i = 0; i < 8; ++i)
				SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			// Fallthrough
		case 131:
			if (++boss->act_wait % 5 == 0)
				SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			if (boss->act_wait / 2 % 2)
				boss->x -= 0x200;
			else
				boss->x += 0x200;

			if (boss->act_wait > 100)
			{
				boss->act_wait = 0;
				boss->act_no = 132;
			}

			break;

		case 132:
			if (++boss->act_wait / 2 % 2)
			{
				boss->view.front = 0x2800;
				boss->view.top = 0x1800;
				boss->view.back = 0x2800;
				boss->view.bottom = 0x1800;
				boss->ani_no = 6;
			}
			else
			{
				boss->view.front = 0x6000;
				boss->view.top = 0x6000;
				boss->view.back = 0x4000;
				boss->view.bottom = 0x2000;
				boss->ani_no = 3;
			}

			if (boss->act_wait % 9 == 0)
				SetNpChar(4, boss->x + (Random(-12, 12) * 0x200), boss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			if (boss->act_wait > 150)
			{
				boss->act_no = 140;
				boss->hit.bottom = 0x1800;
			}

			break;

		case 140:
			boss->act_no = 141;
			// Fallthrough
		case 141:
			if (boss->flag & 8)
			{
				boss->act_no = 142;
				boss->act_wait = 0;
				boss->ani_no = 7;
			}

			break;

		case 142:
			if (++boss->act_wait > 30)
			{
				boss->ani_no = 8;
				boss->ym = -0xA00;
				boss->bits |= 8;
				boss->act_no = 143;
			}

			break;

		case 143:
			boss->ym = -0xA00;

			if (boss->y < 0)
			{
				boss->cond = 0;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;
	}

	boss->ym += 0x40;
	if (boss->ym > 0x5FF)
		boss->ym = 0x5FF;

	boss->x += boss->xm;
	boss->y += boss->ym;

	if (boss->direct == 0)
		boss->rect = rcLeft[boss->ani_no];
	else
		boss->rect = rcRight[boss->ani_no];

	ActBossChar02_01();
	ActBossChar02_02();
}
