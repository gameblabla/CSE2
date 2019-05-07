#include "BossAlmo1.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Frame.h"
#include "Game.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

static void ActBossChar_Core_Face(NPCHAR *npc)
{
	RECT rect[4] = {
		{0, 0, 72, 112},
		{0, 112, 72, 224},
		{160, 0, 232, 112},
		{0, 0, 0, 0},
	};

	switch (npc->act_no)
	{
		case 10:
			npc->act_no = 11;
			npc->ani_no = 2;
			npc->bits = 8;
			npc->view.front = 0x4800;
			npc->view.top = 0x7000;
			// Fallthrough
		case 11:
			npc->x = gBoss[0].x - 0x4800;
			npc->y = gBoss[0].y;
			break;

		case 50:
			npc->act_no = 51;
			npc->act_wait = 112;
			// Fallthrough
		case 51:
			if (--npc->act_wait == 0)
			{
				npc->act_no = 100;
				npc->ani_no = 3;
			}

			npc->x = gBoss[0].x - 0x4800;
			npc->y = gBoss[0].y;
			break;

		case 100:
			npc->ani_no = 3;
			break;
	}

	npc->rect = rect[npc->ani_no];

	if (npc->act_no == 51)
		npc->rect.bottom = npc->act_wait + npc->rect.top;
}

static void ActBossChar_Core_Tail(NPCHAR *npc)
{
	RECT rect[3] = {
		{72, 0, 160, 112},
		{72, 112, 160, 224},
		{0, 0, 0, 0},
	};

	switch (npc->act_no)
	{
		case 10:
			npc->act_no = 11;
			npc->ani_no = 0;
			npc->bits = 8;
			npc->view.front = 0x5800;
			npc->view.top = 0x7000;
			// Fallthrough
		case 11:
			npc->x = gBoss[0].x + 0x5800;
			npc->y = gBoss[0].y;
			break;

		case 50:
			npc->act_no = 51;
			npc->act_wait = 112;
			// Fallthrough
		case 51:
			if (--npc->act_wait == 0)
			{
				npc->act_no = 100;
				npc->ani_no = 2;
			}

			npc->x = gBoss[0].x + 0x5800;
			npc->y = gBoss[0].y;
			break;

		case 100:
			npc->ani_no = 2;
			break;
	}

	npc->rect = rect[npc->ani_no];

	if (npc->act_no == 51)
		npc->rect.bottom = npc->rect.top + npc->act_wait;
}

static void ActBossChar_Core_Mini(NPCHAR *npc)
{
	unsigned char deg;
	int ym;
	int xm;

	RECT rect[3] = {
		{256, 0, 320, 40},
		{256, 40, 320, 80},
		{256, 80, 320, 120},
	};

	npc->life = 1000;

	switch (npc->act_no)
	{
		case 10:
			npc->ani_no = 2;
			npc->bits &= ~0x20;
			break;

		case 100:
			npc->act_no = 101;
			npc->ani_no = 2;
			npc->act_wait = 0;
			npc->tgt_x = gBoss[0].x + (Random(-0x80, 0x20) * 0x200);
			npc->tgt_y = gBoss[0].y + (Random(-0x40, 0x40) * 0x200);
			npc->bits |= 0x20;
			// Fallthrough
		case 101:
			npc->x += (npc->tgt_x - npc->x) / 16;
			npc->y += (npc->tgt_y - npc->y) / 16;

			if (++npc->act_wait > 50)
				npc->ani_no = 0;

			break;

		case 120:
			npc->act_no = 121;
			npc->act_wait = 0;
			// Fallthrough
		case 121:
			if (++npc->act_wait / 2 % 2)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (npc->act_wait > 20)
				npc->act_no = 130;

			break;

		case 130:
			npc->act_no = 131;
			npc->ani_no = 2;
			npc->act_wait = 0;
			npc->tgt_x = npc->x + (Random(0x18, 0x30) * 0x200);
			npc->tgt_y = npc->y + (Random(-4, 4) * 0x200);
			// Fallthrough
		case 131:
			npc->x += (npc->tgt_x - npc->x) / 16;
			npc->y += (npc->tgt_y - npc->y) / 16;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 140;
				npc->ani_no = 0;
			}

			if (npc->act_wait == 1 || npc->act_wait == 3)
			{
				deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
				deg += (unsigned char)Random(-2, 2);
				ym = 2 * GetSin(deg);
				xm = 2 * GetCos(deg);
				SetNpChar(178, npc->x, npc->y, xm, ym, 0, 0, 0x100);
				PlaySoundObject(39, 1);
			}

			break;

		case 140:
			npc->x += (npc->tgt_x - npc->x) / 16;
			npc->y += (npc->tgt_y - npc->y) / 16;
			break;

		case 200:
			npc->act_no = 201;
			npc->ani_no = 2;
			npc->xm = 0;
			npc->ym = 0;
			// Fallthrough
		case 201:
			npc->xm += 0x20;
			npc->x += npc->xm;

			if (npc->x > (gMap.width * 0x200 * 0x10) + (2 * 0x200 * 0x10))
				npc->cond = 0;

			break;
	}

	if (npc->shock)
		npc->tgt_x += 0x400;

	npc->rect = rect[npc->ani_no];
}

static void ActBossChar_Core_Hit(NPCHAR *npc)
{
	switch (npc->count1)
	{
		case 0:
			npc->x = gBoss[0].x;
			npc->y = gBoss[0].y - 0x4000;
			break;
		case 1:
			npc->x = gBoss[0].x + 0x3800;
			npc->y = gBoss[0].y;
			break;
		case 2:
			npc->x = gBoss[0].x + 0x800;
			npc->y = gBoss[0].y + 0x4000;
			break;
		case 3:
			npc->x = gBoss[0].x - 0x3800;
			npc->y = gBoss[0].y + 0x800;
			break;
	}
}

void ActBossChar_Core(void)
{
	unsigned char deg;
	int ym;
	int xm;
	int i;

	static unsigned char flash;

	NPCHAR *npc = gBoss;

	BOOL bShock = FALSE;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;
			npc->exp = 1;
			npc->cond = 0x80;
			npc->bits = 0x800C;
			npc->life = 650;
			npc->hit_voice = 114;
			npc->x = 0x9A000;
			npc->y = 0x1C000;
			npc->xm = 0;
			npc->ym = 0;
			npc->code_event = 1000;
			npc->bits |= 0x200;

			gBoss[4].cond = 0x80;
			gBoss[4].act_no = 10;

			gBoss[5].cond = 0x80;
			gBoss[5].act_no = 10;

			gBoss[8].cond = 0x80;
			gBoss[8].bits = 12;
			gBoss[8].view.front = 0;
			gBoss[8].view.top = 0;
			gBoss[8].hit.back = 0x5000;
			gBoss[8].hit.top = 0x2000;
			gBoss[8].hit.bottom = 0x2000;
			gBoss[8].count1 = 0;

			gBoss[9] = gBoss[8];
			gBoss[9].hit.back = 0x4800;
			gBoss[9].hit.top = 0x3000;
			gBoss[9].hit.bottom = 0x3000;
			gBoss[9].count1 = 1;

			gBoss[10] = gBoss[8];
			gBoss[10].hit.back = 0x5800;
			gBoss[10].hit.top = 0x1000;
			gBoss[10].hit.bottom = 0x1000;
			gBoss[10].count1 = 2;

			gBoss[11] = gBoss[8];
			gBoss[11].cond |= 0x10;
			gBoss[11].hit.back = 0x2800;
			gBoss[11].hit.top = 0x2800;
			gBoss[11].hit.bottom = 0x2800;
			gBoss[11].count1 = 3;

			gBoss[1].cond = 0x80;
			gBoss[1].act_no = 10;
			gBoss[1].bits = 44;
			gBoss[1].life = 1000;
			gBoss[1].hit_voice = 54;
			gBoss[1].hit.back = 0x3000;
			gBoss[1].hit.top = 0x2000;
			gBoss[1].hit.bottom = 0x2000;
			gBoss[1].view.front = 0x4000;
			gBoss[1].view.top = 0x2800;
			gBoss[1].x = npc->x - 0x1000;
			gBoss[1].y = npc->y - 0x8000;

			gBoss[2] = gBoss[1];
			gBoss[2].x = npc->x + 0x2000;
			gBoss[2].y = npc->y;

			gBoss[3] = gBoss[1];
			gBoss[3].x = npc->x - 0x1000;
			gBoss[3].y = npc->y + 0x8000;

			gBoss[6] = gBoss[1];
			gBoss[6].x = npc->x - 0x6000;
			gBoss[6].y = npc->y - 0x4000;

			gBoss[7] = gBoss[1];
			gBoss[7].x = npc->x - 0x6000;
			gBoss[7].y = npc->y + 0x4000;
			break;

		case 200:
			npc->act_no = 201;
			npc->act_wait = 0;
			gBoss[11].bits &= ~0x20;
			gSuperYpos = 0;
			CutNoise();
			// Fallthrough
		case 201:
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y;

			if (++npc->act_wait > 400)
			{
				++npc->count1;
				PlaySoundObject(115, 1);

				if (npc->count1 > 3)
				{
					npc->count1 = 0;
					npc->act_no = 220;
					gBoss[4].ani_no = 0;
					gBoss[5].ani_no = 0;
					bShock = TRUE;
				}
				else
				{
					npc->act_no = 210;
					gBoss[4].ani_no = 0;
					gBoss[5].ani_no = 0;
					bShock = TRUE;
				}
			}

			break;

		case 210:
			npc->act_no = 211;
			npc->act_wait = 0;
			npc->count2 = npc->life;
			gBoss[11].bits |= 0x20;
			// Fallthrough
		case 211:
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y;

			if (npc->shock)
			{
				if (++flash / 2 % 2)
				{
					gBoss[4].ani_no = 0;
					gBoss[5].ani_no = 0;
				}
				else
				{
					gBoss[4].ani_no = 1;
					gBoss[5].ani_no = 1;
				}
			}
			else
			{
				gBoss[4].ani_no = 0;
				gBoss[5].ani_no = 0;
			}

			if (++npc->act_wait % 100 == 1)
			{
				gCurlyShoot_wait = Random(80, 100);
				gCurlyShoot_x = gBoss[11].x;
				gCurlyShoot_y = gBoss[11].y;
			}

			if (npc->act_wait < 200 && npc->act_wait % 20 == 1)
				SetNpChar(179, npc->x + (Random(-0x30, -0x10) * 0x200), npc->y + (Random(-0x40, 0x40) * 0x200), 0, 0, 0, 0, 0x100);

			if (npc->act_wait > 400 || npc->life < npc->count2 - 200)
			{
				npc->act_no = 200;
				gBoss[4].ani_no = 2;
				gBoss[5].ani_no = 0;
				bShock = TRUE;
			}

			break;

		case 220:
			npc->act_no = 221;
			npc->act_wait = 0;
			gSuperYpos = 1;
			gBoss[11].bits |= 0x20u;
			SetQuake(100);
			SetNoise(1, 1000);
			// Fallthrough
		case 221:
			++npc->act_wait;
			SetNpChar(199, gMC.x + (Random(-50, 150) * 0x200 * 2), gMC.y + (Random(-0xA0, 0xA0) * 0x200), 0, 0, 0, 0, 0x100);
			gMC.xm -= 0x20;
			gMC.cond |= 0x20;

			if (npc->shock)
			{
				if (++flash / 2 % 2)
				{
					gBoss[4].ani_no = 0;
					gBoss[5].ani_no = 0;
				}
				else
				{
					gBoss[4].ani_no = 1;
					gBoss[5].ani_no = 1;
				}
			}
			else
			{
				gBoss[4].ani_no = 0;
				gBoss[5].ani_no = 0;
			}

			if (npc->act_wait == 300 || npc->act_wait == 350 || npc->act_wait == 400)
			{
				deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
				ym = 3 * GetSin(deg);
				xm = 3 * GetCos(deg);
				SetNpChar(218, npc->x - 0x5000, npc->y, xm, ym, 0, 0, 0x100);
				PlaySoundObject(101, 1);
			}

			if (npc->act_wait > 400)
			{
				npc->act_no = 200;
				gBoss[4].ani_no = 2;
				gBoss[5].ani_no = 0;
				bShock = TRUE;
			}

			break;

		case 500:
			CutNoise();

			npc->act_no = 501;
			npc->act_wait = 0;
			npc->xm = 0;
			npc->ym = 0;
			gBoss[4].ani_no = 2;
			gBoss[5].ani_no = 0;
			gBoss[1].act_no = 200;
			gBoss[2].act_no = 200;
			gBoss[3].act_no = 200;
			gBoss[6].act_no = 200;
			gBoss[7].act_no = 200;

			SetQuake(20);

			for (i = 0; i < 0x20; ++i)
				SetNpChar(4, npc->x + (Random(-0x80, 0x80) * 0x200), npc->y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			for (i = 0; i < 12; ++i)
				gBoss[i].bits &= ~0x24;

			// Fallthrough
		case 501:
			if (++npc->act_wait % 16)
				SetNpChar(4, npc->x + (Random(-0x40, 0x40) * 0x200), npc->y + (Random(-0x20, 0x20) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			if (npc->act_wait / 2 % 2)
				npc->x -= 0x200;
			else
				npc->x += 0x200;

			if (npc->x < 0x7E000)
				npc->x += 0x80;
			else
				npc->x -= 0x80;

			if (npc->y < 0x16000)
				npc->y += 0x80;
			else
				npc->y -= 0x80;

			break;

		case 600:
			npc->act_no = 601;
			gBoss[4].act_no = 50;
			gBoss[5].act_no = 50;
			gBoss[8].bits &= ~4;
			gBoss[9].bits &= ~4;
			gBoss[10].bits &= ~4;
			gBoss[11].bits &= ~4;
			// Fallthrough
		case 601:
			if (++npc->act_wait / 2 % 2)
				npc->x -= 0x800;
			else
				npc->x += 0x800;

			break;
	}

	if (bShock)
	{
		SetQuake(20);

		gBoss[1].act_no = 100;
		gBoss[2].act_no = 100;
		gBoss[3].act_no = 100;
		gBoss[6].act_no = 100;
		gBoss[7].act_no = 100;

		PlaySoundObject(26, 1);

		for (i = 0; i < 8; ++i)
			SetNpChar(4, gBoss[4].x + (Random(-0x20, 0x10) * 0x200), gBoss[4].y, Random(-0x200, 0x200), Random(-0x100, 0x100), 0, 0, 0x100);
	}

	if (npc->act_no >= 200 && npc->act_no < 300)
	{
		switch (npc->act_wait)
		{
			case 80:
				gBoss[1].act_no = 120;
				break;
			case 110:
				gBoss[2].act_no = 120;
				break;
			case 140:
				gBoss[3].act_no = 120;
				break;
			case 170:
				gBoss[6].act_no = 120;
				break;
			case 200:
				gBoss[7].act_no = 120;
				break;
		}

		if (npc->x < npc->tgt_x + 0x14000)
			npc->xm += 4;
		if (npc->x > npc->tgt_x + 0x14000)
			npc->xm -= 4;

		if (npc->y < npc->tgt_y)
			npc->ym += 4;
		if (npc->y > npc->tgt_y)
			npc->ym -= 4;
	}

	if (npc->xm > 0x80)
		npc->xm = 0x80;
	if (npc->xm < -0x80)
		npc->xm = -0x80;

	if (npc->ym > 0x80)
		npc->ym = 0x80;
	if (npc->ym < -0x80)
		npc->ym = -0x80;

	npc->x += npc->xm;
	npc->y += npc->ym;

	ActBossChar_Core_Face(&gBoss[4]);

	ActBossChar_Core_Tail(&gBoss[5]);

	ActBossChar_Core_Mini(&gBoss[1]);
	ActBossChar_Core_Mini(&gBoss[2]);
	ActBossChar_Core_Mini(&gBoss[3]);
	ActBossChar_Core_Mini(&gBoss[6]);
	ActBossChar_Core_Mini(&gBoss[7]);

	ActBossChar_Core_Hit(&gBoss[8]);
	ActBossChar_Core_Hit(&gBoss[9]);
	ActBossChar_Core_Hit(&gBoss[10]);
	ActBossChar_Core_Hit(&gBoss[11]);
}
