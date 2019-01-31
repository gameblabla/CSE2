#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "CommonDefines.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Quote and Curly on Balrog's back
void ActNpc355(NPCHAR *npc)
{
	RECT rc[4];

	rc[0] = {80, 16, 96, 32};
	rc[1] = {80, 96, 96, 112};
	rc[2] = {128, 16, 144, 32};
	rc[3] = {208, 96, 224, 112};

	if (npc->act_no == 0)
	{
		switch (npc->direct)
		{
			case 0:
				npc->surf = 16;
				npc->ani_no = 0;
				npc->x = npc->pNpc->x - 0x1C00;
				npc->y = npc->pNpc->y + 0x1400;
				break;

			case 1:
				npc->surf = 23;
				npc->ani_no = 1;
				npc->x = npc->pNpc->x + 0x1C00;
				npc->y = npc->pNpc->y + 0x1400;
				break;

			case 2:
				npc->surf = 16;
				npc->ani_no = 2;
				npc->x = npc->pNpc->x - 0xE00;
				npc->y = npc->pNpc->y - 0x2600;
				break;

			case 3:
				npc->surf = 23;
				npc->ani_no = 3;
				npc->x = npc->pNpc->x + 0x800;
				npc->y = npc->pNpc->y - 0x2600;
				break;
		}
	}

	npc->rect = rc[npc->ani_no];
}

//Water droplet generator
void ActNpc359(NPCHAR *npc)
{
	if (gMC.x < npc->x + (((WINDOW_WIDTH / 2) + 160) * 0x200) && gMC.x > npc->x - (((WINDOW_WIDTH / 2) + 160) * 0x200) && gMC.y < npc->y + (((WINDOW_HEIGHT / 2) + 200) * 0x200) && gMC.y > npc->y - (((WINDOW_HEIGHT / 2) + 40) * 0x200) && Random(0, 100) == 2)
		SetNpChar(73, npc->x + (Random(-6, 6) * 0x200), npc->y - 0xE00, 0, 0, 0, 0, 0);
}
