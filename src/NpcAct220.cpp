#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"

//Shovel Brigade
void ActNpc220(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 64, 16, 80};
	rcLeft[1] = {16, 64, 32, 80};

	rcRight[0] = {0, 80, 16, 96};
	rcRight[1] = {16, 80, 32, 96};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
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
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Shovel Brigade (walking)
void ActNpc221(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {0, 64, 16, 80};
	rcLeft[1] = {16, 64, 32, 80};
	rcLeft[2] = {32, 64, 48, 80};
	rcLeft[3] = {0, 64, 16, 80};
	rcLeft[4] = {48, 64, 64, 80};
	rcLeft[5] = {0, 64, 16, 80};

	rcRight[0] = {0, 80, 16, 96};
	rcRight[1] = {16, 80, 32, 96};
	rcRight[2] = {32, 80, 48, 96};
	rcRight[3] = {0, 80, 16, 96};
	rcRight[4] = {48, 80, 64, 96};
	rcRight[5] = {0, 80, 16, 96};

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
			npc->ani_no = 2;
			npc->ani_wait = 0;

			if (Random(0, 9) % 2)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 11:
			if (npc->direct == 0 && npc->flag & 1)
				npc->direct = 2;
			else if ( npc->direct == 2 && npc->flag & 4 )
				npc->direct = 0;

			if (npc->direct == 0)
				npc->xm = -0x200;
			else
				npc->xm = 0x200;

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			if (++npc->act_wait > 32)
				npc->act_no = 0;

			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Prison bars
void ActNpc222(NPCHAR *npc)
{
	RECT rc = {96, 168, 112, 200};

	if (npc->act_no == 0)
	{
		++npc->act_no;
		npc->y -= 0x1000;
	}

	npc->rect = rc;
}

//Momorin
void ActNpc223(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {80, 192, 96, 216};
	rcLeft[1] = {96, 192, 112, 216};
	rcLeft[2] = {112, 192, 128, 216};

	rcRight[0] = {80, 216, 96, 240};
	rcRight[1] = {96, 216, 112, 240};
	rcRight[2] = {112, 216, 128, 240};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 160) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 12)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 3:
			npc->ani_no = 2;
			break;
	}

	if (npc->act_no < 2 && gMC.y < npc->y + 0x2000 && gMC.y > npc->y - 0x2000)
	{
		if (gMC.x < npc->x)
			npc->direct = 0;
		else
			npc->direct = 2;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Chie
void ActNpc224(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {112, 32, 128, 48};
	rcLeft[1] = {128, 32, 144, 48};

	rcRight[0] = {112, 48, 128, 64};
	rcRight[1] = {128, 48, 144, 64};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 160) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 12 )
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->act_no < 2 && gMC.y < npc->y + 0x2000 && gMC.y > npc->y - 0x2000)
	{
		if (gMC.x < npc->x)
			npc->direct = 0;
		else
			npc->direct = 2;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Megane
void ActNpc225(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {64, 64, 80, 80};
	rcLeft[1] = {80, 64, 96, 80};

	rcRight[0] = {64, 80, 80, 96};
	rcRight[1] = {80, 80, 96, 96};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 160) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 12 )
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

//Kanpachi
void ActNpc226(NPCHAR *npc)
{
	RECT rcRight[7];

	rcRight[0] = {256, 56, 272, 80};
	rcRight[1] = {272, 56, 288, 80};
	rcRight[2] = {288, 56, 304, 80};
	rcRight[3] = {256, 56, 272, 80};
	rcRight[4] = {304, 56, 320, 80};
	rcRight[5] = {256, 56, 272, 80};
	rcRight[6] = {240, 56, 256, 80};

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
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough
		case 11:
			npc->xm = 0x200;

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			++npc->act_wait;
			break;

		case 20:
			npc->xm = 0;
			npc->ani_no = 6;
			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rcRight[npc->ani_no];
}

//Bucket
void ActNpc227(NPCHAR *npc)
{
	RECT rc = {208, 32, 224, 48};
	npc->rect = rc;
}

//Droll (guard)
void ActNpc228(NPCHAR *npc)
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0] = {0, 0, 32, 40};
	rcLeft[1] = {32, 0, 64, 40};
	rcLeft[2] = {64, 0, 96, 40};
	rcLeft[3] = {96, 0, 128, 40};

	rcRight[0] = {0, 40, 32, 80};
	rcRight[1] = {32, 40, 64, 80};
	rcRight[2] = {64, 40, 96, 80};
	rcRight[3] = {96, 40, 128, 80};

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
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (++npc->ani_wait > 50)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 2;
			npc->act_wait = 0;
			// Fallthrough
		case 11:
			if (++npc->act_wait > 10)
			{
				npc->act_no = 12;
				npc->ani_no = 3;
				npc->ym = -0x600;

				if (npc->direct == 0)
					npc->xm = -0x200;
				else
					npc->xm = 0x200;
			}

			break;

		case 12:
			if (npc->flag & 8)
			{
				npc->ani_no = 2;
				npc->act_no = 13;
				npc->act_wait = 0;
			}

			break;

		case 13:
			npc->xm /= 2;

			if (++npc->act_wait > 10)
				npc->act_no = 1;

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}
