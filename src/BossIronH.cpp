#include "BossIronH.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"

void ActBossChar_Ironhead(void)
{
	static unsigned char flash;
	int i;

	NPCHAR *npc = gBoss;

	switch (npc->act_no)
	{
		case 0:
			npc->cond = 0x80;
			npc->exp = 1;
			npc->direct = 2;
			npc->act_no = 100;
			npc->x = 0x14000;
			npc->y = 0x10000;
			npc->view.front = 0x5000;
			npc->view.top = 0x1800;
			npc->view.back = 0x3000;
			npc->view.bottom = 0x1800;
			npc->hit_voice = 54;
			npc->hit.front = 0x2000;
			npc->hit.top = 0x1400;
			npc->hit.back = 0x2000;
			npc->hit.bottom = 0x1400;
			npc->bits = 0x8228;
			npc->size = 3;
			npc->damage = 10;
			npc->code_event = 1000;
			npc->life = 400;
			break;

		case 100:
			npc->act_no = 101;
			npc->bits &= ~0x20;
			npc->act_wait = 0;
			// Fallthrough
		case 101:
			if (++npc->act_wait > 50)
			{
				npc->act_no = 250;
				npc->act_wait = 0;
			}

			if (npc->act_wait % 4 == 0)
				SetNpChar(197, Random(15, 18) * 0x2000, Random(2, 13) * 0x2000, 0, 0, 0, 0, 0x100);

			break;

		case 250:
			npc->act_no = 251;

			if (npc->direct == 2)
			{
				npc->x = 0x1E000;
				npc->y = gMC.y;
			}
			else
			{
				npc->x = 0x5A000;
				npc->y = Random(2, 13) * 0x2000;
			}

			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;

			npc->ym = Random(-0x200, 0x200);
			npc->xm = Random(-0x200, 0x200);

			npc->bits |= 0x20;
			// Fallthrough
		case 251:
			if (npc->direct == 2)
			{
				npc->tgt_x += 0x400;
			}
			else
			{
				npc->tgt_x -= 0x200;

				if (npc->tgt_y < gMC.y)
					npc->tgt_y += 0x200;
				else
					npc->tgt_y -= 0x200;
			}

			if (npc->x < npc->tgt_x)
				npc->xm += 8;
			else
				npc->xm -= 8;

			if (npc->y < npc->tgt_y)
				npc->ym += 8;
			else
				npc->ym -= 8;

			if (npc->ym > 0x200)
				npc->ym = 0x200;
			if (npc->ym < -0x200)
				npc->ym = -0x200;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->direct == 2)
			{
				if (npc->x > 0x5A000)
				{
					npc->direct = 0;
					npc->act_no = 100;
				}
			}
			else
			{
				if (npc->x < 0x22000)
				{
					npc->direct = 2;
					npc->act_no = 100;
				}
			}

			if (npc->direct == 0 && (++npc->act_wait == 300 || npc->act_wait == 310 || npc->act_wait == 320))
			{
				PlaySoundObject(39, 1);
				SetNpChar(198, npc->x + 0x1400, npc->y + 0x200, Random(-3, 0) * 0x200, Random(-3, 3) * 0x200, 2, 0, 0x100);
			}

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 7)
				npc->ani_no = 0;

			break;

		case 1000:
			npc->bits &= ~0x20;
			npc->ani_no = 8;
			npc->damage = 0;
			npc->act_no = 1001;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			SetQuake(20);

			for (i = 0; i < 0x20; ++i)
				SetNpChar(4, npc->x + (Random(-0x80, 0x80) * 0x200), npc->y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

			DeleteNpCharCode(197, 1);
			DeleteNpCharCode(271, 1);
			DeleteNpCharCode(272, 1);
			// Fallthrough
		case 1001:
			npc->tgt_x -= 0x200;

			npc->x = npc->tgt_x + (Random(-1, 1) * 0x200);
			npc->y = npc->tgt_y + (Random(-1, 1) * 0x200);

			if (++npc->act_wait % 4 == 0)
				SetNpChar(4, npc->x + (Random(-0x80, 0x80) * 0x200), npc->y + (Random(-0x40, 0x40) * 0x200), Random(-0x80, 0x80) * 0x200, Random(-0x80, 0x80) * 0x200, 0, 0, 0x100);

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
