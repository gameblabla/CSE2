#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "CommonDefines.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Hoppy
void ActNpc347(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			npc->ani_no = 0;

			if (gMC.y < npc->y + 0x10000 && gMC.y > npc->y - 0x10000)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 10:
			if (++npc->act_wait == 4)
				npc->ani_no = 2;

			if (npc->act_wait > 12)
			{
				npc->act_no = 12;
				npc->xm = 0x700;
				PlaySoundObject(6, 1);
				npc->ani_no = 3;
			}

			break;

		case 12:
			if (gMC.y < npc->y)
				npc->ym = -0xAAu;
			else
				npc->ym = 0xAA;

			if (npc->flag & 1)
			{
				npc->act_no = 13;
				npc->act_wait = 0;
				npc->ani_no = 2;
				npc->xm = 0;
				npc->ym = 0;
			}
			else
			{
				npc->xm -= 42;

				if (npc->xm < -0x5FF)
					npc->xm = -0x5FF;

				npc->x += npc->xm;
				npc->y += npc->ym;
			}

			break;

		case 13:
			++npc->act_wait;

			if (++npc->act_wait == 2)
				npc->ani_no = 1;

			if (npc->act_wait == 6)
				npc->ani_no = 0;

			if (npc->act_wait > 16)
				npc->act_no = 1;

			break;
	}

	RECT rc[4];

	rc[0] = {256, 48, 272, 64};
	rc[1] = {272, 48, 288, 64};
	rc[2] = {288, 48, 304, 64};
	rc[3] = {304, 48, 320, 64};

	npc->rect = rc[npc->ani_no];
}

//Statue
void ActNpc349(NPCHAR *npc)
{
	RECT rect = {0, 0, 16, 16};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;

		if (npc->direct == 0)
			npc->x += 0x1000;
		if (npc->direct == 2)
			npc->y += 0x2000;
	}

	npc->rect = rect;
}

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
