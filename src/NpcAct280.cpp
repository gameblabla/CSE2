#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Frame.h"

//Quake
void ActNpc292(NPCHAR *npc)
{
	(void)npc;

	SetQuake(10);
}

//Cloud
void ActNpc295(NPCHAR *npc)
{
	RECT rc[4];
	rc[0] = {0, 0, 208, 64};
	rc[1] = {32, 64, 144, 96};
	rc[2] = {32, 96, 104, 0x80};
	rc[3] = {104, 96, 144, 0x80};
	
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = npc->direct % 4;
			switch (npc->direct)
			{
				case 0:
					npc->ym = -1000;
					npc->view.back = 0xD000;
					npc->view.front = 0xD000;
					break;
				case 1:
					npc->ym = -0x800;
					npc->view.back = 0x7000;
					npc->view.front = 0x7000;
					break;
				case 2:
					npc->ym = -0x400;
					npc->view.back = 0x4000;
					npc->view.front = 0x4000;
					break;
				case 3:
					npc->ym = -0x200;
					npc->view.back = 0x2800;
					npc->view.front = 0x2800;
					break;
				case 4:
					npc->xm = -0x400;
					npc->view.back = 0xD000;
					npc->view.front = 0xD000;
					break;
				case 5:
					npc->xm = -0x200;
					npc->view.back = 0x7000;
					npc->view.front = 0x7000;
					break;
				case 6:
					npc->xm = -0x100;
					npc->view.back = 0x4000;
					npc->view.front = 0x4000;
					break;
				case 7:
					npc->xm = -0x80;
					npc->view.back = 0x2800;
					npc->view.front = 0x2800;
					break;
				default:
					break;
			}
			//Fallthrough
		case 1:
			npc->x += npc->xm;
			npc->y += npc->ym;
			if (npc->x < -0x8000)
				npc->cond = 0;
			if (npc->y < -0x4000)
				npc->cond = 0;
			break;
	}

	npc->rect = rc[npc->ani_no];
}

//Cloud generator
void ActNpc296(NPCHAR *npc)
{
	if (++npc->act_wait > 16)
	{
		npc->act_wait = Random(0, 16);
		int dir = Random(0, 100) % 4;
		
		int pri;
		if (npc->direct)
		{
			switch (dir)
			{
				case 0:
					pri = 0x80;
					break;
				case 1:
					pri = 0x55;
					break;
				case 2:
					pri = 0x40;
					break;
				case 3:
					pri = 0x00;
					break;
			}
			
			SetNpChar(295, npc->x, npc->y + (Random(-7, 7) << 13), 0, 0, dir + 4, 0, pri);
		}
		else
		{
			switch (dir)
			{
				case 0:
					pri = 0x180;
					break;
				case 1:
					pri = 0x80;
					break;
				case 2:
					pri = 0x40;
					break;
				case 3:
					pri = 0x00;
					break;
			}
			
			SetNpChar(295, npc->x + (Random(-10, 10) << 13), npc->y, 0, 0, dir, 0, pri);
		}
	}
}

//Sue in dragon's mouth
void ActNpc297(NPCHAR *npc)
{
	npc->x = npc->pNpc->x + 0x2000;
	npc->y = npc->pNpc->y + 0x1000;
	npc->rect = {112, 48, 0x80, 64};
}

//Doctor (opening)
void ActNpc298(NPCHAR *npc)
{
	RECT rc[8];

	rc[0] = {72, 0x80, 88, 160};
	rc[1] = {88, 0x80, 104, 160};
	rc[2] = {104, 0x80, 120, 160};
	rc[3] = {72, 0x80, 88, 160};
	rc[4] = {120, 0x80, 136, 160};
	rc[5] = {72, 0x80, 88, 160};
	rc[6] = {104, 160, 120, 192};
	rc[7] = {120, 160, 136, 192};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			// Fallthrough

		case 1:
			npc->ani_no = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->count1 = 0;
			// Fallthrough

		case 11:
			if (++npc->ani_wait > 6)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
			{
				npc->ani_no = 0;

				if (++npc->count1 > 7)
				{
					npc->ani_no = 0;
					npc->act_no = 1;
				}
			}

			break;

		case 20:
			npc->act_no = 21;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough

		case 21:
			if (++npc->ani_wait > 10)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if ( npc->ani_no > 5 )
				npc->ani_no = 2;

			npc->x += 0x100;

			break;

		case 30:
			npc->ani_no = 6;
			break;

		case 40:
			npc->act_no = 41;
			npc->ani_no = 6;
			npc->ani_wait = 0;
			npc->count1 = 0;
			// Fallthrough

		case 41:
			if (++npc->ani_wait > 6)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 7)
			{
				npc->ani_no = 6;

				if (++npc->count1 > 7)
				{
					npc->ani_no = 6;
					npc->act_no = 30;
				}
			}

			break;
	}

	npc->rect = rc[npc->ani_no];
}

//Balrog/Misery (opening)
void ActNpc299(NPCHAR *npc)
{
	RECT rc[2];

	rc[0] = {0, 0, 48, 48};
	rc[1] = {48, 0, 96, 48};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 0)
			{
				npc->ani_no = 1;
				npc->act_wait = 25;
				npc->y -= 1600;
			}
			else
			{
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			break;
	}

	if (++npc->act_wait / 50 % 2)
		npc->y += 0x40;
	else
		npc->y -= 0x40;

	npc->rect = rc[npc->ani_no];
}
