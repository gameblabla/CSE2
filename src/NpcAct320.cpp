#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Sweat
void ActNpc334(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {160, 184, 168, 200};
	rcLeft[1] = {168, 184, 176, 200};

	rcRight[0] = {176, 184, 184, 200};
	rcRight[1] = {184, 184, 192, 200};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;

			if (npc->direct == 0)
			{
				npc->x += 0x1400;
				npc->y -= 0x2400;
			}
			else
			{
				npc->x = gMC.x - 0x1400;
				npc->y = gMC.y - 0x400;
			}
			// Fallthrough
		case 1:
			if (++npc->act_wait / 8 & 1)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (npc->act_wait >= 64)
				npc->cond = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}
