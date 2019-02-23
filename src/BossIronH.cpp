#include "BossIronH.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "Sound.h"

void ActBossChar_Ironhead(void)
{
	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss[0].cond = 0x80;
			gBoss[0].exp = 1;
			gBoss[0].direct = 2;
			gBoss[0].act_no = 100;
			gBoss[0].x = 0x14000;
			gBoss[0].y = 0x10000;
			gBoss[0].view.front = 0x5000;
			gBoss[0].view.top = 0x1800;
			gBoss[0].view.back = 0x3000;
			gBoss[0].view.bottom = 0x1800;
			gBoss[0].hit_voice = 54;
			gBoss[0].hit.front = 0x2000;
			gBoss[0].hit.top = 0x1400;
			gBoss[0].hit.back = 0x2000;
			gBoss[0].hit.bottom = 0x1400;
			gBoss[0].bits = 0x8228;
			gBoss[0].size = 3;
			gBoss[0].damage = 10;
			gBoss[0].code_event = 1000;
			gBoss[0].life = 400;
			break;

		case 100:
			gBoss[0].act_no = 101;
			gBoss[0].bits &= ~0x20;
			gBoss[0].act_wait = 0;
			// Fallthrough
		case 101:
			if (++gBoss[0].act_wait > 50)
			{
				gBoss[0].act_no = 250;
				gBoss[0].act_wait = 0;
			}

			if (gBoss[0].act_wait % 4 == 0)
				SetNpChar(197, Random(15, 18) * 0x2000, Random(2, 13) * 0x2000, 0, 0, 0, 0, 0x100);

			break;

		case 250:
			gBoss[0].act_no = 251;

			if (gBoss[0].direct == 2)
			{
				gBoss[0].x = 0x1E000;
				gBoss[0].y = gMC.y;
			}
			else
			{
				gBoss[0].x = 0x5A000;
				gBoss[0].y = Random(2, 13) * 0x2000;
			}

			gBoss[0].tgt_x = gBoss[0].x;
			gBoss[0].tgt_y = gBoss[0].y;

			gBoss[0].ym = Random(-0x200, 0x200);
			gBoss[0].xm = Random(-0x200, 0x200);

			gBoss[0].bits |= 0x20;
			// Fallthrough
		case 251:
			if (gBoss[0].direct == 2)
			{
				gBoss[0].tgt_x += 0x400;
			}
			else
			{
				gBoss[0].tgt_x -= 0x200;

				if (gMC.y > gBoss[0].tgt_y)
					gBoss[0].tgt_y += 0x200;
				else
					gBoss[0].tgt_y -= 0x200;
			}

			if (gBoss[0].tgt_x > gBoss[0].x)
				gBoss[0].xm += 8;
			else
				gBoss[0].xm -= 8;

			if (gBoss[0].tgt_y > gBoss[0].y)
				gBoss[0].ym += 8;
			else
				gBoss[0].ym -= 8;

			if (gBoss[0].ym > 0x200)
				gBoss[0].ym = 0x200;
			if (gBoss[0].ym < -0x200)
				gBoss[0].ym = -0x200;

			gBoss[0].x += gBoss[0].xm;
			gBoss[0].y += gBoss[0].ym;

			if (gBoss[0].direct == 2)
			{
				if (gBoss[0].x > 0x5A000)
				{
					gBoss[0].direct = 0;
					gBoss[0].act_no = 100;
				}
			}
			else
			{
				if (gBoss[0].x < 0x22000)
				{
					gBoss[0].direct = 2;
					gBoss[0].act_no = 100;
				}
			}

			if (gBoss[0].direct == 0 && (++gBoss[0].act_wait == 300 || gBoss[0].act_wait == 310 || gBoss[0].act_wait == 320))
			{
				PlaySoundObject(39, 1);
				SetNpChar(198, gBoss[0].x + 0x1400, gBoss[0].y + 0x200, Random(-3, 0) * 0x200, Random(-3, 3) * 0x200, 2, 0, 0x100);
			}

			if (++gBoss[0].ani_wait > 2)
			{
				gBoss[0].ani_wait = 0;
				++gBoss[0].ani_no;
			}

			if (gBoss[0].ani_no > 7)
				gBoss[0].ani_no = 0;

			break;

		case 1000:
			gBoss[0].bits &= ~0x20;
			gBoss[0].ani_no = 8;
			gBoss[0].damage = 0;
			gBoss[0].act_no = 1001;
			gBoss[0].tgt_x = gBoss[0].x;
			gBoss[0].tgt_y = gBoss[0].y;
			SetQuake(20);

			for (int i = 0; i < 0x20; ++i)
				SetNpChar(4, gBoss[0].x + (Random(-0x80, 0x80) * 0x200), gBoss[0].y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			DeleteNpCharCode(197, 1);
			DeleteNpCharCode(271, 1);
			DeleteNpCharCode(272, 1);
			// Fallthrough
		case 1001:
			gBoss[0].tgt_x -= 0x200;

			gBoss[0].x = gBoss[0].tgt_x + (Random(-1, 1) * 0x200);
			gBoss[0].y = gBoss[0].tgt_y + (Random(-1, 1) * 0x200);

			if (++gBoss[0].act_wait % 4 == 0)
				SetNpChar(4, gBoss[0].x + (Random(-0x80, 0x80) * 0x200), gBoss[0].y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			break;
	}

	RECT rc[9] = {
		{0, 0, 64, 24},
		{64, 0, 128, 24},
		{128, 0, 192, 24},
		{64, 0, 128, 24},
		{0, 0, 64, 24},
		{192, 0, 256, 24},
		{256, 0, 320, 24},
		{192, 0, 256, 24},
		{256, 48, 320, 72},
	};

	RECT rcDamage[9] = {
		{0, 24, 64, 48},
		{64, 24, 128, 48},
		{128, 24, 192, 48},
		{64, 24, 128, 48},
		{0, 24, 64, 48},
		{192, 24, 256, 48},
		{256, 24, 320, 48},
		{192, 24, 256, 48},
		{256, 48, 320, 72},
	};

	if (gBoss[0].shock)
	{
		static unsigned char flash;

		if ((++flash >> 1) % 2)
			gBoss[0].rect = rc[gBoss[0].ani_no];
		else
			gBoss[0].rect = rcDamage[gBoss[0].ani_no];
	}
	else
	{
		gBoss[0].rect = rc[gBoss[0].ani_no];
	}
}