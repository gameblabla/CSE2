#include "BossPress.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Game.h"
#include "Map.h"
#include "NpChar.h"
#include "Sound.h"

void ActBossChar_Press(void)
{
	static unsigned char flash;

	int i;
	int x;

	NPCHAR *npc = gBoss;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;
			npc->cond = 0x80;
			npc->exp = 1;
			npc->direct = 2;
			npc->x = 0;
			npc->y = 0;
			npc->view.front = 0x5000;
			npc->view.top = 0x7800;
			npc->view.back = 0x5000;
			npc->view.bottom = 0x7800;
			npc->hit_voice = 54;
			npc->hit.front = 0x6200;
			npc->hit.top = 0x7800;
			npc->hit.back = 0x5000;
			npc->hit.bottom = 0x6000;
			npc->bits = 0x8248;
			npc->size = 3;
			npc->damage = 10;
			npc->code_event = 1000;
			npc->life = 700;
			break;

		case 5:
			npc->act_no = 6;
			npc->x = 0;
			npc->y = 0;
			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->x = 0x14000;
			npc->y = 0x9400;
			break;

		case 20:
			npc->damage = 0;
			npc->act_no = 21;
			npc->x = 0x14000;
			npc->y = 0x33A00;
			npc->bits &= ~0x40;
			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			// Fallthrough
		case 21:
			if ((++npc->act_wait % 0x10) == 0)
				SetDestroyNpChar(npc->x + (Random(-40, 40) * 0x200), npc->y + (Random(-60, 60) * 0x200), 1, 1);

			break;

		case 30:
			npc->act_no = 31;
			npc->ani_no = 2;
			npc->x = 81920;
			npc->y = 0x8000;
			// Fallthrough
		case 31:
			npc->y += 0x800;

			if (npc->y >= 0x33A00)
			{
				npc->y = 0x33A00;
				npc->ani_no = 0;
				npc->act_no = 20;
				PlaySoundObject(44, 1);

				for (i = 0; i < 5; ++i)
				{
					x = npc->x + (Random(-40, 40) * 0x200);
					SetNpChar(4, x, npc->y + 0x7800, 0, 0, 0, 0, 0x100);
				}

			}

			break;

		case 100:
			npc->act_no = 101;
			npc->count2 = 9;
			npc->act_wait = -100;

			gBoss[1].cond = 0x80;
			gBoss[1].hit.front = 0x1C00;
			gBoss[1].hit.back = 0x1C00;
			gBoss[1].hit.top = 0x1000;
			gBoss[1].hit.bottom = 0x1000;
			gBoss[1].bits = 12;

			gBoss[2] = gBoss[1];

			gBoss[3].cond = 0x90;
			gBoss[3].bits |= 0x20;
			gBoss[3].hit.front = 0xC00;
			gBoss[3].hit.back = 0xC00;
			gBoss[3].hit.top = 0x1000;
			gBoss[3].hit.bottom = 0x1000;

			SetNpChar(325, npc->x, npc->y + 0x7800, 0, 0, 0, 0, 0x100);
			// Fallthrough
		case 101:
			if (npc->count2 > 1 && npc->life < 70 * npc->count2)
			{
				--npc->count2;

				for (i = 0; i < 5; ++i)
				{
					ChangeMapParts(i + 8, npc->count2, 0);
					SetDestroyNpChar((i + 8) * 0x200 * 0x10, npc->count2 * 0x200 * 0x10, 0, 4);
					PlaySoundObject(12, 1);
				}
			}

			if (++npc->act_wait == 81 || npc->act_wait == 241)
				SetNpChar(323, 0x6000, 0x1E000, 0, 0, 1, 0, 0x100);

			if (npc->act_wait == 1 || npc->act_wait == 161)
				SetNpChar(323, 0x22000, 0x1E000, 0, 0, 1, 0, 0x100);

			if (npc->act_wait >= 300)
			{
				npc->act_wait = 0;
				SetNpChar(325, npc->x, npc->y + 0x7800, 0, 0, 0, 0, 0x100);
			}

			break;

		case 500:
			gBoss[3].bits &= ~0x20;

			npc->act_no = 501;
			npc->act_wait = 0;
			npc->count1 = 0;

			DeleteNpCharCode(325, 1);
			DeleteNpCharCode(330, 1);
			// Fallthrough
		case 501:
			if ((++npc->act_wait % 0x10) == 0)
			{
				PlaySoundObject(12, 1);
				SetDestroyNpChar(npc->x + (Random(-40, 40) * 0x200), npc->y + (Random(-60, 60) * 0x200), 1, 1);
			}

			if (npc->act_wait == 95)
				npc->ani_no = 1;
			if (npc->act_wait == 98)
				npc->ani_no = 2;

			if (npc->act_wait > 100)
				npc->act_no = 510;
			break;

		case 510:
			npc->ym += 0x40;
			npc->damage = 0x7F;
			npc->y += npc->ym;

			if (npc->count1 == 0 && npc->y > 0x14000)
			{
				npc->count1 = 1;
				npc->ym = -0x200;
				npc->damage = 0;

				for (i = 0; i < 7; ++i)
				{
					ChangeMapParts(i + 7, 14, 0);
					SetDestroyNpChar((i + 7) * 0x200 * 0x10, 0x1C000, 0, 0);
					PlaySoundObject(12, 1);
				}
			}

			if (npc->y > 0x3C000)
				npc->act_no = 520;

			break;
	}

	gBoss[1].x = npc->x - 0x3000;
	gBoss[1].y = npc->y + 0x6800;

	gBoss[2].x = npc->x + 0x3000;
	gBoss[2].y = npc->y + 0x6800;

	gBoss[3].x = npc->x;
	gBoss[3].y = npc->y + 0x5000;

	RECT rc[3] = {
		{0, 0, 80, 120},
		{80, 0, 160, 120},
		{160, 0, 240, 120},
	};

	RECT rcDamage[3] = {
		{0, 120, 80, 240},
		{80, 120, 160, 240},
		{160, 120, 240, 240},
	};

	if (npc->shock)
	{
		if (++flash / 2 % 2)
			npc->rect = rc[npc->ani_no];
		else
			npc->rect = rcDamage[npc->ani_no];
	}
	else
	{
		npc->rect = rc[npc->ani_no];
	}
}
