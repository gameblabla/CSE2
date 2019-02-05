#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"

//Colon (1)
void ActNpc120(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {64, 0, 80, 16};
	rect[1] = {64, 16, 80, 32};

	if (npc->direct == 0)
		npc->rect = rect[0];
	else
		npc->rect = rect[1];
}

//Colon (2)
void ActNpc121(NPCHAR *npc)
{
	RECT rect[3];

	rect[0] = {0, 0, 16, 16};
	rect[1] = {16, 0, 32, 16};
	rect[2] = {112, 0, 128, 16};

	if (npc->direct == 0)
	{
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

		npc->rect = rect[npc->ani_no];
	}

	npc->rect = rect[2];

	if (++npc->act_wait > 100)
	{
		npc->act_wait = 0;
		SetCaret(npc->x, npc->y, 5, 0);
	}
}

//Colon (attacking)
void ActNpc122(NPCHAR *npc)
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};
	rcLeft[2] = {32, 0, 48, 16};
	rcLeft[3] = {0, 0, 16, 16};
	rcLeft[4] = {48, 0, 64, 16};
	rcLeft[5] = {0, 0, 16, 16};
	rcLeft[6] = {80, 0, 96, 16};
	rcLeft[7] = {96, 0, 112, 16};
	rcLeft[8] = {112, 0, 128, 16};
	rcLeft[9] = {128, 0, 144, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};
	rcRight[2] = {32, 16, 48, 32};
	rcRight[3] = {0, 16, 16, 32};
	rcRight[4] = {48, 16, 64, 32};
	rcRight[5] = {0, 16, 16, 32};
	rcRight[6] = {80, 16, 96, 32};
	rcRight[7] = {96, 16, 112, 32};
	rcRight[8] = {112, 16, 128, 32};
	rcRight[9] = {128, 16, 144, 32};

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

			if (gMC.x > npc->x - 0x4000 && gMC.x < npc->x + 0x4000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;
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
			npc->life = 1000;
			npc->act_no = 11;
			npc->act_wait = Random(0, 50);
			npc->ani_no = 0;
			npc->damage = 0;
			// Fallthrough
		case 11:
			if (npc->act_wait)
				--npc->act_wait;
			else
				npc->act_no = 13;

			break;

		case 13:
			npc->act_no = 14;
			npc->act_wait = Random(0, 50);

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 14:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			if (npc->direct == 0)
				npc->xm -= 0x40;
			else
				npc->xm += 0x40;

			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->bits |= 0x20;
				npc->act_no = 15;
				npc->ani_no = 2;
				npc->ym = -0x200;
				npc->damage = 2;
			}

			break;

		case 15:
			if (npc->flag & 8)
			{
				npc->bits |= 0x20;
				npc->xm = 0;
				npc->act_no = 10;
				npc->damage = 0;
			}

			break;

		case 20:
			if (npc->flag & 8)
			{
				npc->xm = 0;
				npc->act_no = 21;
				npc->damage = 0;

				if (npc->ani_no == 6)
					npc->ani_no = 8;
				else
					npc->ani_no = 9;

				npc->act_wait = Random(300, 400);
			}

			break;

		case 21:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->bits |= 0x20;
				npc->life = 1000;
				npc->act_no = 11;
				npc->act_wait = Random(0, 50);
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->act_no > 10 && npc->act_no < 20 && npc->life != 1000)
	{
		npc->act_no = 20;
		npc->ym = -0x200;
		npc->ani_no = Random(6, 7);
		npc->bits &= ~0x20;
	}

	npc->ym += 0x20;

	if (npc->xm > 0x1FF)
		npc->xm = 0x1FF;
	if (npc->xm < -0x1FF)
		npc->xm = -0x1FF;

	if (npc->ym > 0x5FF )
		npc->ym = 0x5FF;

	npc->y += npc->ym;
	npc->x += npc->xm;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Curly boss projectile
void ActNpc123(NPCHAR *npc)
{
	RECT rect[4];

	rect[0] = {192, 0, 208, 16};
	rect[1] = {208, 0, 224, 16};
	rect[2] = {224, 0, 240, 16};
	rect[3] = {240, 0, 256, 16};

	bool bBreak = false;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			SetCaret(npc->x, npc->y, 3, 0);
			PlaySoundObject(32, 1);

			switch (npc->direct)
			{
				case 0:
					npc->xm = -0x1000;
					npc->ym = Random(-0x80, 0x80);
					break;
				case 1:
					npc->ym = -0x1000;
					npc->xm = Random(-0x80, 0x80);
					break;
				case 2:
					npc->xm = 0x1000;
					npc->ym = Random(-0x80, 0x80);
					break;
				case 3:
					npc->ym = 0x1000;
					npc->xm = Random(-0x80, 0x80);
					break;
			}

			break;

		case 1:
			switch (npc->direct)
			{
				case 0:
					if (npc->flag & 1)
						bBreak = true;
					break;
				case 1:
					if (npc->flag & 2)
						bBreak = true;
					break;
				case 2:
					if (npc->flag & 4)
						bBreak = true;
					break;
				case 3:
					if (npc->flag & 8)
						bBreak = true;
					break;
			}

			npc->x += npc->xm;
			npc->y += npc->ym;

			break;
	}

	if (bBreak)
	{
		SetCaret(npc->x, npc->y, 2, 2);
		PlaySoundObject(28, 1);
		npc->cond = 0;
	}

	npc->rect = rect[npc->direct];
}

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
