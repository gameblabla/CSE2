#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

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
