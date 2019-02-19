#include "BossAlmo2.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Flash.h"
#include "Frame.h"
#include "Game.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

static void ActBossCharA_Head(NPCHAR *npc)
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

			break;

		case 100:
			npc->ani_no = 3;
			break;
	}

	npc->rect = rect[npc->ani_no];

	if (npc->act_no == 51)
		npc->rect.bottom = npc->act_wait + npc->rect.top;
}

static void ActBossCharA_Tail(NPCHAR *npc)
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

			break;

		case 100:
			npc->ani_no = 2;
			break;
	}

	npc->rect = rect[npc->ani_no];

	if (npc->act_no == 51)
		npc->rect.bottom = npc->act_wait + npc->rect.top;
}

static void ActBossCharA_Face(NPCHAR *npc)
{
	RECT rect[5] = {
		{0, 0, 0, 0},
		{160, 112, 232, 152},
		{160, 152, 232, 192},
		{160, 192, 232, 232},
		{248, 160, 320, 200},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->ani_no = 0;
			break;

		case 10:
			npc->ani_no = 1;
			break;

		case 20:
			npc->ani_no = 2;
			break;

		case 30:
			npc->act_no = 31;
			npc->ani_no = 3;
			npc->act_wait = 100;
			// Fallthrough
		case 31:
			if (++npc->act_wait > 300)
				npc->act_wait = 0;

			if (npc->act_wait > 250 && (npc->act_wait & 0xF) == 1)
				PlaySoundObject(26, 1);

			if (npc->act_wait > 250 && (npc->act_wait & 0xF) == 7)
			{
				SetNpChar(293, npc->x, npc->y, 0, 0, 0, 0, 0x80);
				PlaySoundObject(101, 1);
			}

			if (npc->act_wait == 200)
				PlaySoundObject(116, 1);

			if (npc->act_wait > 200 && npc->act_wait % 2)
				npc->ani_no = 4;
			else
				npc->ani_no = 3;

			break;
	}

	npc->view.back = 0x4800;
	npc->view.front = 0x4800;
	npc->view.top = 0x2800;

	npc->x = gBoss[0].x - 0x4800;
	npc->y = gBoss[0].y + 0x800;

	npc->bits = 8;

	npc->rect = rect[npc->ani_no];
}

static void ActBossCharA_Mini(NPCHAR *npc)
{
	RECT rect[3] = {
		{256, 0, 320, 40},
		{256, 40, 320, 80},
		{256, 80, 320, 120},
	};

	if (npc->cond)
	{
		npc->life = 1000;

		switch (npc->act_no)
		{
			case 0:
				npc->bits &= ~0x20;
				break;

			case 5:
				npc->ani_no = 0;
				npc->bits &= ~0x20;
				++npc->count2;
				npc->count2 &= 0xFF;
				break;

			case 10:
				npc->ani_no = 0;
				npc->bits &= ~0x20;
				npc->count2 += 2;
				npc->count2 &= 0xFF;
				break;

			case 20:
				npc->ani_no = 1;
				npc->bits &= ~0x20;
				npc->count2 += 2;
				npc->count2 &= 0xFF;
				break;

			case 30:
				npc->ani_no = 0;
				npc->bits &= ~0x20;
				npc->count2 += 4;
				npc->count2 &= 0xFF;
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

				if (npc->x > (gMap.width + 2) * 0x2000)
					npc->cond = 0;

				break;
		}

		if (npc->act_no < 50)
		{
			int deg;

			if (npc->count1)
				deg = npc->count2 + 0x80;
			else
				deg = npc->count2 + 0x180;

			npc->x = npc->pNpc->x + 0x30 * GetCos(deg / 2) - 0x1000;
			npc->y = npc->pNpc->y + 0x50 * GetSin(deg / 2);
		}

		npc->rect = rect[npc->ani_no];
	}
}

static void ActBossCharA_Hit(NPCHAR *npc)
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

void ActBossChar_Undead(void)
{
	static int life;
	static unsigned char flash;

	BOOL bShock = FALSE;

	switch (gBoss[0].act_no)
	{
		case 1:
			gBoss[0].act_no = 10;
			gBoss[0].exp = 1;
			gBoss[0].cond = 0x80;
			gBoss[0].bits = 0x800C;
			gBoss[0].life = 700;
			gBoss[0].hit_voice = 114;
			gBoss[0].x = 0x4A000;
			gBoss[0].y = 0xF000;
			gBoss[0].xm = 0;
			gBoss[0].ym = 0;
			gBoss[0].code_event = 1000;
			gBoss[0].bits |= 0x200;

			gBoss[3].cond = 0x80;
			gBoss[3].act_no = 0;

			gBoss[4].cond = 0x80;
			gBoss[4].act_no = 10;

			gBoss[5].cond = 0x80;
			gBoss[5].act_no = 10;

			gBoss[8].cond = 0x80;
			gBoss[8].bits = 8;
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
			gBoss[1].act_no = 0;
			gBoss[1].bits = 40;
			gBoss[1].life = 1000;
			gBoss[1].hit_voice = 54;
			gBoss[1].hit.back = 0x3000;
			gBoss[1].hit.top = 0x2000;
			gBoss[1].hit.bottom = 0x2000;
			gBoss[1].view.front = 0x4000;
			gBoss[1].view.top = 0x2800;
			gBoss[1].pNpc = gBoss;

			gBoss[2] = gBoss[1];
			gBoss[2].count2 = 0x80;

			gBoss[6] = gBoss[1];
			gBoss[6].count1 = 1;

			gBoss[7] = gBoss[1];
			gBoss[7].count1 = 1;
			gBoss[7].count2 = 0x80;

			life = gBoss[0].life;

			break;

		case 15:
			gBoss[0].act_no = 16;
			bShock = TRUE;
			gBoss[0].direct = 0;
			gBoss[3].act_no = 10;
			gBoss[4].ani_no = 0;
			break;

		case 20:
			gBoss[0].act_no = 210;
			bShock = TRUE;
			gBoss[0].direct = 0;
			gBoss[1].act_no = 5;
			gBoss[2].act_no = 5;
			gBoss[6].act_no = 5;
			gBoss[7].act_no = 5;
			break;

		case 200:
			gBoss[0].act_no = 201;
			gBoss[0].act_wait = 0;
			gBoss[3].act_no = 0;
			gBoss[4].ani_no = 2;
			gBoss[5].ani_no = 0;
			gBoss[8].bits &= ~4;
			gBoss[9].bits &= ~4;
			gBoss[10].bits &= ~4;
			gBoss[11].bits &= ~0x20;
			gSuperYpos = 0;
			CutNoise();
			bShock = TRUE;
			// Fallthrough
		case 201:
			++gBoss[0].act_wait;

			if ((gBoss[0].direct == 2 || gBoss[0].ani_no > 0 || gBoss[0].life < 200) && gBoss[0].act_wait > 200)
			{
				++gBoss[0].count1;
				PlaySoundObject(115, 1);

				if (gBoss[0].life < 200)
				{
					gBoss[0].act_no = 230;
				}
				else
				{
					if ( gBoss[0].count1 <= 2 )
						gBoss[0].act_no = 210;
					else
						gBoss[0].act_no = 220;
				}
			}

			break;

		case 210:
			gBoss[0].act_no = 211;
			gBoss[0].act_wait = 0;
			gBoss[3].act_no = 10;
			gBoss[8].bits |= 4;
			gBoss[9].bits |= 4;
			gBoss[10].bits |= 4;
			gBoss[11].bits |= 0x20;
			life = gBoss[0].life;
			bShock = TRUE;
			// Fallthrough
		case 211:
			++flash;

			if (gBoss[0].shock && (flash >> 1) & 1)
			{
				gBoss[4].ani_no = 1;
				gBoss[5].ani_no = 1;
			}
			else
			{
				gBoss[4].ani_no = 0;
				gBoss[5].ani_no = 0;
			}

			if (++gBoss[0].act_wait % 100 == 1)
			{
				gCurlyShoot_wait = Random(80, 100);
				gCurlyShoot_x = gBoss[11].x;
				gCurlyShoot_y = gBoss[11].y;
			}

			if (gBoss[0].act_wait < 300)
			{
				if (gBoss[0].act_wait % 120 == 1)
					SetNpChar(288, gBoss[0].x - 0x4000, gBoss[0].y - 0x2000, 0, 0, 1, 0, 0x20);

				if (gBoss[0].act_wait % 120 == 61)
					SetNpChar(288, gBoss[0].x - 0x4000, gBoss[0].y + 0x2000, 0, 0, 3, 0, 0x20);
			}

			if (gBoss[0].life < life - 50 || gBoss[0].act_wait > 400)
				gBoss[0].act_no = 200;

			break;

		case 220:
			gBoss[0].act_no = 221;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;
			gSuperYpos = 1;
			gBoss[3].act_no = 20;
			gBoss[8].bits |= 4;
			gBoss[9].bits |= 4;
			gBoss[10].bits |= 4;
			gBoss[11].bits |= 0x20;
			SetQuake(100);
			life = gBoss[0].life;
			bShock = TRUE;
			// Fallthrough
		case 221:
			if (++gBoss[0].act_wait % 40 == 1)
			{
				int x;
				int y;

				switch (Random(0, 3))
				{
					case 0:
						x = gBoss[1].x;
						y = gBoss[1].y;
						break;
					case 1:
						x = gBoss[2].x;
						y = gBoss[2].y;
						break;
					case 2:
						x = gBoss[6].x;
						y = gBoss[6].y;
						break;
					case 3:
						x = gBoss[7].x;
						y = gBoss[7].y;
						break;
				}

				PlaySoundObject(25, 1);
				SetNpChar(285, x - 0x2000, y, 0, 0, 0, 0, 0x100);
				SetNpChar(285, x - 0x2000, y, 0, 0, 0x400, 0, 0x100);
			}

			++flash;

			if (gBoss[0].shock && (flash >> 1) & 1)
			{
				gBoss[4].ani_no = 1;
				gBoss[5].ani_no = 1;
			}
			else
			{
				gBoss[4].ani_no = 0;
				gBoss[5].ani_no = 0;
			}

			if (gBoss[0].life < life - 150 || gBoss[0].act_wait > 400 || gBoss[0].life < 200)
				gBoss[0].act_no = 200;

			break;

		case 230:
			gBoss[0].act_no = 231;
			gBoss[0].act_wait = 0;
			gBoss[3].act_no = 30;
			gBoss[8].bits |= 4;
			gBoss[9].bits |= 4;
			gBoss[10].bits |= 4;
			gBoss[11].bits |= 0x20;
			PlaySoundObject(25, 1);
			SetNpChar(285, gBoss[3].x - 0x2000, gBoss[3].y, 0, 0, 0, 0, 0x100);
			SetNpChar(285, gBoss[3].x - 0x2000, gBoss[3].y, 0, 0, 0x400, 0, 0x100);
			SetNpChar(285, gBoss[3].x, gBoss[3].y - 0x2000, 0, 0, 0, 0, 0x100);
			SetNpChar(285, gBoss[3].x, gBoss[3].y - 0x2000, 0, 0, 0x400, 0, 0x100);
			SetNpChar(285, gBoss[3].x, gBoss[3].y + 0x2000, 0, 0, 0, 0, 0x100);
			SetNpChar(285, gBoss[3].x, gBoss[3].y + 0x2000, 0, 0, 0x400, 0, 0x100);
			life = gBoss[0].life;
			bShock = TRUE;
			// Fallthrough
		case 231:
			++flash;

			if (gBoss[0].shock && (flash >> 1) & 1)
			{
				gBoss[4].ani_no = 1;
				gBoss[5].ani_no = 1;
			}
			else
			{
				gBoss[4].ani_no = 0;
				gBoss[5].ani_no = 0;
			}

			if (++gBoss[0].act_wait % 100 == 1)
			{
				gCurlyShoot_wait = Random(80, 100);
				gCurlyShoot_x = gBoss[11].x;
				gCurlyShoot_y = gBoss[11].y;
			}

			if (gBoss[0].act_wait % 120 == 1)
				SetNpChar(288, gBoss[0].x - 0x4000, gBoss[0].y - 0x2000, 0, 0, 1, 0, 0x20);

			if (gBoss[0].act_wait % 120 == 61)
				SetNpChar(288, gBoss[0].x - 0x4000, gBoss[0].y + 0x2000, 0, 0, 3, 0, 0x20);

			break;

		case 500:
			CutNoise();
			gBoss[0].act_no = 501;
			gBoss[0].act_wait = 0;
			gBoss[0].xm = 0;
			gBoss[0].ym = 0;
			gBoss[3].act_no = 0;
			gBoss[4].ani_no = 2;
			gBoss[5].ani_no = 0;
			gBoss[1].act_no = 5;
			gBoss[2].act_no = 5;
			gBoss[6].act_no = 5;
			gBoss[7].act_no = 5;
			SetQuake(20);

			for (int i = 0; i < 100; ++i)
				SetNpChar(4, gBoss[0].x + (Random(-0x80, 0x80) * 0x200), gBoss[0].y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0);

			DeleteNpCharCode(282, 1);
			gBoss[11].bits &= ~0x20;

			for (int i = 0; i < 12; ++i)
				gBoss[i].bits &= ~4;
			// Fallthrough
		case 501:
			if (++gBoss[0].act_wait & 0xF)
				SetNpChar(4, gBoss[0].x + (Random(-0x40, 0x40) * 0x200), gBoss[0].y + (Random(-0x20, 0x20) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			gBoss[0].x += 0x40;
			gBoss[0].y += 0x80;

			if (gBoss[0].act_wait > 200)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 1000;
			}

			break;

		case 1000:
			SetQuake(100);

			if (++gBoss[0].act_wait % 8 == 0)
				PlaySoundObject(44, 1);

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
				DeleteNpCharCode(301, 1);
			}

			break;
	}

	if (bShock)
	{
		SetQuake(20);

		if (gBoss[0].act_no == 201)
		{
			gBoss[7].act_no = 10;
			gBoss[6].act_no = 10;
			gBoss[2].act_no = 10;
			gBoss[1].act_no = 10;
		}

		if (gBoss[0].act_no == 221)
		{
			gBoss[7].act_no = 20;
			gBoss[6].act_no = 20;
			gBoss[2].act_no = 20;
			gBoss[1].act_no = 20;
		}

		if (gBoss[0].act_no == 231)
		{
			gBoss[7].act_no = 30;
			gBoss[6].act_no = 30;
			gBoss[2].act_no = 30;
			gBoss[1].act_no = 30;
		}

		PlaySoundObject(26, 1);

		for (int i = 0; i < 8; ++i)
			SetNpChar(4, gBoss[4].x + (Random(-0x20, 0x10) * 0x200), gBoss[4].y, Random(-0x200, 0x200), Random(-0x100, 0x100), 0, 0, 0x100);
	}

	if (gBoss[0].act_no >= 200 && gBoss[0].act_no < 300)
	{
		if (gBoss[0].x < 0x18000)
			gBoss[0].direct = 2;
		if (gBoss[0].x > (gMap.width - 4) * 0x2000)
			gBoss[0].direct = 0;

		if (gBoss[0].direct == 0)
			gBoss[0].xm -= 4;
		else
			gBoss[0].xm += 4;
	}

	switch (gBoss[0].act_no)
	{
		case 201:
		case 211:
		case 221:
		case 231:
			if (++gBoss[0].count2 == 150)
			{
				gBoss[0].count2 = 0;
				SetNpChar(282, (gMap.width * 0x2000) + 0x40, (Random(-1, 3) + 10) * 0x2000, 0, 0, 0, 0, 0x30);
			}
			else if (gBoss[0].count2 == 75)
			{
				SetNpChar(282, (gMap.width * 0x2000) + 0x40, (Random(-3, 0) + 3) * 0x2000, 0, 0, 0, 0, 0x30);
			}

			break;
	}

	if (gBoss[0].xm > 0x80)
		gBoss[0].xm = 0x80;
	if (gBoss[0].xm < -0x80)
		gBoss[0].xm = -0x80;

	if (gBoss[0].ym > 0x80)
		gBoss[0].ym = 0x80;
	if (gBoss[0].ym < -0x80)
		gBoss[0].ym = -0x80;

	gBoss[0].x += gBoss[0].xm;
	gBoss[0].y += gBoss[0].ym;

	ActBossCharA_Face(&gBoss[3]);
	ActBossCharA_Head(&gBoss[4]);
	ActBossCharA_Tail(&gBoss[5]);
	ActBossCharA_Mini(&gBoss[1]);
	ActBossCharA_Mini(&gBoss[2]);
	ActBossCharA_Mini(&gBoss[6]);
	ActBossCharA_Mini(&gBoss[7]);
	ActBossCharA_Hit(&gBoss[8]);
	ActBossCharA_Hit(&gBoss[9]);
	ActBossCharA_Hit(&gBoss[10]);
	ActBossCharA_Hit(&gBoss[11]);
}
