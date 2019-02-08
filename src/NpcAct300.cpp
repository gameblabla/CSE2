#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"
#include "Boss.h"
#include "Frame.h"
#include "Map.h"
#include "Bullet.h"

//Demon crown (opening)
void ActNpc300(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {192, 80, 208, 96};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y += 0xC00;
			break;
	}

	if (++npc->ani_wait % 8 == 1)
		SetCaret(npc->x + (Random(-8, 8) * 0x200), npc->y + 0x1000, 13, 1);

	npc->rect = rc[0];
}

//Camera focus marker
void ActNpc302(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 10:
			npc->x = gMC.x;
			npc->y = gMC.y - 0x4000;
			break;

		case 20:
			switch (npc->direct)
			{
				case 0:
					npc->x -= 0x400;
					break;
				case 1:
					npc->y -= 0x400;
					break;
				case 2:
					npc->x += 0x400;
					break;
				case 3:
					npc->y += 0x400;
					break;
			}

			gMC.x = npc->x;
			gMC.y = npc->y;
			break;

		case 30:
			npc->x = gMC.x;
			npc->y = gMC.y + 0xA000;
			break;

		case 100:
			npc->act_no = 101;

			if (npc->direct)
			{
				int i;
				for (i = 0xAA; i < 0x200; ++i)
				{
					if (gNPC[i].cond & 0x80 && gNPC[i].code_event == npc->direct)
					{
						npc->pNpc = &gNPC[i];
						break;
					}
				}
				if (i == 0x200)
				{
					npc->cond = 0;
					break;
				}
			}
			else
			{
				npc->pNpc = gBoss;
			}
			// Fallthrough
		case 101:
			npc->x = (npc->pNpc->x + gMC.x) / 2;
			npc->y = (npc->pNpc->y + gMC.y) / 2;
			break;
	}
}

//Stumpy
void ActNpc308(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {128, 112, 144, 128};
	rcLeft[1] = {144, 112, 160, 128};

	rcRight[0] = {128, 128, 144, 144};
	rcRight[1] = {144, 128, 160, 144};

	unsigned char deg;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.x < npc->x + 0x1E000 && gMC.x > npc->x - 0x1E000 && gMC.y < npc->y + 0x18000 && gMC.y > npc->y - 0x18000)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->xm2 = 0;
			npc->ym2 = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 11:
			if (++npc->act_wait > 50)
				npc->act_no = 20;

			++npc->ani_wait;

			if (npc->act_wait > 1)
			{
				npc->ani_wait = 0;

				if (++npc->ani_no > 1)
					npc->ani_no = 0;
			}

			if (gMC.x > npc->x + 0x28000 || gMC.x < npc->x - 0x28000 || gMC.y > npc->y + 0x1E000 || gMC.y < npc->y - 0x1E000)
				npc->act_no = 0;

			break;

		case 20:
			npc->act_no = 21;
			npc->act_wait = 0;

			deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y) + Random(-3, 3);
			npc->ym2 = 2 * GetSin(deg);
			npc->xm2 = 2 * GetCos(deg);

			if (npc->xm2 < 0)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 21:
			if (npc->xm2 < 0 && npc->flag & 1)
			{
				npc->direct = 2;
				npc->xm2 = -npc->xm2;
			}

			if (npc->xm2 > 0 && npc->flag & 4)
			{
				npc->direct = 0;
				npc->xm2 = -npc->xm2;
			}

			if (npc->ym2 < 0 && npc->flag & 2)
				npc->ym2 = -npc->ym2;
			if (npc->ym2 > 0 && npc->flag & 8)
				npc->ym2 = -npc->ym2;

			if (npc->flag & 0x100)
				npc->ym2 = -0x200;

			npc->x += npc->xm2;
			npc->y += npc->ym2;

			if (++npc->act_wait > 50)
				npc->act_no = 10;

			if (++npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Ma Pignon
void ActNpc313(NPCHAR *npc)
{
	RECT rcLeft[14];
	RECT rcRight[14];

	rcLeft[0] = {128, 0, 144, 16};
	rcLeft[1] = {144, 0, 160, 16};
	rcLeft[2] = {160, 0, 176, 16};
	rcLeft[3] = {176, 0, 192, 16};
	rcLeft[4] = {192, 0, 208, 16};
	rcLeft[5] = {208, 0, 224, 16};
	rcLeft[6] = {224, 0, 240, 16};
	rcLeft[7] = {240, 0, 256, 16};
	rcLeft[8] = {256, 0, 272, 16};
	rcLeft[9] = {272, 0, 288, 16};
	rcLeft[10] = {288, 0, 304, 16};
	rcLeft[11] = {128, 0, 144, 16};
	rcLeft[12] = {176, 0, 192, 16};
	rcLeft[13] = {304, 0, 320, 16};

	rcRight[0] = {128, 16, 144, 32};
	rcRight[1] = {144, 16, 160, 32};
	rcRight[2] = {160, 16, 176, 32};
	rcRight[3] = {176, 16, 192, 32};
	rcRight[4] = {192, 16, 208, 32};
	rcRight[5] = {208, 16, 224, 32};
	rcRight[6] = {224, 16, 240, 32};
	rcRight[7] = {240, 16, 256, 32};
	rcRight[8] = {256, 16, 272, 32};
	rcRight[9] = {272, 16, 288, 32};
	rcRight[10] = {288, 16, 304, 32};
	rcRight[11] = {128, 16, 144, 32};
	rcRight[12] = {176, 16, 192, 32};
	rcRight[13] = {304, 16, 320, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->y += 0x800;
			// Fallthrough
		case 1:
			npc->ym += 0x40;

			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (gMC.x > npc->x - 0x4000 && gMC.x < npc->x + 0x4000)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 100:
			npc->act_no = 110;
			npc->act_wait = 0;
			npc->count1 = 0;
			npc->bits |= 0x20;
			// Fallthrough
		case 110:
			npc->damage = 1;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->ani_no = 0;

			if (++npc->act_wait > 4)
			{
				npc->act_wait = 0;
				npc->act_no = 120;

				if (++npc->count2 > 12)
				{
					npc->count2 = 0;
					npc->act_no = 300;
				}
			}

			break;

		case 120:
			npc->ani_no = 2;

			if (++npc->act_wait > 4)
			{
				npc->act_no = 130;
				npc->ani_no = 3;
				npc->xm = 2 * Random(-0x200, 0x200);
				npc->ym = -0x800;
				PlaySoundObject(30, 1);
				++npc->count1;
			}

			break;

		case 130:
			npc->ym += 0x80;

			if (npc->y > 0x10000)
				npc->bits &= ~8;

			if (npc->xm < 0 && npc->flag & 1)
				npc->xm = -npc->xm;
			if (npc->xm > 0 && npc->flag & 4)
				npc->xm = -npc->xm;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (npc->ym < -0x200)
				npc->ani_no = 3;
			else if (npc->ym > 0x200)
				npc->ani_no = 4;
			else
				npc->ani_no = 0;

			if (npc->flag & 8)
			{
				npc->act_no = 140;
				npc->act_wait = 0;
				npc->ani_no = 2;
				npc->xm = 0;
			}

			if (npc->count1 > 4 && gMC.y < npc->y + 0x800)
			{
				npc->act_no = 200;
				npc->act_wait = 0;
				npc->xm = 0;
				npc->ym = 0;
			}

			break;

		case 140:
			npc->ani_no = 2;

			if (++npc->act_wait > 4)
				npc->act_no = 110;

			break;

		case 200:
			npc->ani_no = 5;

			if (++npc->act_wait > 10)
			{
				npc->act_no = 210;
				npc->ani_no = 6;

				if (npc->direct == 0)
					npc->xm = -0x5FF;
				else
					npc->xm = 0x5FF;

				PlaySoundObject(25, 1);
				npc->bits &= ~0x20;
				npc->bits |= 4;
				npc->damage = 10;
			}

			break;

		case 210:
			if (++npc->ani_no > 7)
				npc->ani_no = 6;

			if (npc->xm < 0 && npc->flag & 1)
				npc->act_no = 220;
			if (npc->xm > 0 && npc->flag & 4)
				npc->act_no = 220;

			break;

		case 220:
			npc->act_no = 221;
			npc->act_wait = 0;
			SetQuake(16);
			PlaySoundObject(26, 1);
			npc->damage = 4;
			// Fallthrough
		case 221:
			if (++npc->ani_no > 7)
				npc->ani_no = 6;

			if (++npc->act_wait % 6 == 0)
				SetNpChar(314, Random(4, 16) * 0x2000, 0x2000, 0, 0, 0, 0, 0x100);

			if (npc->act_wait > 30)
			{
				npc->count1 = 0;
				npc->act_no = 130;
				npc->bits |= 0x20;
				npc->bits &= ~4;
				npc->damage = 3;
			}

			break;

		case 300:
			npc->act_no = 301;
			npc->ani_no = 9;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 301:
			if (++npc->ani_no > 11)
				npc->ani_no = 9;

			if (npc->direct == 0)
				npc->xm = -0x400;
			else
				npc->xm = 0x400;

			if (gMC.x > npc->x - 0x800 && gMC.x < npc->x + 0x800)
			{
				npc->act_no = 310;
				npc->act_wait = 0;
				npc->ani_no = 2;
				npc->xm = 0;
			}

			break;

		case 310:
			npc->ani_no = 2;

			if (++npc->act_wait > 4)
			{
				npc->act_no = 320;
				npc->ani_no = 12;
				npc->ym = -0x800;
				PlaySoundObject(25, 1);
				npc->bits |= 8;
				npc->bits &= ~0x20;
				npc->bits |= 4;
				npc->damage = 10;
			}

			break;

		case 320:
			if (++npc->ani_no > 13)
				npc->ani_no = 12;

			if (npc->y < 0x2000)
				npc->act_no = 330;

			break;

		case 330:
			npc->ym = 0;
			npc->act_no = 331;
			npc->act_wait = 0;
			SetQuake(16);
			PlaySoundObject(26, 1);
			// Fallthrough
		case 331:
			if (++npc->ani_no > 13)
				npc->ani_no = 12;

			if (++npc->act_wait % 6 == 0)
				SetNpChar(315, Random(4, 16) * 0x2000, 0, 0, 0, 0, 0, 0x100);

			if (npc->act_wait > 30)
			{
				npc->count1 = 0;
				npc->act_no = 130;
				npc->bits |= 0x20;
				npc->bits &= ~4;
				npc->damage = 3;
			}

			break;

		case 500:
			npc->bits &= ~0x20;
			npc->act_no = 501;
			npc->act_wait = 0;
			npc->ani_no = 8;
			npc->tgt_x = npc->x;
			npc->damage = 0;
			DeleteNpCharCode(315, 1);
			// Fallthrough
		case 501:
			npc->ym += 0x20;

			if (++npc->act_wait % 2)
				npc->x = npc->tgt_x;
			else
				npc->x = npc->tgt_x + 0x200;

			break;
	}

	if (npc->act_no > 100 && npc->act_no < 500 && npc->act_no != 210 && npc->act_no != 320)
	{
		if (IsActiveSomeBullet())
		{
			npc->bits &= ~0x20;
			npc->bits |= 4;
		}
		else
		{
			npc->bits |= 0x20;
			npc->bits &= ~4;
		}
	}

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Ma Pignon rock
void ActNpc314(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {64, 64, 80, 80};
	rc[1] = {80, 64, 96, 80};
	rc[2] = {96, 64, 112, 80};

	switch (npc->act_no)
	{
		case 0:
			npc->count2 = 0;
			npc->act_no = 100;
			npc->bits |= 4;
			npc->ani_no = Random(0, 2);
			// Fallthrough
		case 100:
			npc->ym += 0x40;

			if (npc->ym > 0x700)
				npc->ym = 0x700;

			if (npc->y > 0x10000)
				npc->bits &= ~8;

			if (npc->flag & 8)
			{
				npc->ym = -0x200;
				npc->act_no = 110;
				npc->bits |= 8;
				PlaySoundObject(12, 1);
				SetQuake(10);

				for (int i = 0; i < 2; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + 0x2000, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			break;

		case 110:
			npc->ym += 0x40;

			if (npc->y > (gMap.length * 0x2000) + 0x4000)
			{
				npc->cond = 0;
				return;
			}

			break;
	}

	if (++npc->ani_wait > 6)
	{
		++npc->ani_wait;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	if (gMC.y > npc->y)
		npc->damage = 10;
	else
		npc->damage = 0;

	npc->y += npc->ym;

	npc->rect = rc[npc->ani_no];
}

//Ma Pignon clone
void ActNpc315(NPCHAR *npc)
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0] = {128, 0, 144, 16};
	rcLeft[1] = {160, 0, 176, 16};
	rcLeft[2] = {176, 0, 192, 16};
	rcLeft[3] = {192, 0, 208, 16};

	rcRight[0] = {128, 16, 144, 32};
	rcRight[1] = {160, 16, 176, 32};
	rcRight[2] = {176, 16, 192, 32};
	rcRight[3] = {192, 16, 208, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->ani_no = 3;
			npc->ym += 0x80;

			if (npc->y > 0x10000)
			{
				npc->act_no = 130;
				npc->bits &= ~8;
			}

			break;

		case 100:
			npc->act_no = 110;
			npc->act_wait = 0;
			npc->count1 = 0;
			npc->bits |= 0x20;
			// Fallthrough
		case 110:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->ani_no = 0;

			if (++npc->act_wait > 4)
			{
				npc->act_wait = 0;
				npc->act_no = 120;
			}

			break;

		case 120:
			npc->ani_no = 1;

			if (++npc->act_wait > 4)
			{
				npc->act_no = 130;
				npc->ani_no = 3;
				npc->xm = 2 * Random(-0x200, 0x200);
				npc->ym = -0x800;
				PlaySoundObject(30, 1);
			}

			break;

		case 130:
			npc->ym += 0x80;

			if (npc->xm < 0 && npc->flag & 1)
				npc->xm = -npc->xm;
			if (npc->xm > 0 && npc->flag & 4)
				npc->xm = -npc->xm;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (npc->ym < -0x200)
				npc->ani_no = 2;
			else if (npc->ym > 0x200)
				npc->ani_no = 0;
			else
				npc->ani_no = 3;

			if (npc->flag & 8)
			{
				npc->act_no = 140;
				npc->act_wait = 0;
				npc->ani_no = 1;
				npc->xm = 0;
			}

			break;

		case 140:
			npc->ani_no = 1;

			if (++npc->act_wait > 4)
			{
				npc->act_no = 110;
				npc->bits |= 0x20;
			}

			break;
	}

	if (npc->act_no > 100)
	{
		if (IsActiveSomeBullet())
		{
			npc->bits &= ~0x20;
			npc->bits |= 4;
		}
		else
		{
			npc->bits |= 0x20;
			npc->bits &= ~4;
		}
	}

	if (++npc->count2 > 300)
	{
		VanishNpChar(npc);
	}
	else
	{
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;

		npc->x += npc->xm;
		npc->y += npc->ym;

		if (npc->direct == 0)
			npc->rect = rcLeft[npc->ani_no];
		else
			npc->rect = rcRight[npc->ani_no];
	}
}
