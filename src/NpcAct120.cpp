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

//Machine gun trail (Level 2)
void ActNpc127(NPCHAR *npc)
{
	RECT rcV[3];
	RECT rcH[3];
	rcV[0] = {112, 48, 128, 64};
	rcV[1] = {112, 64, 128, 80};
	rcV[2] = {112, 80, 128, 96};
	rcH[0] = {64, 80, 80, 96};
	rcH[1] = {80, 80, 96, 96};
	rcH[2] = {96, 80, 112, 96};
	
	if (++npc->ani_wait > 0)
	{
		npc->ani_wait = 0;
		if (++npc->ani_no > 2)
			npc->cond = 0;
	}
	
	if (npc->direct)
		npc->rect = rcV[npc->ani_no];
	else
		npc->rect = rcH[npc->ani_no];
}

//Machine gun trail (Level 3)
void ActNpc128(NPCHAR *npc)
{
	RECT rcLeft[5];
	RECT rcRight[5];
	RECT rcUp[5];
	RECT rcDown[5];

	rcLeft[0] = {0, 0, 0, 0};
	rcLeft[1] = {176, 16, 184, 32};
	rcLeft[2] = {184, 16, 192, 32};
	rcLeft[3] = {192, 16, 200, 32};
	rcLeft[4] = {200, 16, 208, 32};
	rcRight[0] = {0, 0, 0, 0};
	rcRight[1] = {232, 16, 240, 32};
	rcRight[2] = {224, 16, 232, 32};
	rcRight[3] = {216, 16, 224, 32};
	rcRight[4] = {208, 16, 216, 32};
	rcUp[0] = {0, 0, 0, 0};
	rcUp[1] = {176, 32, 192, 40};
	rcUp[2] = {176, 40, 192, 48};
	rcUp[3] = {192, 32, 208, 40};
	rcUp[4] = {192, 40, 208, 48};
	rcDown[0] = {0, 0, 0, 0};
	rcDown[1] = {208, 32, 224, 40};
	rcDown[2] = {208, 40, 224, 48};
	rcDown[3] = {224, 32, 232, 40};
	rcDown[4] = {224, 40, 232, 48};
	
	if (!npc->act_no)
	{
		npc->act_no = 1;
		
		if (npc->direct && npc->direct != 2)
		{
			npc->view.front = 0x1000;
			npc->view.top = 0x800;
		}
		else
		{
			npc->view.front = 0x800;
			npc->view.top = 0x1000;
		}
	}
	
	if (++npc->ani_no > 4)
		npc->cond = 0;
	
	switch (npc->direct)
	{
		case 0:
			npc->rect = rcLeft[npc->ani_no];
			break;
		case 1:
			npc->rect = rcUp[npc->ani_no];
			break;
		case 2:
			npc->rect = rcRight[npc->ani_no];
			break;
		case 3:
			npc->rect = rcDown[npc->ani_no];
			break;
	}
}

//Fireball trail (Level 2 & 3)
void ActNpc129(NPCHAR *npc)
{
	RECT rect[18];
	rect[0] = {0x80, 0x30, 0x90, 0x40};
	rect[1] = {0x90, 0x30, 0xA0, 0x40};
	rect[2] = {0xA0, 0x30, 0xB0, 0x40};
	rect[3] = {0x80, 0x40, 0x90, 0x50};
	rect[4] = {0x90, 0x40, 0xA0, 0x50};
	rect[5] = {0xA0, 0x40, 0xB0, 0x50};
	rect[6] = {0x80, 0x50, 0x90, 0x60};
	rect[7] = {0x90, 0x50, 0xA0, 0x60};
	rect[8] = {0xA0, 0x50, 0xB0, 0x60};
	rect[9] = {0xB0, 0x30, 0xC0, 0x40};
	rect[10] = {0xC0, 0x30, 0xD0, 0x40};
	rect[11] = {0xD0, 0x30, 0xE0, 0x40};
	rect[12] = {0xB0, 0x40, 0xC0, 0x50};
	rect[13] = {0xC0, 0x40, 0xD0, 0x50};
	rect[14] = {0xD0, 0x40, 0xE0, 0x50};
	rect[15] = {0xB0, 0x50, 0xC0, 0x60};
	rect[16] = {0xC0, 0x50, 0xD0, 0x60};
	rect[17] = {0xD0, 0x50, 0xE0, 0x60};
	
	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		if (++npc->ani_no > 2)
			npc->cond = 0;
	}
	
	npc->y += npc->ym;
	
	npc->rect = rect[npc->ani_no + 3 * npc->direct];
}
