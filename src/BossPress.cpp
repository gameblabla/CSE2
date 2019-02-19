#include "BossPress.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Game.h"
#include "Map.h"
#include "Sound.h"

void ActBossChar_Press(void)
{
	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss[0].act_no = 10;
			gBoss[0].cond = 0x80;
			gBoss[0].exp = 1;
			gBoss[0].direct = 2;
			gBoss[0].x = 0;
			gBoss[0].y = 0;
			gBoss[0].view.front = 0x5000;
			gBoss[0].view.top = 0x7800;
			gBoss[0].view.back = 0x5000;
			gBoss[0].view.bottom = 0x7800;
			gBoss[0].hit_voice = 54;
			gBoss[0].hit.front = 0x6200;
			gBoss[0].hit.top = 0x7800;
			gBoss[0].hit.back = 0x5000;
			gBoss[0].hit.bottom = 0x6000;
			gBoss[0].bits = 0x8248;
			gBoss[0].size = 3;
			gBoss[0].damage = 10;
			gBoss[0].code_event = 1000;
			gBoss[0].life = 700;
			break;

		case 5:
			gBoss[0].act_no = 6;
			gBoss[0].x = 0;
			gBoss[0].y = 0;
			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			break;

		case 10:
			gBoss[0].act_no = 11;
			gBoss[0].x = 0x14000;
			gBoss[0].y = 0x9400;
			break;

		case 20:
			gBoss[0].damage = 0;
			gBoss[0].act_no = 21;
			gBoss[0].x = 0x14000;
			gBoss[0].y = 0x33A00;
			gBoss[0].bits &= ~0x40;
			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			// Fallthrough
		case 21:
			if ((++gBoss[0].act_wait & 0xF) == 0)
				SetDestroyNpChar(gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + (Random(-60, 60) * 0x200), 1, 1);

			break;

		case 30:
			gBoss[0].act_no = 31;
			gBoss[0].ani_no = 2;
			gBoss[0].x = 81920;
			gBoss[0].y = 0x8000;
			// Fallthrough
		case 31:
			gBoss[0].y += 0x800;

			if (gBoss[0].y >= 0x33A00)
			{
				gBoss[0].y = 0x33A00;
				gBoss[0].ani_no = 0;
				gBoss[0].act_no = 20;
				PlaySoundObject(44, 1);

				for (int i = 0; i < 5; ++i)
					SetNpChar(4, gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + 0x7800, 0, 0, 0, 0, 0x100);

			}

			break;

		case 100:
			gBoss[0].act_no = 101;
			gBoss[0].count2 = 9;
			gBoss[0].act_wait = -100;

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

			SetNpChar(325, gBoss[0].x, gBoss[0].y + 0x7800, 0, 0, 0, 0, 0x100);
			// Fallthrough
		case 101:
			if (gBoss[0].count2 > 1 && gBoss[0].life < 70 * gBoss[0].count2)
			{
				--gBoss[0].count2;

				for (int i = 0; i < 5; ++i)
				{
					ChangeMapParts(i + 8, gBoss[0].count2, 0);
					SetDestroyNpChar((i + 8) * 0x2000, gBoss[0].count2 * 0x2000, 0, 4);
					PlaySoundObject(12, 1);
				}
			}

			if (++gBoss[0].act_wait == 81 || gBoss[0].act_wait == 241)
				SetNpChar(323, 0x6000, 0x1E000, 0, 0, 1, 0, 0x100);

			if (gBoss[0].act_wait == 1 || gBoss[0].act_wait == 161)
				SetNpChar(323, 0x22000, 0x1E000, 0, 0, 1, 0, 0x100);

			if (gBoss[0].act_wait >= 300)
			{
				gBoss[0].act_wait = 0;
				SetNpChar(325, gBoss[0].x, gBoss[0].y + 0x7800, 0, 0, 0, 0, 0x100);
			}

			break;

		case 500:
			gBoss[3].bits &= ~0x20;

			gBoss[0].act_no = 501;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;

			DeleteNpCharCode(325, 1);
			DeleteNpCharCode(330, 1);
			// Fallthrough
		case 501:
			if ((++gBoss[0].act_wait & 0xF) == 0)
			{
				PlaySoundObject(12, 1);
				SetDestroyNpChar(gBoss[0].x + (Random(-40, 40) * 0x200), gBoss[0].y + (Random(-60, 60) * 0x200), 1, 1);
			}

			if (gBoss[0].act_wait == 95)
				gBoss[0].ani_no = 1;
			if (gBoss[0].act_wait == 98)
				gBoss[0].ani_no = 2;

			if (gBoss[0].act_wait > 100)
				gBoss[0].act_no = 510;
			break;

		case 510:
			gBoss[0].ym += 0x40;
			gBoss[0].damage = 0x7F;
			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].count1 == 0 && gBoss[0].y > 0x14000)
			{
				gBoss[0].count1 = 1;
				gBoss[0].ym = -0x200;
				gBoss[0].damage = 0;

				for (int i = 0; i < 7; ++i)
				{
					ChangeMapParts(i + 7, 14, 0);
					SetDestroyNpChar((i + 7) * 0x2000, 0x1C000, 0, 0);
					PlaySoundObject(12, 1);
				}
			}

			if (gBoss[0].y > 0x3C000)
				gBoss[0].act_no = 520;

			break;
	}

	gBoss[1].x = gBoss[0].x - 0x3000;
	gBoss[1].y = gBoss[0].y + 0x6800;

	gBoss[2].x = gBoss[0].x + 0x3000;
	gBoss[2].y = gBoss[0].y + 0x6800;

	gBoss[3].x = gBoss[0].x;
	gBoss[3].y = gBoss[0].y + 0x5000;

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

	if (gBoss[0].shock)
	{
		static unsigned char flash;

		if ((++flash / 2) % 2)
			gBoss[0].rect = rc[gBoss[0].ani_no];
		else
			gBoss[0].rect = rcDamage[gBoss[0].ani_no];
	}
	else
	{
		gBoss[0].rect = rc[gBoss[0].ani_no];
	}
}