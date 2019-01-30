#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Little family
void ActNpc278(NPCHAR *npc)
{
	RECT rcMama[2];
	RECT rcPapa[2];
	RECT rcKodomo[2];

	rcPapa[0] = {0, 120, 8, 128};
	rcPapa[1] = {8, 120, 16, 128};

	rcMama[0] = {16, 120, 24, 128};
	rcMama[1] = {24, 120, 32, 128};

	rcKodomo[0] = {32, 120, 40, 128};
	rcKodomo[1] = {40, 120, 48, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (Random(0, 60) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (Random(0, 60) == 1)
			{
				npc->act_no = 10;
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

		case 10:
			npc->act_no = 11;
			npc->act_wait = Random(0, 16);
			npc->ani_no = 0;
			npc->ani_wait = 0;

			if (Random(0, 9) % 2)
				npc->direct = 0;
			else
				npc->direct = 2;

			// Fallthrough
		case 11:
			if (npc->direct == 0 && (npc->flag & 1))
				npc->direct = 2;
			else if ( npc->direct == 2 && npc->flag & 4 )
				npc->direct = 0;

			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (++npc->act_wait > 0x20)
				npc->act_no = 0;

			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	
	switch (npc->code_event)
	{
		case 200:
			npc->rect = rcPapa[npc->ani_no];
			break;

		case 210:
			npc->rect = rcMama[npc->ani_no];
			break;

		default:
			npc->rect = rcKodomo[npc->ani_no];
			break;
	}
}
