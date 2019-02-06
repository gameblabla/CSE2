#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"

//Scooter
void ActNpc192(NPCHAR *npc)
{
	switch ( npc->act_no )
	{
		case 0:
			npc->act_no = 1;
			npc->view.back = 0x2000;
			npc->view.front = 0x2000;
			npc->view.top = 0x1000;
			npc->view.bottom = 0x1000;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 1;
			npc->view.top = 0x2000;
			npc->view.bottom = 0x2000;
			npc->y -= 0xA00;
			break;

		case 20:
			npc->act_no = 21;
			npc->act_wait = 1;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			// Fallthrough
		case 21:
			npc->x = npc->tgt_x + (Random(-1, 1) * 0x200);
			npc->y = npc->tgt_y + (Random(-1, 1) * 0x200);

			if (++npc->act_wait > 30)
				npc->act_no = 30;

			break;

		case 30:
			npc->act_no = 31;
			npc->act_wait = 1;
			npc->xm = -0x800;
			npc->x = npc->tgt_x;
			npc->y = npc->tgt_y;
			PlaySoundObject(44, 1);
			// Fallthrough
		case 31:
			npc->xm += 0x20;
			npc->x += npc->xm;
			++npc->act_wait;
			npc->y = npc->tgt_y + (Random(-1, 1) * 0x200);

			if (npc->act_wait > 10)
				npc->direct = 2;

			if (npc->act_wait > 200)
				npc->act_no = 40;

			break;

		case 40:
			npc->act_no = 41;
			npc->act_wait = 2;
			npc->direct = 0;
			npc->y -= 0x6000;
			npc->xm = -0x1000;
			// Fallthrough
		case 41:
			npc->x += npc->xm;
			npc->y += npc->ym;

			npc->act_wait += 2;

			if (npc->act_wait > 1200)
				npc->cond = 0;

			break;
	}

	if (npc->act_wait % 4 == 0 && npc->act_no >= 20)
	{
		PlaySoundObject(34, 1);

		if (npc->direct == 0)
			SetCaret(npc->x + 5120, npc->y + 5120, 7, 2);
		else
			SetCaret(npc->x - 5120, npc->y + 5120, 7, 0);
	}

	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {224, 64, 256, 80};
	rcLeft[1] = {256, 64, 288, 96};

	rcRight[0] = {224, 80, 256, 96};
	rcRight[1] = {288, 64, 320, 96};

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Scooter (broken)
void ActNpc193(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {256, 96, 320, 112};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;
		npc->y = npc->y;
		npc->x += 0x3000;
	}

	npc->rect = rc[0];
}

//Blue robot (broken)
void ActNpc194(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {192, 120, 224, 128};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;
		npc->y += 0x800;
	}

	npc->rect = rc[0];
}

//Water/wind particles
void ActNpc199(NPCHAR *npc)
{
	RECT rect[5];

	rect[0] = {72, 16, 74, 18};
	rect[1] = {74, 16, 76, 18};
	rect[2] = {76, 16, 78, 18};
	rect[3] = {78, 16, 80, 18};
	rect[4] = {80, 16, 82, 18};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;
		npc->ani_no = Random(0, 2);

		switch (npc->direct)
		{
			case 0:
				npc->xm = -1;
				break;
			case 1:
				npc->ym = -1;
				break;
			case 2:
				npc->xm = 1;
				break;
			case 3:
				npc->ym = 1;
				break;
		}

		npc->xm *= (Random(4, 8) * 0x200) / 2;
		npc->ym *= (Random(4, 8) * 0x200) / 2;
	}

	if (++npc->ani_wait > 6)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 4)
		npc->cond = 0;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rect[npc->ani_no];
}
