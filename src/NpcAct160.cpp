#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Frame.h"

//Puu Black
void ActNpc160(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~1;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x)
				npc->direct = 2;
			else
				npc->direct = 0;

			npc->ym = 0xA00;

			if (npc->y < 0x10000)
			{
				++npc->count1;
			}
			else
			{
				npc->bits &= ~8;
				npc->act_no = 2;
			}

			break;

		case 2:
			npc->ym = 0xA00;

			if (npc->flag & 8)
			{
				DeleteNpCharCode(161, 1);

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

				npc->act_no = 3;
				npc->act_wait = 0;
				SetQuake(30);
				PlaySoundObject(26, 1);
				PlaySoundObject(72, 1);
			}

			if (gMC.y > npc->y && gMC.flag & 8)
				npc->damage = 20;
			else
				npc->damage = 0;

			break;

		case 3:
			npc->damage = 20;
			npc->damage = 0;	// Smart code

			if (++npc->act_wait > 24)
			{
				npc->act_no = 4;
				npc->count1 = 0;
				npc->count2 = 0;
			}

			break;

		case 4:
			gSuperXpos = npc->x;
			gSuperYpos = npc->y;

			if (npc->shock % 2 == 1)
			{
				SetNpChar(161, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-0x600, 0x600), Random(-0x600, 0x600), 0, 0, 0x100);

				if (++npc->count1 > 30)
				{
					npc->count1 = 0;
					npc->act_no = 5;
					npc->ym = -0xC00;
					npc->bits |= 8;
				}
			}

			break;

		case 5:
			gSuperXpos = npc->x;
			gSuperYpos = npc->y;

			if (++npc->count1 > 60)
			{
				npc->count1 = 0;
				npc->act_no = 6;
			}

			break;

		case 6:
			gSuperXpos = gMC.x;
			gSuperYpos = 0x320000;

			if (++npc->count1 > 110)
			{
				npc->count1 = 10;
				npc->x = gMC.x;
				npc->y = 0;
				npc->ym = 0x5FF;
				npc->act_no = 1;
			}

			break;
	}

	npc->y += npc->ym;

	switch (npc->act_no)
	{
		case 0:
		case 1:
			npc->ani_no = 3;
			break;
		case 2:
			npc->ani_no = 3;
			break;
		case 3:
			npc->ani_no = 2;
			break;
		case 4:
			npc->ani_no = 0;
			break;
		case 5:
			npc->ani_no = 3;
			break;
		case 6:
			npc->ani_no = 3;
			break;
	}

	RECT rect_left[4];
	RECT rect_right[4];

	rect_left[0] = {0, 0, 40, 24};
	rect_left[1] = {40, 0, 80, 24};
	rect_left[2] = {80, 0, 120, 24};
	rect_left[3] = {120, 0, 160, 24};

	rect_right[0] = {0, 24, 40, 48};
	rect_right[1] = {40, 24, 80, 48};
	rect_right[2] = {80, 24, 120, 48};
	rect_right[3] = {120, 24, 160, 48};

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}

//Puu Black projectile
void ActNpc161(NPCHAR *npc)
{
	RECT rect[3];

	npc->exp = 0;

	if (gSuperXpos > npc->x)
		npc->xm += 0x40;
	else
		npc->xm -= 0x40;

	if (gSuperYpos > npc->y)
		npc->ym += 0x40;
	else
		npc->ym -= 0x40;

	if (npc->xm < -4605)
		npc->xm = -4605;
	if (npc->xm > 4605)
		npc->xm = 4605;

	if (npc->ym < -4605)
		npc->ym = -4605;
	if (npc->ym > 4605)
		npc->ym = 4605;

	if (npc->life < 100)
	{
		npc->bits &= ~0x20;
		npc->bits &= ~4;
		npc->damage = 0;
		npc->ani_no = 2;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->ani_no < 2)
	{
		if (Random(0, 10) == 2)
			npc->ani_no = 0;
		else
			npc->ani_no = 1;
	}

	rect[0] = {0, 48, 16, 64};
	rect[1] = {16, 48, 32, 64};
	rect[2] = {32, 48, 48, 64};

	npc->rect = rect[npc->ani_no];
}

//Puu Black (dead)
void ActNpc162(NPCHAR *npc)
{
	RECT rect_left = {40, 0, 80, 24};
	RECT rect_right = {40, 24, 80, 48};
	RECT rect_end = {0, 0, 0, 0};

	switch (npc->act_no)
	{
		case 0:
			DeleteNpCharCode(161, 1);
			PlaySoundObject(72, 1);

			for (int i = 0; i < 10; ++i)
				SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-0x600, 0x600), Random(-0x600, 0x600), 0, 0, 0x100);

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (npc->direct == 0)
				npc->rect = rect_left;
			else
				npc->rect = rect_right;

			npc->count1 = 0;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (++npc->count1 % 4 == 0)
				SetNpChar(161, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), 0, 0, 0, 0, 0x100);

			if (npc->count1 > 160)
			{
				npc->count1 = 0;
				npc->act_no = 2;
				npc->tgt_y = npc->y;
			}

			break;

		case 2:
			SetQuake(2);

			if (++npc->count1 > 240)
			{
				npc->rect = rect_end;

				npc->count1 = 0;
				npc->act_no = 3;
			}
			else
			{
				if (npc->direct == 0)
					npc->rect = rect_left;
				else
					npc->rect = rect_right;

				npc->rect.top += npc->count1 / 8;
				npc->y = npc->tgt_y + ((npc->count1 / 8) * 0x200);
				npc->rect.left -= (npc->count1 / 2) % 2;
			}

			if (npc->count1 % 3 == 2)
				SetNpChar(161, npc->x + (Random(-12, 12) * 0x200), npc->y - 0x1800, Random(-0x200, 0x200), 0x100, 0, 0, 0x100);

			if (npc->count1 % 4 == 2)
				PlaySoundObject(21, 1);

			break;

		case 3:
			if (++npc->count1 >= 60)
			{
				DeleteNpCharCode(161, 1);
				npc->cond = 0;
			}

			break;

	}

	gSuperXpos = npc->x;
	gSuperYpos = -0x7D000;
}

//Dr Gero
void ActNpc163(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {192, 0, 208, 16};
	rcLeft[1] = {208, 0, 224, 16};

	rcRight[0] = {192, 16, 208, 32};
	rcRight[1] = {208, 16, 224, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Nurse Hasumi
void ActNpc164(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {224, 0, 240, 16};
	rcLeft[1] = {240, 0, 256, 16};

	rcRight[0] = {224, 16, 240, 32};
	rcRight[1] = {240, 16, 256, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Curly (collapsed)
void ActNpc165(NPCHAR *npc)
{
	RECT rcRight[2];
	RECT rcLeft[1];

	rcRight[0] = {192, 96, 208, 112};
	rcRight[1] = {208, 96, 224, 112};

	rcLeft[0] = {144, 96, 160, 112};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y += 0x1400;
			// Fallthrough
		case 1:
			if (npc->direct == 2 && gMC.x > npc->x - 0x4000 && gMC.x < npc->x + 0x4000 && gMC.y > npc->y - 0x2000 && gMC.y < npc->y + 0x2000)
				npc->ani_no = 1;
			else
				npc->ani_no = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[0];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Chaba
void ActNpc166(NPCHAR *npc)
{
	RECT rcLeft[2];

	rcLeft[0] = {144, 104, 184, 128};
	rcLeft[1] = {184, 104, 224, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8 )
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	npc->rect = rcLeft[npc->ani_no];
}
