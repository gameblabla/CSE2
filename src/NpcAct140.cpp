#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//King's sword
void ActNpc145(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = {96, 32, 112, 48};
	rcRight[0] = {112, 32, 128, 48};

	if (npc->act_no == 0)
	{
		if (npc->pNpc->count2 == 0)
		{
			if (npc->pNpc->direct == 0)
				npc->direct = 0;
			else
				npc->direct = 2;
		}
		else
		{
			if (npc->pNpc->direct == 0)
				npc->direct = 2;
			else
				npc->direct = 0;
		}

		if (npc->direct == 0)
			npc->x = npc->pNpc->x - 0x1400;
		else
			npc->x = npc->pNpc->x + 0x1400;

		npc->y = npc->pNpc->y;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Quote
void ActNpc150(NPCHAR *npc)
{
	RECT rcLeft[9];
	RECT rcRight[9];
	
	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {48, 0, 64, 16};
	rcLeft[2] = {144, 0, 160, 16};
	rcLeft[3] = {16, 0, 32, 16};
	rcLeft[4] = {0, 0, 16, 16};
	rcLeft[5] = {32, 0, 48, 16};
	rcLeft[6] = {0, 0, 16, 16};
	rcLeft[7] = {160, 0, 176, 16};
	rcLeft[8] = {112, 0, 128, 16};
	
	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {48, 16, 64, 32};
	rcRight[2] = {144, 16, 160, 32};
	rcRight[3] = {16, 16, 32, 32};
	rcRight[4] = {0, 16, 16, 32};
	rcRight[5] = {32, 16, 48, 32};
	rcRight[6] = {0, 16, 16, 32};
	rcRight[7] = {160, 16, 176, 32};
	rcRight[8] = {112, 16, 128, 32};
	
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			
			if (npc->direct > 10)
			{
				npc->x = gMC.x;
				npc->y = gMC.y;
				npc->direct -= 10;
			}
			break;
		case 2:
			npc->ani_no = 1;
			break;
		case 10:
			npc->act_no = 11;
			for (int i = 0; i < 4; i++)
				SetNpChar(4, npc->x, npc->y, Random(-0x155, 0x155), Random(-0x600, 0), 0, 0, 0x100);
			PlaySoundObject(71, 1);
		case 11:
			npc->ani_no = 2;
			break;
		case 20:
			npc->act_no = 21;
			npc->act_wait = 64;
			PlaySoundObject(29, 1);
		case 21:
			if (!--npc->act_wait)
				npc->cond = 0;
			break;
		case 50:
			npc->act_no = 51;
			npc->ani_no = 3;
			npc->ani_wait = 0;
		case 51:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}
			if (npc->ani_no > 6)
				npc->ani_no = 3;
			
			if ( npc->direct )
				npc->x += 0x200;
			else
				npc->x -= 0x200;
			break;
		case 60:
			npc->act_no = 61;
			npc->ani_no = 7;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
		case 61:
			npc->tgt_y += 0x100;
			npc->x = npc->tgt_x + (Random(-1, 1) << 9);
			npc->y = npc->tgt_y + (Random(-1, 1) << 9);
			break;
		case 70:
			npc->act_no = 71;
			npc->act_wait = 0;
			npc->ani_no = 3;
			npc->ani_wait = 0;
		case 0x47:
			if ( npc->direct )
				npc->x -= 0x100;
			else
				npc->x += 0x100;
			
			if (++npc->ani_wait > 8)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}
			if (npc->ani_no > 6)
				npc->ani_no = 3;
			break;
		case 80:
			npc->ani_no = 8;
			break;
		case 99:
		case 100:
			npc->act_no = 101;
			npc->ani_no = 3;
			npc->ani_wait = 0;
		case 101:
			npc->ym += 0x40;
			if (npc->ym > 0x5FF)
				npc->ym = 0x5FF;
			if (npc->flag & 8)
			{
				npc->ym = 0;
				npc->act_no = 102;
			}
			npc->y += npc->ym;
			break;
		case 102:
			if (++npc->ani_wait > 8)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}
			if (npc->ani_no > 6)
				npc->ani_no = 3;
			break;
		default:
			break;
	}
	
	if (npc->direct)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
	
	if (npc->act_no == 21)
	{
		npc->rect.bottom = npc->act_wait / 4 + npc->rect.top;
		if (npc->act_wait / 2 & 1)
			++npc->rect.left;
	}
	
	if (gMC.equip & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}
}

//Blue robot (standing)
void ActNpc151(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {192, 0, 208, 16};
	rcLeft[1] = {208, 0, 224, 16};

	rcRight[0] = {192, 16, 208, 32};
	rcRight[1] = {208, 16, 224, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough

		case 1:
			if (Random(0, 100) == 0)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 16)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}
