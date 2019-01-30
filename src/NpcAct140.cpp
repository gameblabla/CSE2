#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//King's sword
void ActNpc145(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = {96, 32, 112, 48};
	rcRight[0] = {112, 32, 128, 48};

	if (npc->act_no == 0)
	{
		if (npc->pNpc->count2 == 0)
		{
			if (npc->pNpc->direct == 0)
				npc->direct = 0;
			else
				npc->direct = 2;
		}
		else
		{
			if (npc->pNpc->direct == 0)
				npc->direct = 2;
			else
				npc->direct = 0;
		}

		if (npc->direct == 0)
			npc->x = npc->pNpc->x - 0x1400;
		else
			npc->x = npc->pNpc->x + 0x1400;

		npc->y = npc->pNpc->y;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Blue robot (standing)
void ActNpc151(NPCHAR *npc)
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
			if (Random(0, 100) == 0)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 16)
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
