#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Sue/Itoh becoming humans
void ActNpc326(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			npc->x += 0x2000;
			npc->ani_no = 0;
			//Fallthrough
		case 1:
			if (++npc->act_wait > 80)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
			}
			else
			{
				if (npc->direct)
				{
					if (npc->act_wait == 50)
						npc->ani_no = 1;
					if (npc->act_wait == 60)
						npc->ani_no = 0;
				}
				else
				{
					if (npc->act_wait == 30)
						npc->ani_no = 1;
					if (npc->act_wait == 40)
						npc->ani_no = 0;
				}
			}
			break;
		case 10:
			if (++npc->act_wait > 50)
			{
				npc->act_no = 15;
				npc->ani_no = 4;
				if ( npc->direct )
					npc->act_wait = -20;
				else
					npc->act_wait = 0;
			}
			else
			{
				if (npc->act_wait / 2 & 1)
					npc->ani_no = 2;
				else
					npc->ani_no = 3;
			}
			break;
		case 15:
			if (++npc->act_wait > 40)
			{
				npc->act_wait = 0;
				npc->act_no = 20;
			}
			break;
		case 20:
			npc->ym += 0x40;
			if (npc->ym > 0x5FF)
				npc->ym = 0x5FF;
			
			npc->y += npc->ym;
			
			if (++npc->act_wait > 50)
			{
				npc->act_no = 30;
				npc->act_wait = 0;
				npc->ani_no = 6;
				
				if (npc->direct)
					SetNpChar(327, npc->x, npc->y - 0x1000, 0, 0, 0, npc, 0x100);
				else
					SetNpChar(327, npc->x, npc->y - 0x2000, 0, 0, 0, npc, 0x100);
			}
			break;
		case 30:
			if (++npc->act_wait == 30)
				npc->ani_no = 7;
			if (npc->act_wait == 40)
				npc->act_no = 40;
			break;
		case 40:
			npc->act_no = 41;
			npc->act_wait = 0;
			npc->ani_no = 0;
			//Fallthorugh
		case 41:
			if (++npc->act_wait == 30)
				npc->ani_no = 1;
			if (npc->act_wait == 40)
				npc->ani_no = 0;
			break;
		default:
			break;
	}
	
	RECT rcSu[8];
	RECT rcItoh[8];
	rcItoh[0] = {0, 128, 16, 152};
	rcItoh[1] = {16, 128, 32, 152};
	rcItoh[2] = {32, 128, 48, 152};
	rcItoh[3] = {48, 128, 64, 152};
	rcItoh[4] = {64, 128, 80, 152};
	rcItoh[5] = {80, 128, 96, 152};
	rcItoh[6] = {96, 128, 112, 152};
	rcItoh[7] = {112, 128, 128, 152};
	rcSu[0] = {128, 128, 144, 152};
	rcSu[1] = {144, 128, 160, 152};
	rcSu[2] = {160, 128, 176, 152};
	rcSu[3] = {176, 128, 192, 152};
	rcSu[4] = {192, 128, 208, 152};
	rcSu[5] = {208, 128, 224, 152};
	rcSu[6] = {224, 128, 240, 152};
	rcSu[7] = {32, 152, 48, 176};
	
	if (npc->direct)
		npc->rect = rcSu[npc->ani_no];
	else
		npc->rect = rcItoh[npc->ani_no];
}

//Sneeze
void ActNpc327(NPCHAR *npc)
{
	RECT rc[2];
	rc[0] = {240, 80, 256, 96};
	rc[1] = {256, 80, 272, 96};
	
	++npc->act_wait;
	
	switch (npc->act_no)
	{
		case 0:
			if (npc->act_wait < 4)
				npc->y -= 0x400;
			
			if (npc->pNpc->ani_no == 7)
			{
				npc->ani_no = 1;
				npc->act_no = 1;
				npc->tgt_x = npc->x;
				npc->tgt_y = npc->y;
			}
			break;
			
		case 1:
			if (npc->act_wait >= 48)
			{
				npc->x = npc->tgt_x;
				npc->y = npc->tgt_y;
			}
			else
			{
				npc->x = npc->tgt_x + (Random(-1, 1) << 9);
				npc->y = npc->tgt_y + (Random(-1, 1) << 9);
			}
			break;
	}
	
	if (npc->act_wait > 70)
		npc->cond = 0;
	
	npc->rect = rc[npc->ani_no];
}

//Thingy that turns Sue and Itoh into humans for 4 seconds
void ActNpc328(NPCHAR *npc)
{
	npc->rect = {96, 0, 128, 48};
}

//Laboratory fan
void ActNpc329(NPCHAR *npc)
{
	if (++npc->ani_wait / 2 & 1)
		npc->rect = {48, 0, 64, 16};
	else
		npc->rect = {64, 0, 80, 16};
}

//Sweat
void ActNpc334(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {160, 184, 168, 200};
	rcLeft[1] = {168, 184, 176, 200};

	rcRight[0] = {176, 184, 184, 200};
	rcRight[1] = {184, 184, 192, 200};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;

			if (npc->direct == 0)
			{
				npc->x += 0x1400;
				npc->y -= 0x2400;
			}
			else
			{
				npc->x = gMC.x - 0x1400;
				npc->y = gMC.y - 0x400;
			}
			// Fallthrough
		case 10:
			if (++npc->act_wait / 8 & 1)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (npc->act_wait >= 64)
				npc->cond = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Ikachan
void ActNpc335(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {0, 16, 16, 32};
	rc[1] = {16, 16, 32, 32};
	rc[2] = {32, 16, 48, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(3, 20);
			// Fallthrough
		case 1:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 2;
				npc->act_wait = Random(10, 50);
				npc->ani_no = 1;
				npc->xm = 0x600;
			}

			break;

		case 2:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 3;
				npc->act_wait = Random(40, 50);
				npc->ani_no = 2;
				npc->ym = Random(-0x100, 0x100);
			}

			break;

		case 3:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 1;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;
	}

	npc->xm -= 0x10;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rc[npc->ani_no];
}

//Ikachan generator
void ActNpc336(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			if (gMC.shock)
				npc->cond = 0;

			break;

		case 10:
			if (++npc->act_wait % 4 == 1 )
				SetNpChar(335, npc->x, npc->y + (Random(0, 13) * 0x2000), 0, 0, 0, 0, 0);

			break;
	}
}

//Numhachi
void ActNpc337(NPCHAR *npc)
{
	RECT rcLeft[2];

	rcLeft[0] = {256, 112, 288, 152};
	rcLeft[1] = {288, 112, 320, 152};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			// Fallthrough
		case 1:
			npc->xm = 0;
			npc->act_no = 2;
			npc->ani_no = 0;
			// Fallthrough
		case 2:
			if (++npc->ani_wait > 50)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rcLeft[npc->ani_no];
}
