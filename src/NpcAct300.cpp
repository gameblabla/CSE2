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
