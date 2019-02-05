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
	SetQuake(10);
}

//Doctor (opening)
void ActNpc298(NPCHAR *npc)
{
	RECT rc[8];

	rc[0] = {72, 128, 88, 160};
	rc[1] = {88, 128, 104, 160};
	rc[2] = {104, 128, 120, 160};
	rc[3] = {72, 128, 88, 160};
	rc[4] = {120, 128, 136, 160};
	rc[5] = {72, 128, 88, 160};
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
