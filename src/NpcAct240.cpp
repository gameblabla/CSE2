#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Curly (carried and unconcious)
void ActNpc259(NPCHAR *npc)
{
	RECT rcLeft = {224, 96, 240, 112};
	RECT rcRight = {224, 112, 240, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~0x2000;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.direct == 0)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->y = gMC.y - 0x800;

			if (npc->direct == 0)
			{
				npc->x = gMC.x + 0x600;
				npc->rect = rcLeft;
			}
			else
			{
				npc->x = gMC.x - 0x600;
				npc->rect = rcRight;
			}

			if (gMC.ani_no % 2)
				++npc->rect.top;

			break;

		case 10:
			npc->act_no = 11;
			npc->xm = 0x40;
			npc->ym = -0x20;

			npc->rect = rcLeft;
			break;

		case 11:
			if (npc->y < 0x8000)
				npc->ym = 0x20;

			npc->x += npc->xm;
			npc->y += npc->ym;
			break;

		case 20:
			VanishNpChar(npc);
			SetDestroyNpCharUp(npc->x, npc->y, 0x2000, 0x40);
			break;
	}
}
