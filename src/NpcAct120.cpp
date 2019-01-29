#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Hidden item
void ActNpc125(NPCHAR *npc)
{
	if (npc->life < 990)
	{
		SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
		PlaySoundObject(70, 1);

		if (npc->direct)
			SetNpChar(86, npc->x, npc->y, 0, 0, 2, 0, 0);
		else
			SetNpChar(87, npc->x, npc->y, 0, 0, 2, 0, 0);

		npc->cond = 0;
	}

	RECT rc[2];

	rc[0] = {0, 96, 16, 112};
	rc[1] = {16, 96, 32, 112};

	if (npc->direct == 0)
		npc->rect = rc[0];
	else
		npc->rect = rc[1];
}
