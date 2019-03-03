#include "BossFrog.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

static void ActBossChar02_01(void)
{
	int minus;

	if (gBoss[0].direct == 0)
		minus = 1;
	else
		minus = -1;

	switch (gBoss[0].ani_no)
	{
		case 0:
			gBoss[1].hit_voice = 52;
			gBoss[1].hit.front = 0x2000;
			gBoss[1].hit.top = 0x2000;
			gBoss[1].hit.back = 0x2000;
			gBoss[1].hit.bottom = 0x2000;
			gBoss[1].size = 3;
			gBoss[1].bits = 4;
			break;

		case 1:
			gBoss[1].x = gBoss[0].x + -0x3000 * minus;
			gBoss[1].y = gBoss[0].y - 0x3000;
			break;

		case 2:
			gBoss[1].x = gBoss[0].x + -0x3000 * minus;
			gBoss[1].y = gBoss[0].y - 0x2800;
			break;

		case 3:
		case 4:
			gBoss[1].x = gBoss[0].x + -0x3000 * minus;
			gBoss[1].y = gBoss[0].y - 0x2000;
			break;

		case 5:
			gBoss[1].x = gBoss[0].x + -0x3000 * minus;
			gBoss[1].y = gBoss[0].y - 0x5600;
			break;
	}
}

static void ActBossChar02_02(void)
{
	if (gBoss[0].ani_no)
	{
		if (gBoss[0].ani_no > 0 && gBoss[0].ani_no <= 5)
		{
			gBoss[2].x = gBoss[0].x;
			gBoss[2].y = gBoss[0].y;
		}
	}
	else
	{
		gBoss[2].hit_voice = 52;
		gBoss[2].hit.front = 0x3000;
		gBoss[2].hit.top = 0x2000;
		gBoss[2].hit.back = 0x3000;
		gBoss[2].hit.bottom = 0x2000;
		gBoss[2].size = 3;
		gBoss[2].bits = 4;
	}
}

void ActBossChar_Frog(void)
{
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

	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss->x = 0xC000;
			gBoss->y = 0x19000;
			gBoss->direct = 2;
			gBoss->view.front = 0x6000;
			gBoss->view.top = 0x6000;
			gBoss->view.back = 0x4000;
			gBoss->view.bottom = 0x2000;
			gBoss->hit_voice = 52;
			gBoss->hit.front = 0x3000;
			gBoss->hit.top = 0x2000;
			gBoss->hit.back = 0x3000;
			gBoss->hit.bottom = 0x2000;
			gBoss->size = 3;
			gBoss->exp = 1;
			gBoss->code_event = 1000;
			gBoss->bits |= 0x8200;
			gBoss->life = 300;
			break;

		case 10:
			gBoss->act_no = 11;
			gBoss->ani_no = 3;
			gBoss->cond = 0x80;
			gBoss->rect = rcRight[0];
			gBoss[1].cond = -112;
			gBoss[1].code_event = 1000;
			gBoss[2].cond = 0x80;
			gBoss[1].damage = 5;
			gBoss[2].damage = 5;

			for (int i = 0; i < 8; ++i)
				SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			break;

		case 20:
			gBoss->act_no = 21;
			gBoss->act_wait = 0;
			// Fallthrough
		case 21:
			if (++gBoss->act_wait / 2 % 2)
				gBoss->ani_no = 3;
			else
				gBoss->ani_no = 0;

			break;

		case 100:
			gBoss->act_no = 101;
			gBoss->act_wait = 0;
			gBoss->ani_no = 1;
			gBoss->xm = 0;
			// Fallthrough
		case 101:
			if (++gBoss->act_wait > 50)
			{
				gBoss->act_no = 102;
				gBoss->ani_wait = 0;
				gBoss->ani_no = 2;
			}

			break;

		case 102:
			if (++gBoss->ani_wait > 10)
			{
				gBoss->act_no = 103;
				gBoss->ani_wait = 0;
				gBoss->ani_no = 1;
			}

			break;

		case 103:
			if (++gBoss->ani_wait > 4)
			{
				gBoss->act_no = 104;
				gBoss->ani_no = 5;
				gBoss->ym = -0x400;
				PlaySoundObject(25, 1);

				if (gBoss->direct == 0)
					gBoss->xm = -0x200;
				else
					gBoss->xm = 0x200;

				gBoss->view.top = 0x8000;
				gBoss->view.bottom = 0x3000;
			}

			break;

		case 104:
			if (gBoss->direct == 0 && gBoss->flag & 1)
			{
				gBoss->direct = 2;
				gBoss->xm = 0x200;
			}

			if (gBoss->direct == 2 && gBoss->flag & 4)
			{
				gBoss->direct = 0;
				gBoss->xm = -0x200;
			}

			if (gBoss->flag & 8)
			{
				PlaySoundObject(26, 1);
				SetQuake(30);
				gBoss->act_no = 100;
				gBoss->ani_no = 1;
				gBoss->view.top = 0x6000;
				gBoss->view.bottom = 0x2000;

				if (gBoss->direct == 0 && gBoss->x < gMC.x)
				{
					gBoss->direct = 2;
					gBoss->act_no = 110;
				}

				if (gBoss->direct == 2 && gBoss->x > gMC.x)
				{
					gBoss->direct = 0;
					gBoss->act_no = 110;
				}

				SetNpChar(110, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + gBoss->hit.bottom, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			break;

		case 110:
			gBoss->ani_no = 1;
			gBoss->act_wait = 0;
			gBoss->act_no = 111;
			// Fallthrough
		case 111:
			++gBoss->act_wait;
			gBoss->xm = 8 * gBoss->xm / 9;

			if (gBoss->act_wait > 50)
			{
				gBoss->ani_no = 2;
				gBoss->ani_wait = 0;
				gBoss->act_no = 112;
			}

			break;

		case 112:
			if (++gBoss->ani_wait > 4)
			{
				gBoss->act_no = 113;
				gBoss->act_wait = 0;
				gBoss->ani_no = 3;
				gBoss->count1 = 16;
				gBoss[1].bits |= 0x20;
				gBoss->tgt_x = gBoss->life;
			}

			break;

		case 113:
			if (gBoss->shock)
			{
				if (gBoss->count2++ / 2 % 2)
					gBoss->ani_no = 4;
				else
					gBoss->ani_no = 3;
			}
			else
			{
				gBoss->count2 = 0;
				gBoss->ani_no = 3;
			}

			gBoss->xm = 10 * gBoss->xm / 11;

			if (++gBoss->act_wait > 16)
			{
				gBoss->act_wait = 0;
				--gBoss->count1;

				unsigned char deg;

				if (gBoss->direct == 0)
					deg = GetArktan(gBoss->x - 0x4000 - gMC.x, gBoss->y - 0x1000 - gMC.y);
				else
					deg = GetArktan(gBoss->x + 0x4000 - gMC.x, gBoss->y - 0x1000 - gMC.y);

				deg += Random(-16, 16);

				int ym = GetSin(deg);
				int xm = GetCos(deg);

				if (gBoss->direct == 0)
					SetNpChar(108, gBoss->x - 0x4000, gBoss->y - 0x1000, xm, ym, 0, 0, 0x100);
				else
					SetNpChar(108, gBoss->x + 0x4000, gBoss->y - 0x1000, xm, ym, 0, 0, 0x100);

				PlaySoundObject(39, 1);

				if (gBoss->count1 == 0 || gBoss->life < gBoss->tgt_x - 90)
				{
					gBoss->act_no = 114;
					gBoss->act_wait = 0;
					gBoss->ani_no = 2;
					gBoss->ani_wait = 0;
					gBoss[1].bits &= ~0x20;
				}
			}

			break;

		case 114:
			if (++gBoss->ani_wait > 10)
			{
				if (++gBoss[1].count1 > 2)
				{
					gBoss[1].count1 = 0;
					gBoss->act_no = 120;
				}
				else
				{
					gBoss->act_no = 100;
				}

				gBoss->ani_wait = 0;
				gBoss->ani_no = 1;
			}

			break;

		case 120:
			gBoss->act_no = 121;
			gBoss->act_wait = 0;
			gBoss->ani_no = 1;
			gBoss->xm = 0;
			// Fallthrough
		case 121:
			if (++gBoss->act_wait > 50)
			{
				gBoss->act_no = 122;
				gBoss->ani_wait = 0;
				gBoss->ani_no = 2;
			}

			break;

		case 122:
			if (++gBoss->ani_wait > 20)
			{
				gBoss->act_no = 123;
				gBoss->ani_wait = 0;
				gBoss->ani_no = 1;
			}

			break;

		case 123:
			if (++gBoss->ani_wait > 4)
			{
				gBoss->act_no = 124;
				gBoss->ani_no = 5;
				gBoss->ym = -0xA00;
				gBoss->view.top = 0x8000;
				gBoss->view.bottom = 0x3000;
				PlaySoundObject(25, 1);
			}

			break;

		case 124:
			if (gBoss->flag & 8)
			{
				PlaySoundObject(26, 1);
				SetQuake(60);
				gBoss->act_no = 100;
				gBoss->ani_no = 1;
				gBoss->view.top = 0x6000;
				gBoss->view.bottom = 0x2000;

				for (int i = 0; i < 2; ++i)
					SetNpChar(104, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (int i = 0; i < 6; ++i)
					SetNpChar(110, Random(4, 16) * 0x2000, Random(0, 4) * 0x2000, 0, 0, 4, 0, 0x80);

				for (int i = 0; i < 8; ++i)
					SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + gBoss->hit.bottom, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

				if (gBoss->direct == 0 && gMC.x > gBoss->x)
				{
					gBoss->direct = 2;
					gBoss->act_no = 110;
				}

				if ( gBoss->direct == 2 && gMC.x < gBoss->x)
				{
					gBoss->direct = 0;
					gBoss->act_no = 110;
				}
			}

			break;

		case 130:
			gBoss->act_no = 131;
			gBoss->ani_no = 3;
			gBoss->act_wait = 0;
			gBoss->xm = 0;
			PlaySoundObject(72, 1);

			for (int i = 0; i < 8; ++i)
				SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			// Fallthrough
		case 131:
			if (++gBoss->act_wait % 5 == 0)
				SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			if (gBoss->act_wait / 2 % 2)
				gBoss->x -= 0x200;
			else
				gBoss->x += 0x200;

			if (gBoss->act_wait > 100)
			{
				gBoss->act_wait = 0;
				gBoss->act_no = 132;
			}

			break;

		case 132:
			if (++gBoss->act_wait / 2 % 2)
			{
				gBoss->view.front = 0x2800;
				gBoss->view.top = 0x1800;
				gBoss->view.back = 0x2800;
				gBoss->view.bottom = 0x1800;
				gBoss->ani_no = 6;
			}
			else
			{
				gBoss->view.front = 0x6000;
				gBoss->view.top = 0x6000;
				gBoss->view.back = 0x4000;
				gBoss->view.bottom = 0x2000;
				gBoss->ani_no = 3;
			}

			if (gBoss->act_wait % 9 == 0)
				SetNpChar(4, gBoss->x + (Random(-12, 12) * 0x200), gBoss->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			if (gBoss->act_wait > 150)
			{
				gBoss->act_no = 140;
				gBoss->hit.bottom = 0x1800;
			}

			break;

		case 140:
			gBoss->act_no = 141;
			// Fallthrough
		case 141:
			if (gBoss->flag & 8)
			{
				gBoss->act_no = 142;
				gBoss->act_wait = 0;
				gBoss->ani_no = 7;
			}

			break;

		case 142:
			if (++gBoss->act_wait > 30)
			{
				gBoss->ani_no = 8;
				gBoss->ym = -0xA00;
				gBoss->bits |= 8;
				gBoss->act_no = 143;
			}

			break;

		case 143:
			gBoss->ym = -0xA00;

			if (gBoss->y < 0)
			{
				gBoss->cond = 0;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;
	}

	gBoss->ym += 0x40;
	if (gBoss->ym > 0x5FF)
		gBoss->ym = 0x5FF;

	gBoss->x += gBoss->xm;
	gBoss->y += gBoss->ym;

	if (gBoss->direct == 0)
		gBoss->rect = rcLeft[gBoss->ani_no];
	else
		gBoss->rect = rcRight[gBoss->ani_no];

	ActBossChar02_01();
	ActBossChar02_02();
}
