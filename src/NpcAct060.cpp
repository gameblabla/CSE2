#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Map.h"

//Kazuma at computer
void ActNpc062(NPCHAR *npc)
{
	RECT rcLeft[3];

	rcLeft[0] = {272, 192, 288, 216};
	rcLeft[1] = {288, 192, 304, 216};
	rcLeft[2] = {304, 192, 320, 216};

	switch ( npc->act_no )
	{
		case 0:
			npc->x -= 0x800;
			npc->y += 0x2000;
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough

		case 1:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (Random(0, 80) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (Random(0, 120) == 10)
			{
				npc->act_no = 3;
				npc->act_wait = 0;
				npc->ani_no = 2;
			}

			break;

		case 2:
			if (++npc->act_wait > 40)
			{
				npc->act_no = 3;
				npc->act_wait = 0;
				npc->ani_no = 2;
			}

			break;

		case 3:
			if (++npc->act_wait > 80)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	npc->rect = rcLeft[npc->ani_no];
}

//First Cave Critter
void ActNpc064(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};
	rcLeft[2] = {32, 0, 48, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};
	rcRight[2] = {32, 16, 48, 32};

	switch (npc->act_no)
	{
		case 0: //Init
			npc->y += 0x600;
			npc->act_no = 1;

		case 1: //Waiting
			//Look at player
			if (npc->x <= gMC.x)
				npc->direct = 2;
			else
				npc->direct = 0;

			if (npc->tgt_x < 100)
				++npc->tgt_x;

			//Open eyes near player
			if (npc->act_wait < 8 || npc->x - 0xE000 >= gMC.x || npc->x + 0xE000 <= gMC.x || npc->y - 0xA000 >= gMC.y || npc->y + 0xA000 <= gMC.y)
			{
				if (npc->act_wait < 8)
					++npc->act_wait;
				npc->ani_no = 0;
			}
			else
			{
				npc->ani_no = 1;
			}

			//Jump if attacked
			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			//Jump if player is nearby
			if (npc->act_wait >= 8 && npc->tgt_x >= 100 && npc->x - 0x8000 < gMC.x && npc->x + 0x8000 > gMC.x && npc->y - 0xA000 < gMC.y && npc->y + 0x6000 > gMC.y)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}
			break;

		case 2: //Going to jump
			if (++npc->act_wait > 8)
			{
				//Set jump state
				npc->act_no = 3;
				npc->ani_no = 2;

				//Jump
				npc->ym = -0x5FF;
				PlaySoundObject(30, 1);

				//Jump in facing direction
				if (npc->direct)
					npc->xm = 0x100;
				else
					npc->xm = -0x100;
			}
			break;

		case 3: //Jumping
			//Land
			if (npc->flag & 8)
			{
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;
				PlaySoundObject(23, 1);
			}
			break;
	}

	//Gravity
	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	//Move
	npc->x += npc->xm;
	npc->y += npc->ym;

	//Set framerect
	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//First Cave Bat
void ActNpc065(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			npc->count1 = 120;
			npc->act_no = 1;
			npc->act_wait = Random(0, 50);
			// Fallthrough

		case 1:
			if (++npc->act_wait >= 50)
			{
				npc->act_wait = 0;
				npc->act_no = 2;
				npc->ym = 0x300;
			}

			break;

		case 2:
			if (gMC.x >= npc->x)
				npc->direct = 2;
			else
				npc->direct = 0;

			if (npc->tgt_y < npc->y)
				npc->ym -= 0x10;
			if (npc->tgt_y > npc->y)
				npc->ym += 0x10;
			if (npc->ym > 0x300)
				npc->ym = 0x300;
			if (npc->ym < -0x300)
				npc->ym = -0x300;

			break;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[4];
	RECT rect_right[4];

	rect_left[0] = {32, 32, 48, 48};
	rect_left[1] = {48, 32, 64, 48};
	rect_left[2] = {64, 32, 80, 48};
	rect_left[3] = {80, 32, 96, 48};

	rect_right[0] = {32, 48, 48, 64};
	rect_right[1] = {48, 48, 64, 64};
	rect_right[2] = {64, 48, 80, 64};
	rect_right[3] = {80, 48, 96, 64};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}

//Water droplet
void ActNpc073(NPCHAR *npc)
{
	RECT rect[5];

	rect[0] = {72, 16, 74, 18};
	rect[1] = {74, 16, 76, 18};
	rect[2] = {76, 16, 78, 18};
	rect[3] = {78, 16, 80, 18};
	rect[4] = {80, 16, 82, 18};

	npc->ym += 0x20;
	npc->ani_no = Random(0, 4);

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rect[npc->ani_no];

	if (npc->direct == 2)
	{
		npc->rect.top += 2;
		npc->rect.bottom += 2;
	}

	if (++npc->act_wait > 10)
	{
		if (npc->flag & 1)
			npc->cond = 0;
		if (npc->flag & 4)
			npc->cond = 0;
		if (npc->flag & 8)
			npc->cond = 0;
		if (npc->flag & 0x100)
			npc->cond = 0;
	}

	if (npc->y > gMap.length * 0x10 * 0x200)
		npc->cond = 0;
}
