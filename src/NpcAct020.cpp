#include "WindowsWrapper.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Chest (open)
void ActNpc021(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
				npc->y += 0x2000;

			break;
	}

	RECT rect[0];

	rect[0]	= {224, 40, 240, 48};

	npc->rect = rect[0];
}

//Life capsule
void ActNpc032(NPCHAR *npc)
{
	RECT rect[2];
	rect[0] = {32, 96, 48, 112};
	rect[1] = {48, 96, 64, 112};
	
	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}
	
	if (npc->ani_no > 1)
		npc->ani_no = 0;
	
	npc->rect = rect[npc->ani_no];
}