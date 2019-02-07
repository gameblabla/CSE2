#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"
#include "Map.h"

// Ironhead block
void ActNpc271(NPCHAR *npc)
{
	if (npc->xm < 0 && npc->x < -0x2000)
	{
		VanishNpChar(npc);
	}
	else
	{
		if (npc->xm > 0 && npc->x > (gMap.width + 1) * 0x2000)
		{
			VanishNpChar(npc);
		}
		else
		{
			if (npc->act_no == 0)
			{
				npc->act_no = 1;
				int a = Random(0, 9);

				if (a == 9)
				{
					npc->rect.left = 0;
					npc->rect.right = 0x20;
					npc->rect.top = 0x40;
					npc->rect.bottom = 0x60;
					npc->view.front = 0x2000;
					npc->view.back = 0x2000;
					npc->view.top = 0x2000;
					npc->view.bottom = 0x2000;
					npc->hit.front = 0x1800;
					npc->hit.back = 0x1800;
					npc->hit.top = 0x1800;
					npc->hit.bottom = 0x1800;
				}
				else
				{
					npc->rect.left = 16 * (a % 3 + 7);
					npc->rect.top = 16 * (a / 3);
					npc->rect.right = npc->rect.left + 16;
					npc->rect.bottom = npc->rect.top + 16;
				}

				if (npc->direct == 0)
					npc->xm = -2 * Random(0x100, 0x200);
				else
					npc->xm = 2 * Random(0x100, 0x200);

				npc->ym = Random(-0x200, 0x200);
			}

			if (npc->ym < 0 && npc->y - npc->hit.top <= 0xFFF)
			{
				npc->ym = -npc->ym;
				SetCaret(npc->x, npc->y - 0x1000, 13, 0);
				SetCaret(npc->x, npc->y - 0x1000, 13, 0);
			}

			if (npc->ym > 0 && npc->y + npc->hit.bottom > 0x1D000)
			{
				npc->ym = -npc->ym;
				SetCaret(npc->x, npc->y + 0x1000, 13, 0);
				SetCaret(npc->x, npc->y + 0x1000, 13, 0);
			}

			npc->x += npc->xm;
			npc->y += npc->ym;
		}
	}
}

// Ironhead block generator
void ActNpc272(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(0, 200);
			// Fallthrough
		case 1:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->act_no = 0;
				SetNpChar(271, npc->x, npc->y + (Random(-32, 32) * 0x200), 0, 0, npc->direct, 0, 0x100);
			}

			break;
	}
}

//Little family
void ActNpc278(NPCHAR *npc)
{
	RECT rcMama[2];
	RECT rcPapa[2];
	RECT rcKodomo[2];

	rcPapa[0] = {0, 120, 8, 128};
	rcPapa[1] = {8, 120, 16, 128};

	rcMama[0] = {16, 120, 24, 128};
	rcMama[1] = {24, 120, 32, 128};

	rcKodomo[0] = {32, 120, 40, 128};
	rcKodomo[1] = {40, 120, 48, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (Random(0, 60) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (Random(0, 60) == 1)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = Random(0, 16);
			npc->ani_no = 0;
			npc->ani_wait = 0;

			if (Random(0, 9) % 2)
				npc->direct = 0;
			else
				npc->direct = 2;

			// Fallthrough
		case 11:
			if (npc->direct == 0 && (npc->flag & 1))
				npc->direct = 2;
			else if ( npc->direct == 2 && npc->flag & 4 )
				npc->direct = 0;

			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (++npc->act_wait > 0x20)
				npc->act_no = 0;

			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	
	switch (npc->code_event)
	{
		case 200:
			npc->rect = rcPapa[npc->ani_no];
			break;

		case 210:
			npc->rect = rcMama[npc->ani_no];
			break;

		default:
			npc->rect = rcKodomo[npc->ani_no];
			break;
	}
}
