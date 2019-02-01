#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"
#include "Flash.h"

//Gravekeeper
void ActNpc080(NPCHAR *npc)
{
	RECT rcLeft[7];
	RECT rcRight[7];

	rcLeft[0] = {0, 64, 24, 88};
	rcLeft[1] = {24, 64, 48, 88};
	rcLeft[2] = {0, 64, 24, 88};
	rcLeft[3] = {48, 64, 72, 88};
	rcLeft[4] = {72, 64, 96, 88};
	rcLeft[5] = {96, 64, 120, 88};
	rcLeft[6] = {120, 64, 144, 88};

	rcRight[0] = {0, 88, 24, 112};
	rcRight[1] = {24, 88, 48, 112};
	rcRight[2] = {0, 88, 24, 112};
	rcRight[3] = {48, 88, 72, 112};
	rcRight[4] = {72, 88, 96, 112};
	rcRight[5] = {96, 88, 120, 112};
	rcRight[6] = {120, 88, 144, 112};

	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~0x20;
			npc->act_no = 1;
			npc->damage = 0;
			npc->hit.front = 0x800;
			// Fallthrough
		case 1:
			npc->ani_no = 0;

			if (npc->x - 0x10000 < gMC.x && npc->x + 0x10000 > gMC.x && npc->y - 0x6000 < gMC.y && npc->y + 0x4000 > gMC.y)
			{
				npc->ani_wait = 0;
				npc->act_no = 2;
			}

			if (npc->shock)
			{
				npc->ani_no = 1;
				npc->ani_wait = 0;
				npc->act_no = 2;
				npc->bits &= ~0x20u;
			}

			if (gMC.x >= npc->x)
				npc->direct = 2;
			else
				npc->direct = 0;

			break;

		case 2:
			if (++npc->ani_wait > 6)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 0;

			if (npc->x - 0x2000 < gMC.x && npc->x + 0x2000 > gMC.x)
			{
				npc->hit.front = 0x2400;
				npc->act_wait = 0;
				npc->act_no = 3;
				npc->bits |= 0x20;
				PlaySoundObject(34, 1);

				if (npc->direct == 0)
					npc->xm = -0x400;
				else
					npc->xm = 0x400;
			}

			if (gMC.x >= npc->x)
			{
				npc->direct = 2;
				npc->xm = 0x100;
			}
			else
			{
				npc->direct = 0;
				npc->xm = -0x100;
			}

			break;

		case 3:
			npc->xm = 0;

			if (++npc->act_wait > 40)
			{
				npc->act_wait = 0;
				npc->act_no = 4;
				PlaySoundObject(106, 1);
			}

			npc->ani_no = 4;
			break;

		case 4:
			npc->damage = 10;

			if (++npc->act_wait > 2)
			{
				npc->act_wait = 0;
				npc->act_no = 5;
			}

			npc->ani_no = 5;
			break;

		case 5:
			npc->ani_no = 6;

			if (++npc->act_wait > 60)
				npc->act_no = 0;

			break;
	}

	if (npc->xm < 0 && npc->flag & 1)
		npc->xm = 0;
	if (npc->xm > 0 && npc->flag & 4)
		npc->xm = 0;

	npc->ym += 0x20;

	if (npc->xm > 0x400)
		npc->xm = 0x400;
	if (npc->xm < -0x400)
		npc->xm = -0x400;

	if (npc->ym > 0x5FF)
		npc->xm = 0x5FF;
	if (npc->ym < -0x5FF)
		npc->xm = -0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Giant pignon
void ActNpc081(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {144, 64, 168, 88};
	rcLeft[1] = {168, 64, 192, 88};
	rcLeft[2] = {192, 64, 216, 88};
	rcLeft[3] = {216, 64, 240, 88};
	rcLeft[4] = {144, 64, 168, 88};
	rcLeft[5] = {240, 64, 264, 88};

	rcRight[0] = {144, 88, 168, 112};
	rcRight[1] = {168, 88, 192, 112};
	rcRight[2] = {192, 88, 216, 112};
	rcRight[3] = {216, 88, 240, 112};
	rcRight[4] = {144, 88, 168, 112};
	rcRight[5] = {240, 88, 264, 112};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (Random(0, 100) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}
			else
			{
				if (Random(0, 150) == 1)
				{
					if (npc->direct == 0)
						npc->direct = 2;
					else
						npc->direct = 0;
				}

				if (Random(0, 150) == 1)
				{
					npc->act_no = 3;
					npc->act_wait = 50;
					npc->ani_no = 0;
				}
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 3:
			npc->act_no = 4;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough
		case 4:
			if (--npc->act_wait == 0)
				npc->act_no = 0;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
				npc->ani_no = 2;

			if (npc->flag & 1)
			{
				npc->direct = 2;
				npc->xm = 0x200;
			}

			if (npc->flag & 4)
			{
				npc->direct = 0;
				npc->xm = -0x200;
			}

			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			break;

		case 5:
			if (npc->flag & 8)
				npc->act_no = 0;

			break;
	}

	switch (npc->act_no)
	{
		case 1:
		case 2:
		case 4:
			if (npc->shock )
			{
				npc->ym = -0x200;
				npc->ani_no = 5;
				npc->act_no = 5;

				if (npc->x >= gMC.x)
					npc->xm = -0x100;
				else
					npc->xm = 0x100;
			}

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

//Misery (standing)
void ActNpc082(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 2;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 3;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 2;
			}

			break;

		case 15:
			npc->act_no = 16;
			npc->act_wait = 0;
			npc->ani_no = 4;
			// Fallthrough
		case 16:
			if (++npc->act_wait == 30)
			{
				PlaySoundObject(21, 1);
				SetNpChar(66, npc->x, npc->y - 0x2000, 0, 0, 0, npc, 0);
			}

			if (npc->act_wait == 50)
				npc->act_no = 14;

			break;

		case 20:
			npc->act_no = 21;
			npc->ani_no = 0;
			npc->ym = 0;
			npc->bits |= 8;
			// Fallthrough
		case 21:
			npc->ym -= 0x20;

			if (npc->y < -0x1000)
				npc->cond = 0;

			break;

		case 25:
			npc->act_no = 26;
			npc->act_wait = 0;
			npc->ani_no = 5;
			npc->ani_wait = 0;
			// Fallthrough
		case 26:
			if (++npc->ani_no > 7)
				npc->ani_no = 5;

			if (++npc->act_wait == 30)
			{
				PlaySoundObject(101, 1);
				SetFlash(0, 0, 2);
				npc->act_no = 27;
				npc->ani_no = 7;
			}

			break;

		case 27:
			if (++npc->act_wait == 50)
			{
				npc->act_no = 0;
				npc->ani_no = 0;
			}

			break;

		case 30:
			npc->act_no = 31;
			npc->ani_no = 3;
			npc->ani_wait = 0;
			// Fallthrough
		case 31:
			if (++npc->ani_wait > 10)
			{
				npc->act_no = 32;
				npc->ani_no = 4;
				npc->ani_wait = 0;
			}

			break;

		case 32:
			if (++npc->ani_wait > 100)
			{
				npc->act_no = 1;
				npc->ani_no = 2;
			}

			break;

		case 40:
			npc->act_no = 41;
			npc->act_wait = 0;
			// Fallthrough
		case 41:
			npc->ani_no = 4;

			switch (++npc->act_wait)
			{
				case 30:
				case 40:
				case 50:
					SetNpChar(11, npc->x + 0x1000, npc->y - 0x1000, 0x600, Random(-0x200, 0), 0, 0, 0x100);
					PlaySoundObject(33, 1);
					break;
			}

			if (npc->act_wait > 50)
				npc->act_no = 0;

			break;

		case 50:
			npc->ani_no = 8;
			break;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rcLeft[9];
	RECT rcRight[9];

	rcLeft[0] = {80, 0, 96, 16};
	rcLeft[1] = {96, 0, 112, 16};
	rcLeft[2] = {112, 0, 128, 16};
	rcLeft[3] = {128, 0, 144, 16};
	rcLeft[4] = {144, 0, 160, 16};
	rcLeft[5] = {160, 0, 176, 16};
	rcLeft[6] = {176, 0, 192, 16};
	rcLeft[7] = {144, 0, 160, 16};
	rcLeft[8] = {208, 64, 224, 80};

	rcRight[0] = {80, 16, 96, 32};
	rcRight[1] = {96, 16, 112, 32};
	rcRight[2] = {112, 16, 128, 32};
	rcRight[3] = {128, 16, 144, 32};
	rcRight[4] = {144, 16, 160, 32};
	rcRight[5] = {160, 16, 176, 32};
	rcRight[6] = {176, 16, 192, 32};
	rcRight[7] = {144, 16, 160, 32};
	rcRight[8] = {208, 80, 224, 96};

	if (npc->act_no == 11)
	{
		if (npc->ani_wait)
		{
			--npc->ani_wait;
			npc->ani_no = 1;
		}
		else
		{
			if (Random(0, 100) == 1)
				npc->ani_wait = 30;

			npc->ani_no = 0;
		}
	}

	if (npc->act_no == 14)
	{
		if (npc->ani_wait)
		{
			--npc->ani_wait;
			npc->ani_no = 3;
		}
		else
		{
			if (Random(0, 100) == 1)
				npc->ani_wait = 30;

			npc->ani_no = 2;
		}
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Igor (cutscene)
void ActNpc083(NPCHAR *npc)
{
	RECT rcLeft[8];
	RECT rcRight[8];

	rcLeft[0] = {0, 0, 40, 40};
	rcLeft[1] = {40, 0, 80, 40};
	rcLeft[2] = {80, 0, 120, 40};
	rcLeft[3] = {0, 0, 40, 40};
	rcLeft[4] = {120, 0, 160, 40};
	rcLeft[5] = {0, 0, 40, 40};
	rcLeft[6] = {160, 0, 200, 40};
	rcLeft[7] = {200, 0, 240, 40};

	rcRight[0] = {0, 40, 40, 80};
	rcRight[1] = {40, 40, 80, 80};
	rcRight[2] = {80, 40, 120, 80};
	rcRight[3] = {0, 40, 40, 80};
	rcRight[4] = {120, 40, 160, 80};
	rcRight[5] = {0, 40, 40, 80};
	rcRight[6] = {160, 40, 200, 80};
	rcRight[7] = {200, 40, 240, 80};

	switch (npc->act_no)
	{
		case 0:
			npc->xm = 0;
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 5)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;

		case 2:
			npc->act_no = 3;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough
		case 3:
			if (++npc->ani_wait > 3)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			if (npc->direct == 0)
				npc->xm = -0x200;
			else
				npc->xm = 0x200;

			break;

		case 4:
			npc->xm = 0;
			npc->act_no = 5;
			npc->act_wait = 0;
			npc->ani_no = 6;
			// Fallthrough
		case 5:
			if (++npc->act_wait > 10)
			{
				npc->act_wait = 0;
				npc->act_no = 6;
				npc->ani_no = 7;
				PlaySoundObject(70, 1);
			}

			break;

		case 6:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 0;
				npc->ani_no = 0;
			}

			break;

		case 7:
			npc->act_no = 1;
			break;
	}

	npc->ym += 0x40;
	if ( npc->ym > 0x5FF )
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Basu projectile (Egg Corridor)
void ActNpc084(NPCHAR *npc)
{
	if (npc->flag & 0xFF)
	{
		SetCaret(npc->x, npc->y, 2, 0);
		npc->cond = 0;
	}

	npc->y += npc->ym;
	npc->x += npc->xm;

	RECT rect_left[4];

	rect_left[0] = {48, 48, 64, 64};
	rect_left[1] = {64, 48, 80, 64};
	rect_left[2] = {48, 64, 64, 80};
	rect_left[3] = {64, 64, 80, 80};

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 3)
		npc->ani_no = 0;

	npc->rect = rect_left[npc->ani_no];

	if (++npc->count1 > 300)
	{
		SetCaret(npc->x, npc->y, 2, 0);
		npc->cond = 0;
	}
}

// Terminal
void ActNpc085(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {256, 96, 272, 120};
	rcLeft[1] = {256, 96, 272, 120};
	rcLeft[2] = {272, 96, 288, 120};

	rcRight[0] = {256, 96, 272, 120};
	rcRight[1] = {288, 96, 304, 120};
	rcRight[2] = {304, 96, 320, 120};

	switch(npc->act_no)
	{
		case 0:
			npc->ani_no = 0;

			if (npc->x - 0x1000 < gMC.x && npc->x + 0x1000 > gMC.x && npc->y - 0x2000 < gMC.y && npc->y + 0x1000 > gMC.y)
			{
				PlaySoundObject(43, 1);
				npc->act_no = 1;
			}

			break;

		case 1:
			if (++npc->ani_no > 2)
				npc->ani_no = 1;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Missile
void ActNpc086(NPCHAR *npc)
{
	RECT rect1[2];
	RECT rect3[2];
	RECT rcLast[1];

	rect1[0] = {0, 80, 16, 96};
	rect1[1] = {16, 80, 32, 96};

	rect3[0] = {0, 112, 16, 128};
	rect3[1] = {16, 112, 32, 128};

	rcLast[0] = {16, 0, 32, 16};

	if (npc->direct == 0)
	{
		if (++npc->ani_wait > 2)
		{
			npc->ani_wait = 0;
			++npc->ani_no;
		}

		if (npc->ani_no > 1)
			npc->ani_no = 0;
	}

	if (gBack.type == 5 || gBack.type == 6)
	{
		if (npc->act_no == 0)
		{
			npc->act_no = 1;
			npc->ym = Random(-0x20, 0x20);
			npc->xm = Random(0x7F, 0x100);
		}

		npc->xm -= 8;

		if (npc->x < 0xA000)
			npc->cond = 0;

		if (npc->x < -0x600)
			npc->x = -0x600;

		if (npc->flag & 1)
			npc->xm = 0x100;

		if (npc->flag & 2)
			npc->ym = 0x40;

		if (npc->flag & 8)
			npc->ym = -0x40;

		npc->x += npc->xm;
		npc->y += npc->ym;
	}

	switch (npc->exp)
	{
		case 1:
			npc->rect = rect1[npc->ani_no];
			break;

		case 3:
			npc->rect = rect3[npc->ani_no];
			break;
	}

	if (npc->direct == 0)
		++npc->count1;

	if (npc->count1 > 550)
		npc->cond = 0;

	if (npc->count1 > 500 && npc->count1 / 2 % 2)
		npc->rect.right = 0;

	if (npc->count1 > 547)
		npc->rect = rcLast[0];;
}

// Heart
void ActNpc087(NPCHAR *npc)
{
	RECT rect2[5];
	RECT rect6[2];
	RECT rcLast[1];

	rect2[0] = {32, 80, 48, 96};
	rect2[1] = {48, 80, 64, 96};

	rect6[0] = {64, 80, 80, 96};
	rect6[1] = {80, 80, 96, 96};

	rcLast[0] = {16, 0, 32, 16};

	if (npc->direct == 0)
	{
		if (++npc->ani_wait > 2)
		{
			npc->ani_wait = 0;
			++npc->ani_no;
		}

		if (npc->ani_no > 1)
			npc->ani_no = 0;
	}

	if (gBack.type == 5 || gBack.type == 6)
	{
		if (npc->act_no == 0)
		{
			npc->act_no = 1;
			npc->ym = Random(-0x20, 0x20);
			npc->xm = Random(0x7F, 0x100);
		}

		npc->xm -= 8;

		if (npc->x < 0xA000)
			npc->cond = 0;

		if (npc->x < -0x600)
			npc->x = -0x600;

		if (npc->flag & 1)
			npc->xm = 0x100;

		if (npc->flag & 2)
			npc->ym = 0x40;

		if (npc->flag & 8)
			npc->ym = -0x40;

		npc->x += npc->xm;
		npc->y += npc->ym;
	}

	switch (npc->exp)
	{
		case 2:
			npc->rect = rect2[npc->ani_no];
			break;

		case 6:
			npc->rect = rect6[npc->ani_no];
			break;
	}

	if (npc->direct == 0)
		++npc->count1;

	if (npc->count1 > 550)
		npc->cond = 0;

	if (npc->count1 > 500 && npc->count1 / 2 % 2)
		npc->rect.right = 0;

	if (npc->count1 > 547)
		npc->rect = rcLast[0];
}

// Cage
void ActNpc091(NPCHAR *npc)
{
	RECT rect[1];

	rect[0] = {96, 88, 128, 112};

	if (npc->act_no == 0)
	{
		++npc->act_no;
		npc->y += 0x10 * 0x200;
	}

	npc->rect = rect[0];
}
