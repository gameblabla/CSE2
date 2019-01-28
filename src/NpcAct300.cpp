#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"

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
