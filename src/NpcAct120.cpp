#include "NpcAct.h"

#include "WindowsWrapper.h"

#include "Back.h"
#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

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

//Sunstone
void ActNpc124(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {160, 0, 192, 32};
	rect[1] = {192, 0, 224, 32};

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

//Puppy (running)
void ActNpc126(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {48, 144, 64, 160};
	rcLeft[1] = {64, 144, 80, 160};
	rcLeft[2] = {48, 144, 64, 160};
	rcLeft[3] = {80, 144, 96, 160};
	rcLeft[4] = {96, 144, 112, 160};
	rcLeft[5] = {112, 144, 128, 160};

	rcRight[0] = {48, 160, 64, 176};
	rcRight[1] = {64, 160, 80, 176};
	rcRight[2] = {48, 160, 64, 176};
	rcRight[3] = {80, 160, 96, 176};
	rcRight[4] = {96, 160, 112, 176};
	rcRight[5] = {112, 160, 128, 176};

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

			if (gMC.x > npc->x - 0xC000 && gMC.x < npc->x + 0xC000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			if (gMC.x > npc->x - 0x4000 && gMC.x < npc->x + 0x4000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
			{
				if (gMC.x < npc->x)
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

//Puppy (sitting, wagging tail)
void ActNpc130(NPCHAR *npc)
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0] = {48, 144, 64, 160};
	rcLeft[1] = {64, 144, 80, 160};
	rcLeft[2] = {48, 144, 64, 160};
	rcLeft[3] = {80, 144, 96, 160};

	rcRight[0] = {48, 160, 64, 176};
	rcRight[1] = {64, 160, 80, 176};
	rcRight[2] = {48, 160, 64, 176};
	rcRight[3] = {80, 160, 96, 176};

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

			if (gMC.x > npc->x - 0x8000 && gMC.x < npc->x + 0x8000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
			{
				if (++npc->ani_wait > 3)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 3)
					npc->ani_no = 2;
			}

			if (gMC.x > npc->x - 0xC000 && gMC.x < npc->x + 0xC000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
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

//Puppy (sleeping)
void ActNpc131(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = {144, 144, 160, 160};
	rcRight[0] = {144, 160, 160, 176};

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

//Puppy (barking)
void ActNpc132(NPCHAR *npc)
{
	RECT rcLeft[5];
	RECT rcRight[5];

	rcLeft[0] = {48, 144, 64, 160};
	rcLeft[1] = {64, 144, 80, 160};
	rcLeft[2] = {96, 144, 112, 160};
	rcLeft[3] = {96, 144, 112, 160};
	rcLeft[4] = {128, 144, 144, 160};

	rcRight[0] = {48, 160, 64, 176};
	rcRight[1] = {64, 160, 80, 176};
	rcRight[2] = {96, 160, 112, 176};
	rcRight[3] = {96, 160, 112, 176};
	rcRight[4] = {128, 160, 144, 176};

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

			if (gMC.x > npc->x - 0x8000 && gMC.x < npc->x + 0x8000 && gMC.y > npc->y - 0x2000 && gMC.y < npc->y + 0x2000)
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

//Jenka
void ActNpc133(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {176, 32, 192, 48};
	rcLeft[1] = {192, 32, 208, 48};

	rcRight[0] = {176, 48, 192, 64};
	rcRight[1] = {192, 48, 208, 64};

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

//Armadillo
void ActNpc134(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {224, 0, 256, 16};
	rcLeft[1] = {256, 0, 288, 16};
	rcLeft[2] = {288, 0, 320, 16};

	rcRight[0] = {224, 16, 256, 32};
	rcRight[1] = {256, 16, 288, 32};
	rcRight[2] = {288, 16, 320, 32};

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

//Skeleton
void ActNpc135(NPCHAR *npc)
{
	RECT rcRight[2];
	RECT rcLeft[2];

	rcLeft[0] = {256, 32, 288, 64};
	rcLeft[1] = {288, 32, 320, 64};

	rcRight[0] = {256, 64, 288, 96};
	rcRight[1] = {288, 64, 320, 96};

	if (gMC.x < npc->x - 0x2C000 || gMC.x > npc->x + 0x2C000 || gMC.y < npc->y - 0x14000 || gMC.y > npc->y + 0x8000)
		npc->act_no = 0;

	switch ( npc->act_no )
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

				if (npc->shock == 0)
				{
					if (gMC.x > npc->x)
						npc->xm += 0x100;
					else
						npc->xm -= 0x100;
				}
				else
				{
					if (gMC.x > npc->x)
						npc->xm -= 0x100;
					else
						npc->xm += 0x100;
				}
			}

			break;

		case 20:
			if (npc->ym > 0 && npc->count1 == 0)
			{
				++npc->count1;
				const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y + 0x800 - gMC.y);
				const int ym = 2 * GetSin(deg);
				const int xm = 2 * GetCos(deg);
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
		if (gMC.x < npc->x)
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

//Puppy (carried)
void ActNpc136(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {48, 144, 64, 160};
	rcLeft[1] = {64, 144, 80, 160};

	rcRight[0] = {48, 160, 64, 176};
	rcRight[1] = {64, 160, 80, 176};

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

//Large door (frame)
void ActNpc137(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {96, 136, 128, 188};

	npc->rect = rc[0];
}

//Large door (door)
void ActNpc138(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = {96, 112, 112, 136};
	rcRight[0] = {112, 112, 128, 136};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 0)
			{
				npc->rect = rcLeft[0];
				npc->x += 0x1000;
			}
			else
			{
				npc->rect = rcRight[0];
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
				npc->rect = rcLeft[0];
				npc->rect.left += npc->act_wait / 8;
			}
			else
			{
				npc->x = npc->tgt_x + ((npc->act_wait / 8) * 0x200);
				npc->rect = rcRight[0];
				npc->rect.right -= npc->act_wait / 8;
			}

			if (npc->act_wait == 104)
				npc->cond = 0;

			break;
	}
}

//Doctor
void ActNpc139(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {0, 128, 24, 160};
	rcLeft[1] = {24, 128, 48, 160};
	rcLeft[2] = {48, 128, 72, 160};

	rcRight[0] = {0, 160, 24, 192};
	rcRight[1] = {24, 160, 48, 192};
	rcRight[2] = {48, 160, 72, 192};

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

			if (++npc->act_wait >= 64)
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
