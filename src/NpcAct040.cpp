#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Eye door
void ActNpc059(NPCHAR *npc)
{
	RECT rcLeft[4];

	rcLeft[0] = {224, 16, 240, 40};
	rcLeft[1] = {208, 80, 224, 104};
	rcLeft[2] = {224, 80, 240, 104};
	rcLeft[3] = {240, 80, 256, 104};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough

		case 1:
			if (npc->x - 0x8000 < gMC.x && npc->x + 0x8000 > gMC.x && npc->y - 0x8000 < gMC.y && npc->y + 0x8000 > gMC.y)
			{
				npc->act_no = 2;
				npc->ani_wait = 0;
			}

			break;

		case 2:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no == 2)
				npc->act_no = 3;

			break;

		case 3:
			if (npc->x - 0x8000 >= gMC.x || npc->x + 0x8000 <= gMC.x || npc->y - 0x8000 >= gMC.y || npc->y + 0x8000 <= gMC.y)
			{
				npc->act_no = 4;
				npc->ani_wait = 0;
			}

			break;

		case 4:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				--npc->ani_no;
			}

			if (npc->ani_no == 0)
				npc->act_no = 1;

			break;
	}

	if (npc->shock)
		npc->rect = rcLeft[3];
	else
		npc->rect = rcLeft[npc->ani_no];
}
