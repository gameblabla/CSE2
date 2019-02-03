#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Quote (teleport out)
void ActNpc111(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};

	switch ( npc->act_no )
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->y -= 0x2000;
			break;

		case 1:
			if (++npc->act_wait > 20)
			{
				npc->act_wait = 0;
				npc->act_no = 2;
				npc->ani_no = 1;
				npc->ym = -0x2FF;
			}

			break;

		case 2:
			if (npc->ym > 0)
				npc->hit.bottom = 0x2000;

			if (npc->flag & 8)
			{
				npc->act_no = 3;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;

		case 3:
			if (++npc->act_wait > 40)
			{
				npc->act_no = 4;
				npc->act_wait = 64;
				PlaySoundObject(29, 1);
			}

			break;

		case 4:
			--npc->act_wait;
			npc->ani_no = 0;

			if (npc->act_wait == 0)
				npc->cond = 0;

			break;
	}

	npc->ym += 0x40;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (gMC.equip & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}

	if (npc->act_no == 4)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 4;

		if (npc->act_wait / 2 % 2)
			++npc->rect.left;
	}
}

//Quote (teleport in)
void ActNpc112(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->x += 0x2000;
			npc->y += 0x1000;
			PlaySoundObject(29, 1);
			// Fallthrough
		case 1:
			if (++npc->act_wait == 64)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
			}

			break;

		case 2:
			if (++npc->act_wait > 20)
			{
				npc->act_no = 3;
				npc->ani_no = 1;
				npc->hit.bottom = 0x1000;
			}

			break;

		case 3:
			if (npc->flag & 8)
			{
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;
	}

	npc->ym += 0x40;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (gMC.equip & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}

	if (npc->act_no == 1)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 4;

		if ( npc->act_wait / 2 % 2 )
			++npc->rect.left;
	}
}

//Red petals
void ActNpc116(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {272, 184, 320, 200};

	npc->rect = rc[0];
}

//Table and chair
void ActNpc119(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {248, 184, 272, 200};

	npc->rect = rc[0];
}
