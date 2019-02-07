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

//Stumpy
void ActNpc308(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {128, 112, 144, 128};
	rcLeft[1] = {144, 112, 160, 128};

	rcRight[0] = {128, 128, 144, 144};
	rcRight[1] = {144, 128, 160, 144};

	unsigned char deg;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.x < npc->x + 0x1E000 && gMC.x > npc->x - 0x1E000 && gMC.y < npc->y + 0x18000 && gMC.y > npc->y - 0x18000)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->xm2 = 0;
			npc->ym2 = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 11:
			if (++npc->act_wait > 50)
				npc->act_no = 20;

			++npc->ani_wait;

			if (npc->act_wait > 1)
			{
				npc->ani_wait = 0;

				if (++npc->ani_no > 1)
					npc->ani_no = 0;
			}

			if (gMC.x > npc->x + 0x28000 || gMC.x < npc->x - 0x28000 || gMC.y > npc->y + 0x1E000 || gMC.y < npc->y - 0x1E000)
				npc->act_no = 0;

			break;

		case 20:
			npc->act_no = 21;
			npc->act_wait = 0;

			deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y) + Random(-3, 3);
			npc->ym2 = 2 * GetSin(deg);
			npc->xm2 = 2 * GetCos(deg);

			if (npc->xm2 < 0)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 21:
			if (npc->xm2 < 0 && npc->flag & 1)
			{
				npc->direct = 2;
				npc->xm2 = -npc->xm2;
			}

			if (npc->xm2 > 0 && npc->flag & 4)
			{
				npc->direct = 0;
				npc->xm2 = -npc->xm2;
			}

			if (npc->ym2 < 0 && npc->flag & 2)
				npc->ym2 = -npc->ym2;
			if (npc->ym2 > 0 && npc->flag & 8)
				npc->ym2 = -npc->ym2;

			if (npc->flag & 0x100)
				npc->ym2 = -0x200;

			npc->x += npc->xm2;
			npc->y += npc->ym2;

			if (++npc->act_wait > 50)
				npc->act_no = 10;

			if (++npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}
