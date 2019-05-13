#include "BossBallos.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Flash.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "MycParam.h"
#include "NpChar.h"
#include "Sound.h"

static void ActBossChar_Eye(NPCHAR *npc)
{
	RECT rcLeft[5] = {
		{272, 0, 296, 16},
		{272, 16, 296, 32},
		{272, 32, 296, 48},
		{0, 0, 0, 0},
		{240, 16, 264, 32},
	};

	RECT rcRight[5] = {
		{296, 0, 320, 16},
		{296, 16, 320, 32},
		{296, 32, 320, 48},
		{0, 0, 0, 0},
		{240, 32, 264, 48},
	};

	switch (npc->act_no)
	{
		case 100:
			npc->act_no = 101;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 101:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
				npc->act_no = 102;

			break;

		case 102:
			npc->ani_no = 3;
			break;

		case 200:
			npc->act_no = 201;
			npc->ani_no = 3;
			npc->ani_wait = 0;
			// Fallthrough
		case 201:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				--npc->ani_no;
			}

			if (npc->ani_no <= 0)
				npc->act_no = 202;

			break;

		case 300:
			npc->act_no = 301;
			npc->ani_no = 4;

			if (npc->direct == 0)
				SetDestroyNpChar(npc->x - 0x800, npc->y, 0x800, 10);
			else
				SetDestroyNpChar(npc->x + 0x800, npc->y, 0x800, 10);

			break;
	}

	if (npc->direct == 0)
		npc->x = gBoss[0].x - 0x3000;
	else
		npc->x = gBoss[0].x + 0x3000;

	npc->y = gBoss[0].y - 0x4800;

	if (npc->act_no >= 0 && npc->act_no < 300)
	{
		if (npc->ani_no != 3)
			npc->bits &= ~0x20;
		else
			npc->bits |= 0x20;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

static void ActBossChar_Body(NPCHAR *npc)
{
	RECT rc[4] = {
		{0, 0, 120, 120},
		{120, 0, 240, 120},
		{0, 120, 120, 240},
		{120, 120, 240, 240},
	};

	npc->x = gBoss[0].x;
	npc->y = gBoss[0].y;

	npc->rect = rc[npc->ani_no];
}

static void ActBossChar_HITAI(NPCHAR *npc)
{
	npc->x = gBoss[0].x;
	npc->y = gBoss[0].y - 0x5800;
}

static void ActBossChar_HARA(NPCHAR *npc)
{
	npc->x = gBoss[0].x;
	npc->y = gBoss[0].y;
}

void ActBossChar_Ballos(void)
{
	static unsigned char flash;

	int i;
	int x;
	int y;

	NPCHAR *npc = gBoss;

	switch (npc->act_no)
	{
		case 0:
			// Initialize main boss
			npc->act_no = 1;
			npc->cond = 0x80;
			npc->exp = 1;
			npc->direct = 0;
			npc->x = 0x28000;
			npc->y = -0x8000;
			npc->hit_voice = 54;
			npc->hit.front = 0x4000;
			npc->hit.top = 0x6000;
			npc->hit.back = 0x4000;
			npc->hit.bottom = 0x6000;
			npc->bits = 0x8248;
			npc->size = 3;
			npc->damage = 0;
			npc->code_event = 1000;
			npc->life = 800;

			// Initialize eyes
			gBoss[1].cond = 0x90;
			gBoss[1].direct = 0;
			gBoss[1].bits = 8;
			gBoss[1].life = 10000;
			gBoss[1].view.front = 0x1800;
			gBoss[1].view.top = 0;
			gBoss[1].view.back = 0x1800;
			gBoss[1].view.bottom = 0x2000;
			gBoss[1].hit.front = 0x1800;
			gBoss[1].hit.top = 0;
			gBoss[1].hit.back = 0x1800;
			gBoss[1].hit.bottom = 0x2000;

			gBoss[2] = gBoss[1];
			gBoss[2].direct = 2;

			// Initialize the body
			gBoss[3].cond = 0x90;
			gBoss[3].bits = 0xD;
			gBoss[3].view.front = 0x7800;
			gBoss[3].view.top = 0x7800;
			gBoss[3].view.back = 0x7800;
			gBoss[3].view.bottom = 0x7800;
			gBoss[3].hit.front = 0x6000;
			gBoss[3].hit.top = 0x3000;
			gBoss[3].hit.back = 0x6000;
			gBoss[3].hit.bottom = 0x4000;

			gBoss[4].cond = 0x90;
			gBoss[4].bits = 0xD;
			gBoss[4].hit.front = 0x4000;
			gBoss[4].hit.top = 0x1000;
			gBoss[4].hit.back = 0x4000;
			gBoss[4].hit.bottom = 0x1000;

			gBoss[5].cond = 0x90;
			gBoss[5].bits = 0x4C;
			gBoss[5].hit.front = 0x4000;
			gBoss[5].hit.top = 0;
			gBoss[5].hit.back = 0x4000;
			gBoss[5].hit.bottom = 0x6000;
			break;

		case 100:
			npc->act_no = 101;
			npc->ani_no = 0;
			npc->x = gMC.x;
			SetNpChar(333, gMC.x, 0x26000, 0, 0, 2, 0, 0x100);
			npc->act_wait = 0;
			// Fallthrough
		case 101:
			if (++npc->act_wait > 30)
				npc->act_no = 102;

			break;

		case 102:
			npc->ym += 0x40;
			if (npc->ym > 0xC00)
				npc->ym = 0xC00;

			npc->y += npc->ym;

			if (npc->y > 0x26000 - npc->hit.bottom)
			{
				npc->y = 0x26000 - npc->hit.bottom;
				npc->ym = 0;
				npc->act_no = 103;
				npc->act_wait = 0;
				SetQuake2(30);
				PlaySoundObject(44, 1);

				if (gMC.y > npc->y + 0x6000 && gMC.x < npc->x + 0x3000 && gMC.x > npc->x - 0x3000)
					DamageMyChar(16);

				for (i = 0; i < 0x10; ++i)
				{
					x = npc->x + (Random(-40, 40) * 0x200);
					SetNpChar(4, x, npc->y + 0x5000, 0, 0, 0, 0, 0x100);
				}

				if (gMC.flag & 8)
					gMC.ym = -0x200;
			}

			break;

		case 103:
			if (++npc->act_wait == 50)
			{
				npc->act_no = 104;
				gBoss[1].act_no = 100;
				gBoss[2].act_no = 100;
			}

			break;

		case 200:
			npc->act_no = 201;
			npc->count1 = 0;
			// Fallthrough
		case 201:
			npc->act_no = 203;
			npc->xm = 0;
			++npc->count1;
			npc->hit.bottom = 0x6000;
			npc->damage = 0;

			if (npc->count1 % 3 == 0)
				npc->act_wait = 150;
			else
				npc->act_wait = 50;
			// Fallthrough
		case 203:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 204;
				npc->ym = -0xC00;

				if (npc->x < gMC.x)
					npc->xm = 0x200;
				else
					npc->xm = -0x200;
			}

			break;

		case 204:
			if (npc->x < 0xA000)
				npc->xm = 0x200;
			if (npc->x > 0x44000)
				npc->xm = -0x200;

			npc->ym += 0x55;
			if (npc->ym > 0xC00)
				npc->ym = 0xC00;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->y > 0x26000 - npc->hit.bottom)
			{
				npc->y = 0x26000 - npc->hit.bottom;
				npc->ym = 0;
				npc->act_no = 201;
				npc->act_wait = 0;

				if (gMC.y > npc->y + 0x7000)
					DamageMyChar(16);

				if (gMC.flag & 8)
					gMC.ym = -0x200;

				SetQuake2(30);
				PlaySoundObject(26, 1);
				SetNpChar(332, npc->x - 0x1800, npc->y + 0x6800, 0, 0, 0, 0, 0x100);
				SetNpChar(332, npc->x + 0x1800, npc->y + 0x6800, 0, 0, 2, 0, 0x100);
				PlaySoundObject(44, 1);

				for (i = 0; i < 0x10; ++i)
				{
					x = npc->x + (Random(-40, 40) * 0x200);
					SetNpChar(4, x, npc->y + 0x5000, 0, 0, 0, 0, 0x100);
				}
			}

			break;

		case 220:
			npc->act_no = 221;
			npc->life = 1200;
			gBoss[1].act_no = 200;
			gBoss[2].act_no = 200;
			npc->xm = 0;
			npc->ani_no = 0;
			npc->shock = 0;
			flash = 0;
			// Fallthrough
		case 221:
			npc->ym += 0x40;
			if (npc->ym > 0xC00)
				npc->ym = 0xC00;

			npc->y += npc->ym;

			if (npc->y > 0x26000 - npc->hit.bottom)
			{
				npc->y = 0x26000 - npc->hit.bottom;
				npc->ym = 0;
				npc->act_no = 222;
				npc->act_wait = 0;
				SetQuake2(30);
				PlaySoundObject(26, 1);

				for (i = 0; i < 0x10; ++i)
				{
					x = npc->x + (Random(-40, 40) * 0x200);
					SetNpChar(4, x, npc->y + 0x5000, 0, 0, 0, 0, 0x100);
				}

				if (gMC.flag & 8)
					gMC.ym = -0x200;
			}

			break;

		case 300:
			npc->act_no = 301;
			npc->act_wait = 0;

			for (i = 0; i < 0x100; i += 0x40)
			{
				SetNpChar(342, npc->x, npc->y, 0, 0, i, npc, 0x5A);
				SetNpChar(342, npc->x, npc->y, 0, 0, i + 544, npc, 0x5A);
			}

			SetNpChar(343, npc->x, npc->y, 0, 0, 0, npc, 0x18);
			SetNpChar(344, npc->x - 0x3000, npc->y - 0x4800, 0, 0, 0, npc, 0x20);
			SetNpChar(344, npc->x + 0x3000, npc->y - 0x4800, 0, 0, 2, npc, 0x20);
			// Fallthrough
		case 301:
			npc->y += (0x1C200 - npc->y) / 8;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 310;
				npc->act_wait = 0;
			}

			break;

		case 311:
			npc->direct = 0;
			npc->xm = -0x3AA;
			npc->ym = 0;
			npc->x += npc->xm;

			if (npc->x < 0xDE00)
			{
				npc->x = 0xDE00;
				npc->act_no = 312;
			}

			break;

		case 312:
			npc->direct = 1;
			npc->ym = -0x3AA;
			npc->xm = 0;
			npc->y += npc->ym;

			if (npc->y < 0xDE00)
			{
				npc->y = 0xDE00;
				npc->act_no = 313;
			}

			break;

		case 313:
			npc->direct = 2;
			npc->xm = 0x3AA;
			npc->ym = 0;
			npc->x += npc->xm;

			if (npc->x > 0x40200)
			{
				npc->x = 0x40200;
				npc->act_no = 314;
			}

			if (npc->count1)
				--npc->count1;

			if (npc->count1 == 0 && npc->x > 0x26000 && npc->x < 0x2A000)
				npc->act_no = 400;

			break;

		case 314:
			npc->direct = 3;
			npc->ym = 0x3AA;
			npc->xm = 0;
			npc->y += npc->ym;

			if (npc->y > 0x1C200)
			{
				npc->y = 0x1C200;
				npc->act_no = 311;
			}

			break;

		case 400:
			npc->act_no = 401;
			npc->act_wait = 0;
			npc->xm = 0;
			npc->ym = 0;
			DeleteNpCharCode(339, 0);
			// Fallthrough
		case 401:
			npc->y += (0x13E00 - npc->y) / 8;

			if (++npc->act_wait > 50)
			{
				npc->act_wait = 0;
				npc->act_no = 410;

				for (i = 0; i < 0x100; i += 0x20)
					SetNpChar(346, npc->x, npc->y, 0, 0, i, npc, 0x50);

				SetNpChar(343, npc->x, npc->y, 0, 0, 0, npc, 0x18);
				SetNpChar(344, npc->x - 0x3000, npc->y - 0x4800, 0, 0, 0, npc, 0x20);
				SetNpChar(344, npc->x + 0x3000, npc->y - 0x4800, 0, 0, 2, npc, 0x20);
			}

			break;

		case 410:
			if (++npc->act_wait > 50)
			{
				npc->act_wait = 0;
				npc->act_no = 411;
			}

			break;

		case 411:
			if (++npc->act_wait % 30 == 1)
			{
				x = (2 * (npc->act_wait / 30) + 2) * 0x10 * 0x200;
				SetNpChar(348, x, 0x2A000, 0, 0, 0, 0, 0x180);
			}

			if (npc->act_wait / 3 % 2)
				PlaySoundObject(26, 1);

			if (npc->act_wait > 540)
				npc->act_no = 420;

			break;

		case 420:
			npc->act_no = 421;
			npc->act_wait = 0;
			npc->ani_wait = 0;
			SetQuake2(30);
			PlaySoundObject(35, 1);
			gBoss[1].act_no = 102;
			gBoss[2].act_no = 102;

			for (i = 0; i < 0x100; ++i)
			{
				x = npc->x + (Random(-60, 60) * 0x200);
				y = npc->y + (Random(-60, 60) * 0x200);
				SetNpChar(4, x, y, 0, 0, 0, 0, 0);
			}
			// Fallthrough
		case 421:
			if (++npc->ani_wait > 500)
			{
				npc->ani_wait = 0;
				npc->act_no = 422;
			}

			break;

		case 422:
			if (++npc->ani_wait > 200)
			{
				npc->ani_wait = 0;
				npc->act_no = 423;
			}

			break;

		case 423:
			if (++npc->ani_wait > 20)
			{
				npc->ani_wait = 0;
				npc->act_no = 424;
			}

			break;

		case 424:
			if (++npc->ani_wait > 200)
			{
				npc->ani_wait = 0;
				npc->act_no = 425;
			}

			break;

		case 425:
			if (++npc->ani_wait > 500)
			{
				npc->ani_wait = 0;
				npc->act_no = 426;
			}

			break;

		case 426:
			if (++npc->ani_wait > 200)
			{
				npc->ani_wait = 0;
				npc->act_no = 427;
			}

			break;

		case 427:
			if (++npc->ani_wait > 20)
			{
				npc->ani_wait = 0;
				npc->act_no = 428;
			}

			break;

		case 428:
			if (++npc->ani_wait > 200)
			{
				npc->ani_wait = 0;
				npc->act_no = 421;
			}

			break;

		case 1000:
			npc->act_no = 1001;
			npc->act_wait = 0;

			gBoss[1].act_no = 300;
			gBoss[2].act_no = 300;

			gBoss[1].act_no &= ~0x41;
			gBoss[2].act_no &= ~0x41;

			gBoss[0].bits &= ~0x41;
			gBoss[3].bits &= ~0x41;
			gBoss[4].bits &= ~0x41;
			gBoss[5].bits &= ~0x41;
			// Fallthrough
		case 1001:
			++gBoss[0].act_wait;

			if (gBoss[0].act_wait % 12 == 0)
				PlaySoundObject(44, 1);

			SetDestroyNpChar(gBoss[0].x + (Random(-60, 60) * 0x200), gBoss[0].y + (Random(-60, 60) * 0x200), 1, 1);

			if (gBoss[0].act_wait > 150)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 1002;
				SetFlash(gBoss[0].x, gBoss[0].y, 1);
				PlaySoundObject(35, 1);
			}

			break;

		case 1002:
			SetQuake2(40);

			++gBoss[0].act_wait;

			if (gBoss[0].act_wait == 50)
			{
				gBoss[0].cond = 0;
				gBoss[1].cond = 0;
				gBoss[2].cond = 0;
				gBoss[3].cond = 0;
				gBoss[4].cond = 0;
				gBoss[5].cond = 0;
				DeleteNpCharCode(350, 1);
				DeleteNpCharCode(348, 1);
			}

			break;
	}

	if (npc->act_no > 420 && npc->act_no < 500)
	{
		gBoss[3].bits |= 0x20;
		gBoss[4].bits |= 0x20;
		gBoss[5].bits |= 0x20;

		if (++npc->act_wait > 300)
		{
			npc->act_wait = 0;

			if (gMC.x > npc->x)
			{
				for (i = 0; i < 8; ++i)
				{
					x = ((Random(-4, 4) + 156) * 0x200 * 0x10) / 4;
					y = (Random(8, 68) * 0x200 * 0x10) / 4;
					SetNpChar(350, x, y, 0, 0, 0, 0, 0x100);
				}
			}
			else
			{
				for (i = 0; i < 8; ++i)
				{
					x = (Random(-4, 4) * 0x200 * 0x10) / 4;
					y = (Random(8, 68) * 0x200 * 0x10) / 4;
					SetNpChar(350, x, y, 0, 0, 2, 0, 0x100);
				}
			}
		}

		if (npc->act_wait == 270 || npc->act_wait == 280 || npc->act_wait == 290)
		{
			SetNpChar(353, npc->x, npc->y - 0x6800, 0, 0, 1, 0, 0x100);
			PlaySoundObject(39, 1);

			for (i = 0; i < 4; ++i)
				SetNpChar(4, npc->x, npc->y - 0x6800, 0, 0, 0, 0, 0x100);
		}

		if (npc->life > 500)
		{
			if (Random(0, 10) == 2)
			{
				x = npc->x + (Random(-40, 40) * 0x200);
				y = npc->y + (Random(0, 40) * 0x200);
				SetNpChar(270, x, y, 0, 0, 3, 0, 0);
			}
		}
		else
		{
			if (Random(0, 4) == 2)
			{
				x = npc->x + (Random(-40, 40) * 0x200);
				y = npc->y + (Random(0, 40) * 0x200);
				SetNpChar(270, x, y, 0, 0, 3, 0, 0);
			}
		}
	}

	if (npc->shock)
	{
		if (++flash / 2 % 2)
			gBoss[3].ani_no = 1;
		else
			gBoss[3].ani_no = 0;
	}
	else
	{
		gBoss[3].ani_no = 0;
	}

	if (npc->act_no > 420)
		gBoss[3].ani_no += 2;

	ActBossChar_Eye(&gBoss[1]);
	ActBossChar_Eye(&gBoss[2]);
	ActBossChar_Body(&gBoss[3]);
	ActBossChar_HITAI(&gBoss[4]);
	ActBossChar_HARA(&gBoss[5]);
}
