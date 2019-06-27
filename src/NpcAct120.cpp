#include "NpcAct.h"

#include "WindowsWrapper.h"

#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

// Colon (1)
void ActNpc120(NPCHAR *npc)
{
	RECT rect[2] = {
		{64, 0, 80, 16},
		{64, 16, 80, 32},
	};

	if (npc->direct == 0)
		npc->rect = rect[0];
	else
		npc->rect = rect[1];
}

// Colon (2)
void ActNpc121(NPCHAR *npc)
{
	RECT rect[3] = {
		{0, 0, 16, 16},
		{16, 0, 32, 16},
		{112, 0, 128, 16},
	};

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
	else
	{
		npc->rect = rect[2];

		if (++npc->act_wait > 100)
		{
			npc->act_wait = 0;
			SetCaret(npc->x, npc->y, 5, 0);
		}
	}
}

// Colon (attacking)
void ActNpc122(NPCHAR *npc)
{
	RECT rcLeft[10] = {
		{0, 0, 16, 16},
		{16, 0, 32, 16},
		{32, 0, 48, 16},
		{0, 0, 16, 16},
		{48, 0, 64, 16},
		{0, 0, 16, 16},
		{80, 0, 96, 16},
		{96, 0, 112, 16},
		{112, 0, 128, 16},
		{128, 0, 144, 16},
	};

	RECT rcRight[10] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{32, 16, 48, 32},
		{0, 16, 16, 32},
		{48, 16, 64, 32},
		{0, 16, 16, 32},
		{80, 16, 96, 32},
		{96, 16, 112, 32},
		{112, 16, 128, 32},
		{128, 16, 144, 32},
	};

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

			if (npc->x - 0x4000 < gMC.x && npc->x + 0x4000 > gMC.x && npc->y - 0x4000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (npc->x > gMC.x)
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

			if (npc->x > gMC.x)
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

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;
	npc->x += npc->xm;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Curly boss projectile
void ActNpc123(NPCHAR *npc)
{
	RECT rect[4] = {
		{192, 0, 208, 16},
		{208, 0, 224, 16},
		{224, 0, 240, 16},
		{240, 0, 256, 16},
	};

	BOOL bBreak = FALSE;

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
						bBreak = TRUE;
					break;

				case 1:
					if (npc->flag & 2)
						bBreak = TRUE;
					break;

				case 2:
					if (npc->flag & 4)
						bBreak = TRUE;
					break;

				case 3:
					if (npc->flag & 8)
						bBreak = TRUE;
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

// Sunstone
void ActNpc124(NPCHAR *npc)
{
	RECT rect[2] = {
		{160, 0, 192, 32},
		{192, 0, 224, 32},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->x += 0x1000;
			npc->y += 0x1000;
			// Fallthrough
		case 1:
			npc->bits &= ~8;
			npc->ani_no = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 1;
			npc->act_wait = 0;
			npc->bits |= 8;
			// Fallthrough
		case 11:
			switch (npc->direct)
			{
				case 0:
					npc->x -= 0x80;
					break;

				case 1:
					npc->y -= 0x80;
					break;

				case 2:
					npc->x += 0x80;
					break;

				case 3:
					npc->y += 0x80;
					break;
			}
			if (++npc->act_wait % 8 == 0)
				PlaySoundObject(26, 1);

			SetQuake(20);
			break;
	}

	npc->rect = rect[npc->ani_no];
}

// Hidden item
void ActNpc125(NPCHAR *npc)
{
	if (npc->life < 990)
	{
		SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
		PlaySoundObject(70, 1);

		if (npc->direct == 0)
			SetNpChar(87, npc->x, npc->y, 0, 0, 2, 0, 0);
		else
			SetNpChar(86, npc->x, npc->y, 0, 0, 2, 0, 0);

		npc->cond = 0;
	}

	RECT rc[2] = {
		{0, 96, 16, 112},
		{16, 96, 32, 112},
	};

	if (npc->direct == 0)
		npc->rect = rc[0];
	else
		npc->rect = rc[1];
}

// Puppy (running)
void ActNpc126(NPCHAR *npc)
{
	RECT rcLeft[6] = {
		{48, 144, 64, 160},
		{64, 144, 80, 160},
		{48, 144, 64, 160},
		{80, 144, 96, 160},
		{96, 144, 112, 160},
		{112, 144, 128, 160},
	};

	RECT rcRight[6] = {
		{48, 160, 64, 176},
		{64, 160, 80, 176},
		{48, 160, 64, 176},
		{80, 160, 96, 176},
		{96, 160, 112, 176},
		{112, 160, 128, 176},
	};

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

			if (npc->x - 0xC000 < gMC.x && npc->x + 0xC000 > gMC.x && npc->y - 0x4000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			if (npc->x - 0x4000 < gMC.x && npc->x + 0x4000 > gMC.x && npc->y - 0x4000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (npc->x > gMC.x)
					npc->direct = 2;
				else
					npc->direct = 0;

				npc->act_no = 10;
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
			npc->ani_no = 4;
			npc->ani_wait = 0;
			// Fallthrough
		case 11:
			if (npc->flag & 8)
			{
				if (++npc->ani_wait > 2)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 5)
					npc->ani_no = 4;
			}
			else
			{
				npc->ani_no = 5;
				npc->ani_wait = 0;
			}

			if (npc->xm < 0 && npc->flag & 1)
			{
				npc->xm /= -2;
				npc->direct = 2;
			}

			if (npc->xm > 0 && npc->flag & 4)
			{
				npc->xm /= -2;
				npc->direct = 0;
			}

			if (npc->direct == 0)
				npc->xm -= 0x40;
			else
				npc->xm += 0x40;

			if (npc->xm > 0x5FF)
				npc->xm = 0x400;

			if (npc->xm < -0x5FF)
				npc->xm = -0x400;

			break;
	}

	if (gKeyTrg & gKeyDown)
		npc->bits |= 0x2000;
	else
		npc->bits &= ~0x2000;

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

// Machine gun trail (Level 2)
void ActNpc127(NPCHAR *npc)
{
	RECT rcV[3] = {
		{112, 48, 128, 64},
		{112, 64, 128, 80},
		{112, 80, 128, 96},
	};

	RECT rcH[3] = {
		{64, 80, 80, 96},
		{80, 80, 96, 96},
		{96, 80, 112, 96},
	};

	if (++npc->ani_wait > 0)
	{
		npc->ani_wait = 0;
		if (++npc->ani_no > 2)
			npc->cond = 0;
	}

	if (npc->direct == 0)
		npc->rect = rcH[npc->ani_no];
	else
		npc->rect = rcV[npc->ani_no];
}

// Machine gun trail (Level 3)
void ActNpc128(NPCHAR *npc)
{
	RECT rcLeft[5] = {
		{0, 0, 0, 0},
		{176, 16, 184, 32},
		{184, 16, 192, 32},
		{192, 16, 200, 32},
		{200, 16, 208, 32},
	};

	RECT rcRight[5] = {
		{0, 0, 0, 0},
		{232, 16, 240, 32},
		{224, 16, 232, 32},
		{216, 16, 224, 32},
		{208, 16, 216, 32},
	};

	RECT rcUp[5] = {
		{0, 0, 0, 0},
		{176, 32, 192, 40},
		{176, 40, 192, 48},
		{192, 32, 208, 40},
		{192, 40, 208, 48},
	};

	RECT rcDown[5] = {
		{0, 0, 0, 0},
		{208, 32, 224, 40},
		{208, 40, 224, 48},
		{224, 32, 232, 40},
		{224, 40, 232, 48},
	};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;

		if (npc->direct == 0 || npc->direct == 2)
		{
			npc->view.front = 0x800;
			npc->view.top = 0x1000;
		}
		else
		{
			npc->view.front = 0x1000;
			npc->view.top = 0x800;
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

// Fireball trail (Level 2 & 3)
void ActNpc129(NPCHAR *npc)
{
	RECT rect[18] = {
		{0x80, 0x30, 0x90, 0x40},
		{0x90, 0x30, 0xA0, 0x40},
		{0xA0, 0x30, 0xB0, 0x40},
		{0x80, 0x40, 0x90, 0x50},
		{0x90, 0x40, 0xA0, 0x50},
		{0xA0, 0x40, 0xB0, 0x50},
		{0x80, 0x50, 0x90, 0x60},
		{0x90, 0x50, 0xA0, 0x60},
		{0xA0, 0x50, 0xB0, 0x60},
		{0xB0, 0x30, 0xC0, 0x40},
		{0xC0, 0x30, 0xD0, 0x40},
		{0xD0, 0x30, 0xE0, 0x40},
		{0xB0, 0x40, 0xC0, 0x50},
		{0xC0, 0x40, 0xD0, 0x50},
		{0xD0, 0x40, 0xE0, 0x50},
		{0xB0, 0x50, 0xC0, 0x60},
		{0xC0, 0x50, 0xD0, 0x60},
		{0xD0, 0x50, 0xE0, 0x60},
	};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		if (++npc->ani_no > 2)
			npc->cond = 0;
	}

	npc->y += npc->ym;

	npc->rect = rect[3 * npc->direct + npc->ani_no];
}

// Puppy (sitting, wagging tail)
void ActNpc130(NPCHAR *npc)
{
	RECT rcLeft[4] = {
		{48, 144, 64, 160},
		{64, 144, 80, 160},
		{48, 144, 64, 160},
		{80, 144, 96, 160},
	};

	RECT rcRight[4] = {
		{48, 160, 64, 176},
		{64, 160, 80, 176},
		{48, 160, 64, 176},
		{80, 160, 96, 176},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->bits |= 0x2000;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (npc->x - 0x8000 < gMC.x && npc->x + 0x8000 > gMC.x && npc->y - 0x4000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (++npc->ani_wait > 3)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 3)
					npc->ani_no = 2;
			}

			if (npc->x - 0xC000 < gMC.x && npc->x + 0xC000 > gMC.x && npc->y - 0x4000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (npc->x > gMC.x)
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

// Puppy (sleeping)
void ActNpc131(NPCHAR *npc)
{
	RECT rcLeft[1] = {144, 144, 160, 160};
	RECT rcRight[1] = {144, 160, 160, 176};

	if (++npc->act_wait > 100)
	{
		npc->act_wait = 0;
		SetCaret(npc->x, npc->y, 5, 0);
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Puppy (barking)
void ActNpc132(NPCHAR *npc)
{
	RECT rcLeft[5] = {
		{48, 144, 64, 160},
		{64, 144, 80, 160},
		{96, 144, 112, 160},
		{96, 144, 112, 160},
		{128, 144, 144, 160},
	};

	RECT rcRight[5] = {
		{48, 160, 64, 176},
		{64, 160, 80, 176},
		{96, 160, 112, 176},
		{96, 160, 112, 176},
		{128, 160, 144, 176},
	};

	if (npc->act_no < 100)
	{
		if (gMC.x < npc->x)
			npc->direct = 0;
		else
			npc->direct = 2;
	}

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

			if (npc->x - 0x8000 < gMC.x && npc->x + 0x8000 > gMC.x && npc->y - 0x2000 < gMC.y && npc->y + 0x2000 > gMC.y)
			{
				if (++npc->ani_wait > 4)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 4)
					npc->ani_no = 2;

				if (npc->ani_no == 4 && npc->ani_wait == 0)
					PlaySoundObject(105, 1);
			}
			else
			{
				if (npc->ani_no == 4)
					npc->ani_no = 2;
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
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 11:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 12;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 12:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 11;
				npc->ani_no = 0;
			}

			break;

		case 100:
			npc->act_no = 101;
			npc->count1 = 0;
			// Fallthrough
		case 101:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
			{
				if (npc->count1 < 3)
				{
					npc->ani_no = 2;
					++npc->count1;
				}
				else
				{
					npc->ani_no = 0;
					npc->count1 = 0;
				}
			}

			if (npc->ani_no == 4 && npc->ani_wait == 0)
				PlaySoundObject(105, 1);

			break;

		case 120:
			npc->ani_no = 0;
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

// Jenka
void ActNpc133(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{176, 32, 192, 48},
		{192, 32, 208, 48},
	};

	RECT rcRight[2] = {
		{176, 48, 192, 64},
		{192, 48, 208, 64},
	};

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

// Armadillo
void ActNpc134(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{224, 0, 256, 16},
		{256, 0, 288, 16},
		{288, 0, 320, 16},
	};

	RECT rcRight[3] = {
		{224, 16, 256, 32},
		{256, 16, 288, 32},
		{288, 16, 320, 32},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 2;
			npc->bits &= ~0x20;
			npc->bits |= 4;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x - 0x28000 && gMC.x < npc->x + 0x28000 && gMC.y > npc->y - 0x14000 && gMC.y < npc->y + 0x8000)	// TODO: Maybe do something about this for widescreen/tallscreen?
			{
				npc->act_no = 10;
				npc->bits |= 0x20;
				npc->bits &= ~4;
			}

			break;

		case 10:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->direct == 0 && npc->flag & 1)
				npc->direct = 2;
			if (npc->direct == 2 && npc->flag & 4)
				npc->direct = 0;

			if (npc->direct == 0)
				npc->x -= 0x100;
			else
				npc->x += 0x100;

			if (CountArmsBullet(6))
			{
				npc->act_no = 20;
				npc->act_wait = 0;
				npc->ani_no = 2;
				npc->bits &= ~0x20;
				npc->bits |= 4;
			}

			break;

		case 20:
			if (++npc->act_wait > 100)
			{
				npc->act_no = 10;
				npc->ani_no = 0;
				npc->ani_wait = 0;
				npc->bits |= 0x20;
				npc->bits &= ~4;
			}

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Skeleton
void ActNpc135(NPCHAR *npc)
{
	unsigned char deg;
	int ym;
	int xm;

	RECT rcLeft[2] = {
		{256, 32, 288, 64},
		{288, 32, 320, 64},
	};

	RECT rcRight[2] = {
		{256, 64, 288, 96},
		{288, 64, 320, 96},
	};

	if (gMC.x < npc->x - 0x2C000 || gMC.x > npc->x + 0x2C000 || gMC.y < npc->y - 0x14000 || gMC.y > npc->y + 0x8000)
		npc->act_no = 0;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x - 0x28000 && gMC.x < npc->x + 0x28000 && gMC.y > npc->y - 0x14000 && gMC.y < npc->y + 0x8000)
				npc->act_no = 10;

			if (npc->flag & 8)
				npc->ani_no = 0;

			break;

		case 10:
			npc->xm = 0;
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->ani_no = 0;
			// Fallthrough
		case 11:
			if (++npc->act_wait >= 5 && npc->flag & 8)
			{
				npc->act_no = 20;
				npc->ani_no = 1;
				npc->count1 = 0;
				npc->ym = -0x200 * Random(1, 3);

				if (npc->shock)
				{
					if (npc->x < gMC.x)
						npc->xm -= 0x100;
					else
						npc->xm += 0x100;
				}
				else
				{
					if (npc->x < gMC.x)
						npc->xm += 0x100;
					else
						npc->xm -= 0x100;
				}
			}

			break;

		case 20:
			if (npc->ym > 0 && npc->count1 == 0)
			{
				++npc->count1;
				deg = GetArktan(npc->x - gMC.x, npc->y + 0x800 - gMC.y);
				ym = 2 * GetSin(deg);
				xm = 2 * GetCos(deg);
				SetNpChar(50, npc->x, npc->y, xm, ym, 0, 0, 0x180);
				PlaySoundObject(39, 1);
			}

			if (npc->flag & 8)
			{
				npc->act_no = 10;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->act_no >= 10)
	{
		if (npc->x > gMC.x)
			npc->direct = 0;
		else
			npc->direct = 2;
	}

	npc->ym += 0x33;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	if (npc->xm > 0x5FF)
		npc->xm = 0x5FF;
	if (npc->xm < -0x5FF)
		npc->xm = -0x5FF;

	npc->y += npc->ym;
	npc->x += npc->xm;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Puppy (carried)
void ActNpc136(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{48, 144, 64, 160},
		{64, 144, 80, 160},
	};

	RECT rcRight[2] = {
		{48, 160, 64, 176},
		{64, 160, 80, 176},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~0x2000;
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

	if (gMC.direct == 0)
		npc->direct = 0;
	else
		npc->direct = 2;

	npc->y = gMC.y - 0x1400;

	if (npc->direct == 0)
	{
		npc->x = gMC.x + 0x800;
		npc->rect = rcLeft[npc->ani_no];
	}
	else
	{
		npc->x = gMC.x - 0x800;
		npc->rect = rcRight[npc->ani_no];
	}

	if (gMC.ani_no % 2)
		++npc->rect.top;
}

// Large door (frame)
void ActNpc137(NPCHAR *npc)
{
	RECT rc = {96, 136, 128, 188};

	npc->rect = rc;
}

// Large door (door)
void ActNpc138(NPCHAR *npc)
{
	RECT rcLeft = {96, 112, 112, 136};
	RECT rcRight = {112, 112, 128, 136};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 0)
			{
				npc->rect = rcLeft;
				npc->x += 0x1000;
			}
			else
			{
				npc->rect = rcRight;
				npc->x -= 0x1000;
			}

			npc->tgt_x = npc->x;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 1;
			npc->act_wait = 0;
			npc->bits |= 8;
			// Fallthrough
		case 11:
			if (++npc->act_wait % 8 == 0)
				PlaySoundObject(26, 1);

			if (npc->direct == 0)
			{
				npc->rect = rcLeft;
				npc->rect.left += npc->act_wait / 8;
			}
			else
			{
				npc->x = npc->tgt_x + ((npc->act_wait / 8) * 0x200);
				npc->rect = rcRight;
				npc->rect.right -= npc->act_wait / 8;
			}

			if (npc->act_wait == 104)
				npc->cond = 0;

			break;
	}
}

// Doctor
void ActNpc139(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{0, 128, 24, 160},
		{24, 128, 48, 160},
		{48, 128, 72, 160},
	};

	RECT rcRight[3] = {
		{0, 160, 24, 192},
		{24, 160, 48, 192},
		{48, 160, 72, 192},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->xm = 0;
			npc->ym = 0;
			npc->y -= 0x1000;
			// Fallthrough
		case 1:
			if (npc->flag & 8)
				npc->ani_no = 0;
			else
				npc->ani_no = 2;

			npc->ym += 0x40;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 1;
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
				++npc->count1;
			}

			if (npc->count1 > 8)
			{
				npc->ani_no = 0;
				npc->act_no = 1;
			}

			break;

		case 20:
			npc->act_no = 21;
			npc->act_wait = 0;
			npc->ani_no = 2;
			npc->tgt_y = npc->y - 0x4000;
			// Fallthrough
		case 21:
			if (npc->y < npc->tgt_y)
				npc->ym += 0x20;
			else
				npc->ym -= 0x20;

			if (npc->ym > 0x200)
				npc->ym = 0x200;
			if (npc->ym < -0x200)
				npc->ym = -0x200;

			break;

		case 30:
			npc->act_no = 31;
			npc->xm = 0;
			npc->ym = 0;
			npc->act_wait = 2 * (npc->rect.bottom - npc->rect.top);
			PlaySoundObject(29, 1);
			// Fallthrough
		case 31:
			--npc->act_wait;
			npc->ani_no = 0;

			if (npc->act_wait == 0)
				npc->cond = 0;

			break;

		case 40:
			npc->act_no = 41;
			npc->act_wait = 0;
			npc->xm = 0;
			npc->ym = 0;
			PlaySoundObject(29, 1);
			// Fallthrough
		case 41:
			npc->ani_no = 2;

			if (++npc->act_wait < 64)
				break;

			npc->act_no = 20;
			break;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (npc->act_no == 31 || npc->act_no == 41)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 2;

		if (npc->act_wait / 2 % 2)
			++npc->rect.left;
	}
}
