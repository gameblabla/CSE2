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

			if ( npc->ani_no <= 0 )
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
		if (npc->ani_no == 3)
			npc->bits |= 0x20;
		else
			npc->bits &= ~0x20;
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

	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss[0].act_no = 1;
			gBoss[0].cond = 0x80;
			gBoss[0].exp = 1;
			gBoss[0].direct = 0;
			gBoss[0].x = 0x28000;
			gBoss[0].y = -0x8000;
			gBoss[0].hit_voice = 54;
			gBoss[0].hit.front = 0x4000;
			gBoss[0].hit.top = 0x6000;
			gBoss[0].hit.back = 0x4000;
			gBoss[0].hit.bottom = 0x6000;
			gBoss[0].bits = 0x8248;
			gBoss[0].size = 3;
			gBoss[0].damage = 0;
			gBoss[0].code_event = 1000;
			gBoss[0].life = 800;

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
			gBoss[0].act_no = 101;
			gBoss[0].ani_no = 0;
			gBoss[0].x = gMC.x;
			SetNpChar(333, gMC.x, 0x26000, 0, 0, 2, 0, 0x100);
			gBoss[0].act_wait = 0;
			// Fallthrough
		case 101:
			if (++gBoss[0].act_wait > 30)
				gBoss[0].act_no = 102;

			break;

		case 102:
			gBoss[0].ym += 0x40;
			if (gBoss[0].ym > 0xC00)
				gBoss[0].ym = 0xC00;

			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].y > 0x26000 - gBoss[0].hit.bottom)
			{
				gBoss[0].y = 0x26000 - gBoss[0].hit.bottom;
				gBoss[0].ym = 0;
				gBoss[0].act_no = 103;
				gBoss[0].act_wait = 0;
				SetQuake2(30);
				PlaySoundObject(44, 1);

				if ( gMC.y > gBoss[0].y + 0x6000 && gMC.x < gBoss[0].x + 0x3000 && gMC.x > gBoss[0].x - 0x3000 )
					DamageMyChar(16);

				for (int i = 0; i < 0x10; ++i)
					SetNpChar(4, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + 0x5000, 0, 0, 0, 0, 0x100);

				if (gMC.flag & 8)
					gMC.ym = -0x200;
			}

			break;

		case 103:
			if (++gBoss[0].act_wait == 50)
			{
				gBoss[0].act_no = 104;
				gBoss[1].act_no = 100;
				gBoss[2].act_no = 100;
			}

			break;

		case 200:
			gBoss[0].act_no = 201;
			gBoss[0].count1 = 0;
			// Fallthrough
		case 201:
			gBoss[0].act_no = 203;
			gBoss[0].xm = 0;
			++gBoss[0].count1;
			gBoss[0].hit.bottom = 0x6000;
			gBoss[0].damage = 0;

			if (gBoss[0].count1 % 3)
				gBoss[0].act_wait = 50;
			else
				gBoss[0].act_wait = 150;
			// Fallthrough
		case 203:
			if (--gBoss[0].act_wait <= 0)
			{
				gBoss[0].act_no = 204;
				gBoss[0].ym = -0xC00;

				if (gMC.x > gBoss[0].x)
					gBoss[0].xm = 0x200;
				else
					gBoss[0].xm = -0x200;
			}

			break;

		case 204:
			if (gBoss[0].x < 0xA000)
				gBoss[0].xm = 0x200;
			if (gBoss[0].x > 0x44000)
				gBoss[0].xm = -0x200;

			gBoss[0].ym += 0x55;
			if (gBoss[0].ym > 0xC00)
				gBoss[0].ym = 0xC00;

			gBoss[0].x += gBoss[0].xm;
			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].y > 0x26000 - gBoss[0].hit.bottom)
			{
				gBoss[0].y = 0x26000 - gBoss[0].hit.bottom;
				gBoss[0].ym = 0;
				gBoss[0].act_no = 201;
				gBoss[0].act_wait = 0;

				if (gMC.y > gBoss[0].y + 0x7000)
					DamageMyChar(16);

				if (gMC.flag & 8)
					gMC.ym = -0x200;

				SetQuake2(30);
				PlaySoundObject(26, 1);
				SetNpChar(332, gBoss[0].x - 0x1800, gBoss[0].y + 0x6800, 0, 0, 0, 0, 0x100);
				SetNpChar(332, gBoss[0].x + 0x1800, gBoss[0].y + 0x6800, 0, 0, 2, 0, 0x100);
				PlaySoundObject(44, 1);

				for (int i = 0; i < 0x10; ++i)
					SetNpChar(4, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + 0x5000, 0, 0, 0, 0, 0x100);
			}

			break;

		case 220:
			gBoss[0].act_no = 221;
			gBoss[0].life = 1200;
			gBoss[1].act_no = 200;
			gBoss[2].act_no = 200;
			gBoss[0].xm = 0;
			gBoss[0].ani_no = 0;
			gBoss[0].shock = 0;
			flash = 0;
			// Fallthrough
		case 221:
			gBoss[0].ym += 0x40;
			if (gBoss[0].ym > 0xC00)
				gBoss[0].ym = 0xC00;

			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].y > 0x26000 - gBoss[0].hit.bottom)
			{
				gBoss[0].y = 0x26000 - gBoss[0].hit.bottom;
				gBoss[0].ym = 0;
				gBoss[0].act_no = 222;
				gBoss[0].act_wait = 0;
				SetQuake2(30);
				PlaySoundObject(26, 1);

				for (int i = 0; i < 0x10; ++i)
					SetNpChar(4, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + 0x5000, 0, 0, 0, 0, 0x100);

				if (gMC.flag & 8)
					gMC.ym = -0x200;
			}

			break;

		case 300:
			gBoss[0].act_no = 301;
			gBoss[0].act_wait = 0;

			for (int i = 0; i < 0x100; i += 0x40)
			{
				SetNpChar(342, gBoss[0].x, gBoss[0].y, 0, 0, i, gBoss, 0x5A);
				SetNpChar(342, gBoss[0].x, gBoss[0].y, 0, 0, i + 544, gBoss, 0x5A);
			}

			SetNpChar(343, gBoss[0].x, gBoss[0].y, 0, 0, 0, gBoss, 0x18);
			SetNpChar(344, gBoss[0].x - 0x3000, gBoss[0].y - 0x4800, 0, 0, 0, gBoss, 0x20);
			SetNpChar(344, gBoss[0].x + 0x3000, gBoss[0].y - 0x4800, 0, 0, 2, gBoss, 0x20);
			// Fallthrough
		case 301:
			gBoss[0].y += (0x1C200 - gBoss[0].y) / 8;

			if (++gBoss[0].act_wait > 50)
			{
				gBoss[0].act_no = 310;
				gBoss[0].act_wait = 0;
			}

			break;

		case 311:
			gBoss[0].direct = 0;
			gBoss[0].xm = -0x3AA;
			gBoss[0].ym = 0;
			gBoss[0].x += gBoss[0].xm;

			if (gBoss[0].x < 0xDE00)
			{
				gBoss[0].x = 0xDE00;
				gBoss[0].act_no = 312;
			}

			break;

		case 312:
			gBoss[0].direct = 1;
			gBoss[0].ym = -0x3AA;
			gBoss[0].xm = 0;
			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].y < 0xDE00)
			{
				gBoss[0].y = 0xDE00;
				gBoss[0].act_no = 313;
			}

			break;

		case 313:
			gBoss[0].direct = 2;
			gBoss[0].xm = 0x3AA;
			gBoss[0].ym = 0;
			gBoss[0].x += gBoss[0].xm;

			if (gBoss[0].x > 0x40200)
			{
				gBoss[0].x = 0x40200;
				gBoss[0].act_no = 314;
			}

			if (gBoss[0].count1)
				--gBoss[0].count1;

			if (gBoss[0].count1 == 0 && gBoss[0].x > 0x26000 && gBoss[0].x < 0x2A000)
				gBoss[0].act_no = 400;

			break;

		case 314:
			gBoss[0].direct = 3;
			gBoss[0].ym = 0x3AA;
			gBoss[0].xm = 0;
			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].y > 0x1C200)
			{
				gBoss[0].y = 0x1C200;
				gBoss[0].act_no = 311;
			}

			break;

		case 400:
			gBoss[0].act_no = 401;
			gBoss[0].act_wait = 0;
			gBoss[0].xm = 0;
			gBoss[0].ym = 0;
			DeleteNpCharCode(339, 0);
			// Fallthrough
		case 401:
			gBoss[0].y += (0x13E00 - gBoss[0].y) / 8;

			if (++gBoss[0].act_wait > 50)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 410;

				for (int i = 0; i < 0x100; i += 0x20)
					SetNpChar(346, gBoss[0].x, gBoss[0].y, 0, 0, i, gBoss, 0x50);

				SetNpChar(343, gBoss[0].x, gBoss[0].y, 0, 0, 0, gBoss, 0x18);
				SetNpChar(344, gBoss[0].x - 0x3000, gBoss[0].y - 0x4800, 0, 0, 0, gBoss, 0x20);
				SetNpChar(344, gBoss[0].x + 0x3000, gBoss[0].y - 0x4800, 0, 0, 2, gBoss, 0x20);
			}

			break;

		case 410:
			if (++gBoss[0].act_wait > 50)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 411;
			}

			break;

		case 411:
			if (++gBoss[0].act_wait % 30 == 1)
				SetNpChar(348, (2 * (gBoss[0].act_wait / 30) + 2) << 13, 0x2A000, 0, 0, 0, 0, 0x180);

			if (gBoss[0].act_wait / 3 & 1)
				PlaySoundObject(26, 1);

			if (gBoss[0].act_wait > 540)
				gBoss[0].act_no = 420;

			break;

		case 420:
			gBoss[0].act_no = 421;
			gBoss[0].act_wait = 0;
			gBoss[0].ani_wait = 0;
			SetQuake2(30);
			PlaySoundObject(35, 1);
			gBoss[1].act_no = 102;
			gBoss[2].act_no = 102;

			for (int i = 0; i < 0x100; ++i)
				SetNpChar(4, gBoss[0].x + (Random(-60, 60) * 0x200), gBoss[0].y + (Random(-60, 60) * 0x200), 0, 0, 0, 0, 0);
			// Fallthrough
		case 421:
			if (++gBoss[0].ani_wait > 500)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 422;
			}

			break;

		case 422:
			if (++gBoss[0].ani_wait > 200)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 423;
			}

			break;

		case 423:
			if (++gBoss[0].ani_wait > 20)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 424;
			}

			break;

		case 424:
			if (++gBoss[0].ani_wait > 200)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 425;
			}

			break;

		case 425:
			if (++gBoss[0].ani_wait > 500)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 426;
			}

			break;

		case 426:
			if (++gBoss[0].ani_wait > 200)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 427;
			}

			break;

		case 427:
			if (++gBoss[0].ani_wait > 20)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 428;
			}

			break;

		case 428:
			if (++gBoss[0].ani_wait > 200)
			{
				gBoss[0].ani_wait = 0;
				gBoss[0].act_no = 421;
			}

			break;

		case 1000:
			gBoss[0].act_no = 1001;
			gBoss[0].act_wait = 0;
			gBoss[1].act_no = 300;
			gBoss[2].act_no = 300;
			gBoss[0].bits &= ~0x41;
			gBoss[3].bits &= ~0x41;
			gBoss[4].bits &= ~0x41;
			gBoss[5].bits &= ~0x41;
			// Fallthrough
		case 1001:
			if (++gBoss[0].act_wait % 12 == 0)
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

			if (++gBoss[0].act_wait == 50)
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

	if (gBoss[0].act_no > 420 && gBoss[0].act_no < 500)
	{
		gBoss[3].bits |= 0x20;
		gBoss[4].bits |= 0x20;
		gBoss[5].bits |= 0x20;

		if (++gBoss[0].act_wait > 300)
		{
			gBoss[0].act_wait = 0;

			if (gMC.x > gBoss[0].x)
			{
				for (int i = 0; i < 8; ++i)
					SetNpChar(350, ((Random(-4, 4) + 156) * 0x2000) / 4, (Random(8, 68) * 0x2000) / 4, 0, 0, 0, 0, 0x100);
			}
			else
			{
				for (int i = 0; i < 8; ++i)
					SetNpChar(350, (Random(-4, 4) * 0x2000) / 4, (Random(8, 68) * 0x2000) / 4, 0, 0, 2, 0, 0x100);
			}
		}

		if (gBoss[0].act_wait == 270 || gBoss[0].act_wait == 280 || gBoss[0].act_wait == 290)
		{
			SetNpChar(353, gBoss[0].x, gBoss[0].y - 0x6800, 0, 0, 1, 0, 0x100);
			PlaySoundObject(39, 1);

			for (int i = 0; i < 4; ++i)
				SetNpChar(4, gBoss[0].x, gBoss[0].y - 0x6800, 0, 0, 0, 0, 0x100);
		}

		if (gBoss[0].life > 500)
		{
			if (Random(0, 10) == 2)
				SetNpChar(270, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + (Random(0, 40) * 0x200), 0, 0, 3, 0, 0);
		}
		else
		{
			if (Random(0, 4) == 2)
				SetNpChar(270, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + (Random(0, 40) * 0x200), 0, 0, 3, 0, 0);
		}
	}

	if (gBoss[0].shock)
	{
		if ((++flash >> 1) & 1)
			gBoss[3].ani_no = 0;
		else
			gBoss[3].ani_no = 1;
	}
	else
	{
		gBoss[3].ani_no = 0;
	}

	if (gBoss[0].act_no > 420)
		gBoss[3].ani_no += 2;

	ActBossChar_Eye(&gBoss[1]);
	ActBossChar_Eye(&gBoss[2]);
	ActBossChar_Body(&gBoss[3]);
	ActBossChar_HITAI(&gBoss[4]);
	ActBossChar_HARA(&gBoss[5]);
}