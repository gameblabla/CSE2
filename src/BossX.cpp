#include "BossX.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Flash.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

static void ActBossChar03_01(NPCHAR *npc)
{
	RECT rcUp[6] = {
		{0, 0, 72, 32},
		{0, 32, 72, 64},
		{72, 0, 144, 32},
		{144, 0, 216, 32},
		{72, 32, 144, 64},
		{144, 32, 216, 64},
	};

	RECT rcDown[6] = {
		{0, 64, 72, 96},
		{0, 96, 72, 128},
		{72, 64, 144, 96},
		{144, 64, 216, 96},
		{72, 96, 144, 128},
		{144, 96, 216, 128},
	};

	switch (npc->act_no)
	{
		case 10:
			npc->ani_no = 0;
			npc->bits &= ~0x10;
			break;

		case 100:
			npc->bits |= 0x10;
			npc->act_no = 101;
			npc->act_wait = 0;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough
		case 101:
			if (++npc->act_wait > 30)
				npc->act_no = 102;

			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if ( npc->ani_no > 3 )
				npc->ani_no = 2;

			npc->xm -= 0x20;
			break;

		case 102:
			npc->bits &= ~0x10;
			npc->act_no = 103;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 103:
			++npc->act_wait;

			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			npc->xm -= 0x20;
			break;

		case 200:
			npc->bits |= 0x10;
			npc->bits |= 0x80;
			npc->act_no = 201;
			npc->act_wait = 0;
			npc->ani_no = 4;
			npc->ani_wait = 0;
			// Fallthrough
		case 201:
			if (++npc->act_wait > 30)
				npc->act_no = 202;

			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if ( npc->ani_no > 5 )
				npc->ani_no = 4;

			npc->xm += 0x20;
			break;

		case 202:
			npc->bits &= ~0x10;
			npc->act_no = 203;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 203:
			++npc->act_wait;

			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			npc->xm += 0x20;
			break;

		case 300:
			npc->act_no = 301;
			npc->ani_no = 4;
			npc->ani_wait = 0;
			npc->bits |= 0x10;
			// Fallthrough
		case 301:
			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 4;

			npc->xm += 0x20;

			if (npc->xm > 0)
			{
				npc->xm = 0;
				npc->act_no = 10;
			}

			break;

		case 400:
			npc->act_no = 401;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			npc->bits |= 0x10;
			// Fallthrough
		case 401:
			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 2;

			npc->xm -= 0x20;

			if (npc->xm < 0)
			{
				npc->xm = 0;
				npc->act_no = 10;
			}

			break;
	}

	if ((npc->act_no == 101 || npc->act_no == 201 || npc->act_no == 301 || npc->act_no == 401) && npc->act_wait % 2 == 1)
		PlaySoundObject(112, 1);

	if ((npc->act_no == 103 || npc->act_no == 203) && npc->act_wait % 4 == 1)
		PlaySoundObject(111, 1);

	if (npc->act_no < 100 || gMC.y >= npc->y + 0x800 || gMC.y <= npc->y - 0x800)
	{
		npc->damage = 0;
		npc->bits &= ~0x80;
	}
	else
	{
		npc->damage = 10;
		npc->bits |= 0x80;
	}

	if (npc->xm > 0x400)
		npc->xm = 0x400;
	if (npc->xm < -0x400)
		npc->xm = -0x400;

	npc->x += npc->xm;

	if (npc->direct == 1)
		npc->rect = rcUp[npc->ani_no];
	else
		npc->rect = rcDown[npc->ani_no];
}

static void ActBossChar03_02(NPCHAR *npc)
{
	RECT rect[4] = {
		{0, 128, 72, 160},
		{72, 128, 144, 160},
		{0, 160, 72, 192},
		{72, 160, 144, 192},
	};

	switch (npc->act_no)
	{
		case 10:
			npc->act_no = 11;
			npc->act_wait = 30 * npc->ani_no + 30;
			break;

		case 11:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				int x;
				int y;
				int direct;

				switch (npc->ani_no)
				{
					case 0:
						direct = 3;
						x = -0x3C00;
						y = 0xC00;
						break;
					case 1:
						direct = 2;
						x = 0x3C00;
						y = 0xC00;
						break;
					case 2:
						direct = 0;
						x = -0x3C00;
						y = -0xC00;
						break;
					case 3:
						direct = 1;
						x = 0x3C00;
						y = -0xC00;
						break;
				}

				SetNpChar(158, npc->x + x, npc->y + y, 0, 0, direct, 0, 0x100);
				PlaySoundObject(39, 1);
				npc->act_wait = 120;
			}

			break;
	}

	npc->x = (gBoss[0].x + gBoss[npc->count1].x) / 2;
	npc->y = (gBoss[0].y + gBoss[npc->count1].y) / 2;

	npc->rect = rect[npc->ani_no];
}

static void ActBossChar03_03(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 10:
			npc->tgt_x += 0x200;

			if (npc->tgt_x > 0x4000)
			{
				npc->tgt_x = 0x4000;
				npc->act_no = 0;
				gBoss[3].act_no = 10;
				gBoss[4].act_no = 10;
				gBoss[5].act_no = 10;
				gBoss[6].act_no = 10;
			}

			break;

		case 20:
			npc->tgt_x -= 0x200;

			if (npc->tgt_x < 0)
			{
				npc->tgt_x = 0;
				npc->act_no = 0;
				gBoss[3].act_no = 0;
				gBoss[4].act_no = 0;
				gBoss[5].act_no = 0;
				gBoss[6].act_no = 0;
			}

			break;

		case 30:
			npc->tgt_x += 0x200;

			if (npc->tgt_x > 0x2800)
			{
				npc->tgt_x = 0x2800;
				npc->act_no = 0;
				gBoss[7].act_no = 10;
				gBoss[13].act_no = 10;
				gBoss[14].act_no = 10;
				gBoss[15].act_no = 10;
				gBoss[16].act_no = 10;
			}

			break;

		case 40:
			npc->tgt_x -= 0x200;

			if (npc->tgt_x < 0)
			{
				npc->tgt_x = 0;
				npc->act_no = 0;
				gBoss[7].act_no = 0;
				gBoss[13].act_no = 0;
				gBoss[14].act_no = 0;
				gBoss[15].act_no = 0;
				gBoss[16].act_no = 0;
			}

			break;
	}

	RECT rcLeft = {216, 96, 264, 144};
	RECT rcRight = {264, 96, 312, 144};

	if (npc->direct == 0)
	{
		npc->rect = rcLeft;
		npc->x = gBoss[0].x - npc->tgt_x - 0x3000;
		npc->y = gBoss[0].y;
	}
	else
	{
		npc->rect = rcRight;
		npc->x = gBoss[0].x + npc->tgt_x + 0x3000;
		npc->y = gBoss[0].y;
	}
}

static void ActBossChar03_04(NPCHAR *npc)
{
	RECT rect[8] = {
		{0, 192, 16, 208},
		{16, 192, 32, 208},
		{32, 192, 48, 208},
		{48, 192, 64, 208},
		{0, 208, 16, 224},
		{16, 208, 32, 224},
		{32, 208, 48, 224},
		{48, 208, 64, 224},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~0x20;
			npc->ani_no = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 10 * npc->tgt_x + 40;
			npc->bits |= 0x20;
			// Fallthrough
		case 11:
			npc->ani_no = npc->act_wait < 16 && npc->act_wait / 2 % 2;

			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y) + Random(-2, 2);
				const int ym = 3 * GetSin(deg);
				const int xm = 3 * GetCos(deg);
				SetNpChar(156, npc->x, npc->y, xm, ym, 0, 0, 0x100);
				PlaySoundObject(39, 1);
				npc->act_wait = 40;
			}

			break;
	}

	switch (npc->tgt_x)
	{
		case 0:
			npc->x = gBoss[0].x - 0x2C00;
			npc->y = gBoss[0].y - 0x2000;
			break;
		case 1:
			npc->x = gBoss[0].x + 0x3800;
			npc->y = gBoss[0].y - 0x2000;
			break;
		case 2:
			npc->x = gBoss[0].x - 0x1E00;
			npc->y = gBoss[0].y + 0x1C00;
			break;
		case 3:
			npc->x = gBoss[0].x + 0x2200;
			npc->y = gBoss[0].y + 0x1C00;
			break;
	}

	npc->rect = rect[npc->tgt_x + 4 * npc->ani_no];
}

static void ActBossChar03_face(NPCHAR *npc)
{
	RECT rect[3] = {
		{216, 0, 320, 48},
		{216, 48, 320, 96},
		{216, 144, 320, 192},
	};

	switch (npc->act_no)
	{
		case 0:
			gBoss[0].bits &= ~0x20;
			npc->ani_no = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 10 * npc->tgt_x + 40;
			gBoss[0].bits |= 0x20;
			// Fallthrough
		case 11:
			if (gBoss[0].shock)
			{
				static unsigned char flash;

				if ((flash++ >> 1) & 1)
					npc->ani_no = 1;
				else
					npc->ani_no = 0;
			}
			else
			{
				npc->ani_no = 0;
			}

			break;
	}

	gBoss[7].x = gBoss[0].x;
	gBoss[7].y = gBoss[0].y;

	if (gBoss[0].act_no <= 10)
		npc->ani_no = 2;

	npc->rect = rect[npc->ani_no];
}

void ActBossChar_MonstX(void)
{
	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss[0].life = 1;
			gBoss[0].x = -0x28000;
			break;

		case 1:
			gBoss[0].life = 700;
			gBoss[0].exp = 1;
			gBoss[0].act_no = 1;
			gBoss[0].x = 0x100000;
			gBoss[0].y = 102400;
			gBoss[0].hit_voice = 54;
			gBoss[0].hit.front = 0x3000;
			gBoss[0].hit.top = 0x3000;
			gBoss[0].hit.back = 0x3000;
			gBoss[0].hit.bottom = 0x3000;
			gBoss[0].bits = 0x8208;
			gBoss[0].size = 3;
			gBoss[0].code_event = 1000;
			gBoss[0].ani_no = 0;
			gBoss[1].cond = 0x80;
			gBoss[1].size = 3;
			gBoss[1].direct = 0;
			gBoss[1].view.front = 0x3000;
			gBoss[1].view.top = 0x3000;
			gBoss[1].view.back = 0x3000;
			gBoss[1].view.bottom = 0x3000;
			gBoss[1].bits = 8;
			gBoss[2] = gBoss[1];
			gBoss[2].direct = 2;
			gBoss[3].cond = 0x80;
			gBoss[3].life = 60;
			gBoss[3].hit_voice = 54;
			gBoss[3].destroy_voice = 71;
			gBoss[3].size = 2;
			gBoss[3].view.front = 0x1000;
			gBoss[3].view.top = 0x1000;
			gBoss[3].view.back = 0x1000;
			gBoss[3].view.bottom = 0x1000;
			gBoss[3].hit.front = 0xA00;
			gBoss[3].hit.back = 0xA00;
			gBoss[3].hit.top = 0xA00;
			gBoss[3].hit.bottom = 0xA00;
			gBoss[3].bits = 8;
			gBoss[3].tgt_x = 0;
			gBoss[4] = gBoss[3];
			gBoss[4].tgt_x = 1;
			gBoss[5] = gBoss[3];
			gBoss[5].tgt_x = 2;
			gBoss[5].life = 100;
			gBoss[6] = gBoss[3];
			gBoss[6].tgt_x = 3;
			gBoss[6].life = 100;
			gBoss[7].cond = 0x80;
			gBoss[7].x = 0x100000;
			gBoss[7].y = 0x19000;
			gBoss[7].view.front = 0x6800;
			gBoss[7].view.top = 0x3000;
			gBoss[7].view.back = 0x6800;
			gBoss[7].view.bottom = 0x3000;
			gBoss[7].hit_voice = 52;
			gBoss[7].hit.front = 0x1000;
			gBoss[7].hit.top = 0x3000;
			gBoss[7].hit.back = 0x1000;
			gBoss[7].hit.bottom = 0x2000;
			gBoss[7].bits = 8;
			gBoss[7].size = 3;
			gBoss[7].ani_no = 0;
			gBoss[9].cond = 0x80;
			gBoss[9].act_no = 0;
			gBoss[9].direct = 1;
			gBoss[9].x = 0xF8000;
			gBoss[9].y = 0x12000;
			gBoss[9].view.front = 0x4800;
			gBoss[9].view.top = 0x1000;
			gBoss[9].view.back = 0x4800;
			gBoss[9].view.bottom = 0x3000;
			gBoss[9].hit_voice = 52;
			gBoss[9].hit.front = 0x3800;
			gBoss[9].hit.top = 0x1000;
			gBoss[9].hit.back = 0x3800;
			gBoss[9].hit.bottom = 0x2000;
			gBoss[9].bits = 141;
			gBoss[9].size = 3;
			gBoss[10] = gBoss[9];
			gBoss[10].x = 0x108000;
			gBoss[11] = gBoss[9];
			gBoss[11].direct = 3;
			gBoss[11].x = 0xF8000;
			gBoss[11].y = 0x20000;
			gBoss[11].view.top = 0x3000;
			gBoss[11].view.bottom = 0x1000;
			gBoss[11].hit.top = 0x2000;
			gBoss[11].hit.bottom = 0x1000;
			gBoss[12] = gBoss[11];
			gBoss[12].x = 0x108000;
			gBoss[13] = gBoss[9];
			gBoss[13].cond = 0x80;
			gBoss[13].view.top = 0x2000;
			gBoss[13].view.bottom = 0x2000;
			gBoss[13].view.front = 0x3C00;
			gBoss[13].view.back = 0x5400;
			gBoss[13].count1 = 9;
			gBoss[13].ani_no = 0;
			gBoss[13].bits = 8;
			gBoss[14] = gBoss[13];
			gBoss[14].view.front = 0x5400;
			gBoss[14].view.back = 0x3C00;
			gBoss[14].count1 = 10;
			gBoss[14].ani_no = 1;
			gBoss[14].bits = 8;
			gBoss[15] = gBoss[13];
			gBoss[15].view.top = 0x2000;
			gBoss[15].view.bottom = 0x2000;
			gBoss[15].count1 = 11;
			gBoss[15].ani_no = 2;
			gBoss[15].bits = 8;
			gBoss[16] = gBoss[15];
			gBoss[16].view.front = 0x5400;
			gBoss[16].view.back = 0x3C00;
			gBoss[16].count1 = 12;
			gBoss[16].ani_no = 3;
			gBoss[16].bits = 8;
			gBoss[0].act_no = 2;
			break;

		case 10:
			gBoss[0].act_no = 11;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;
			// Fallthrough
		case 11:
			if (++gBoss[0].act_wait > 100)
			{
				gBoss[0].act_wait = 0;

				if (gMC.x < gBoss[0].x)
					gBoss[0].act_no = 100;
				else
					gBoss[0].act_no = 200;
			}

			break;

		case 100:
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 101;
			++gBoss[0].count1;
			// Fallthrough
		case 101:
			if (++gBoss[0].act_wait == 4)
				gBoss[9].act_no = 100;
			if (gBoss[0].act_wait == 8)
				gBoss[10].act_no = 100;
			if (gBoss[0].act_wait == 10)
				gBoss[11].act_no = 100;
			if (gBoss[0].act_wait == 12)
				gBoss[12].act_no = 100;
			if (gBoss[0].act_wait > 120 && gBoss[0].count1 > 2)
				gBoss[0].act_no = 300;
			if (gBoss[0].act_wait > 121 && gMC.x > gBoss[0].x)
				gBoss[0].act_no = 200;

			break;

		case 200:
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 201;
			++gBoss[0].count1;
			// Fallthrough
		case 201:
			if (++gBoss[0].act_wait == 4)
				gBoss[9].act_no = 200;
			if (gBoss[0].act_wait == 8)
				gBoss[10].act_no = 200;
			if (gBoss[0].act_wait == 10)
				gBoss[11].act_no = 200;
			if (gBoss[0].act_wait == 12)
				gBoss[12].act_no = 200;
			if (gBoss[0].act_wait > 120 && gBoss[0].count1 > 2)
				gBoss[0].act_no = 400;
			if (gBoss[0].act_wait > 121 && gMC.x < gBoss[0].x)
				gBoss[0].act_no = 100;

			break;

		case 300:
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 301;
			// Fallthrough
		case 301:
			if (++gBoss[0].act_wait == 4)
				gBoss[9].act_no = 300;
			if (gBoss[0].act_wait == 8)
				gBoss[10].act_no = 300;
			if (gBoss[0].act_wait == 10)
				gBoss[11].act_no = 300;
			if (gBoss[0].act_wait == 12)
				gBoss[12].act_no = 300;
			if (gBoss[0].act_wait > 50)
			{
				if (gBoss[3].cond || gBoss[4].cond || gBoss[5].cond || gBoss[6].cond)
					gBoss[0].act_no = 500;
				else
					gBoss[0].act_no = 600;
			}
			break;

		case 400:
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 401;
			// Fallthrough
		case 401:
			if (++gBoss[0].act_wait == 4)
				gBoss[9].act_no = 400;
			if (gBoss[0].act_wait == 8)
				gBoss[10].act_no = 400;
			if (gBoss[0].act_wait == 10)
				gBoss[11].act_no = 400;
			if (gBoss[0].act_wait == 12)
				gBoss[12].act_no = 400;
			if (gBoss[0].act_wait > 50)
			{
				if (gBoss[3].cond || gBoss[4].cond || gBoss[5].cond || gBoss[6].cond)
					gBoss[0].act_no = 500;
				else
					gBoss[0].act_no = 600;
			}

			break;

		case 500:
			gBoss[0].act_no = 501;
			gBoss[0].act_wait = 0;
			gBoss[1].act_no = 10;
			gBoss[2].act_no = 10;
			// Fallthrough
		case 501:
			if (++gBoss[0].act_wait > 300)
			{
				gBoss[0].act_no = 502;
				gBoss[0].act_wait = 0;
			}

			if (gBoss[3].cond == 0 && gBoss[4].cond == 0 && gBoss[5].cond == 0 && gBoss[6].cond == 0)
			{
				gBoss[0].act_no = 502;
				gBoss[0].act_wait = 0;
			}

			break;

		case 502:
			gBoss[0].act_no = 503;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;
			gBoss[1].act_no = 20;
			gBoss[2].act_no = 20;
			// Fallthrough
		case 503:
			if (++gBoss[0].act_wait > 50)
			{
				if (gMC.x < gBoss[0].x)
					gBoss[0].act_no = 100;
				else
					gBoss[0].act_no = 200;
			}

			break;

		case 600:
			gBoss[0].act_no = 601;
			gBoss[0].act_wait = 0;
			gBoss[0].count2 = gBoss[0].life;
			gBoss[1].act_no = 30;
			gBoss[2].act_no = 30;
			// Fallthrough
		case 601:
			++gBoss[0].act_wait;

			if (gBoss[0].life < gBoss[0].count2 - 200 || gBoss[0].act_wait > 300)
			{
				gBoss[0].act_no = 602;
				gBoss[0].act_wait = 0;
			}

			break;

		case 602:
			gBoss[0].act_no = 603;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;
			gBoss[1].act_no = 40;
			gBoss[2].act_no = 40;
			break;

		case 603:
			if (++gBoss[0].act_wait > 50)
			{
				if (gMC.x < gBoss[0].x)
					gBoss[0].act_no = 100;
				else
					gBoss[0].act_no = 200;
			}

			break;

		case 1000:
			SetQuake(2);

			if (++gBoss[0].act_wait % 8 == 0)
				PlaySoundObject(52, 1);

			SetDestroyNpChar(gBoss[0].x + (Random(-0x48, 0x48) * 0x200), gBoss[0].y + (Random(-0x40, 0x40) * 0x200), 1, 1);

			if (gBoss[0].act_wait > 100)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 1001;
				SetFlash(gBoss[0].x, gBoss[0].y, 1);
				PlaySoundObject(35, 1);
			}

			break;

		case 1001:
			SetQuake(40);

			if (++gBoss[0].act_wait > 50)
			{
				for (int i = 0; i < 20; ++i)
					gBoss[i].cond = 0;

				DeleteNpCharCode(158, 1);
				SetNpChar(159, gBoss[0].x, gBoss[0].y - 0x3000, 0, 0, 0, 0, 0);
			}

			break;
	}

	ActBossChar03_01(&gBoss[9]);
	ActBossChar03_01(&gBoss[10]);
	ActBossChar03_01(&gBoss[11]);
	ActBossChar03_01(&gBoss[12]);
	gBoss[0].x += ((gBoss[11].x + gBoss[10].x + gBoss[9].x + gBoss[12].x) / 4 - gBoss[0].x) / 16;
	ActBossChar03_face(&gBoss[7]);
	ActBossChar03_02(&gBoss[13]);
	ActBossChar03_02(&gBoss[14]);
	ActBossChar03_02(&gBoss[15]);
	ActBossChar03_02(&gBoss[16]);
	ActBossChar03_03(&gBoss[1]);
	ActBossChar03_03(&gBoss[2]);

	if (gBoss[3].cond)
		ActBossChar03_04(&gBoss[3]);
	if (gBoss[4].cond)
		ActBossChar03_04(&gBoss[4]);
	if (gBoss[5].cond)
		ActBossChar03_04(&gBoss[5]);
	if (gBoss[6].cond)
		ActBossChar03_04(&gBoss[6]);

	if (gBoss[0].life == 0 && gBoss[0].act_no < 1000)
	{
		gBoss[0].act_no = 1000;
		gBoss[0].act_wait = 0;
		gBoss[0].shock = 0x96;
		gBoss[9].act_no = 300;
		gBoss[10].act_no = 300;
		gBoss[11].act_no = 300;
		gBoss[12].act_no = 300;
	}
}