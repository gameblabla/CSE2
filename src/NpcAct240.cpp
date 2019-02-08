#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"
#include "Frame.h"
#include "Map.h"

//Mimiga (jailed)
void ActNpc240(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {160, 64, 176, 80};
	rcLeft[1] = {176, 64, 192, 80};
	rcLeft[2] = {192, 64, 208, 80};
	rcLeft[3] = {160, 64, 176, 80};
	rcLeft[4] = {208, 64, 224, 80};
	rcLeft[5] = {160, 64, 176, 80};

	rcRight[0] = {160, 80, 176, 96};
	rcRight[1] = {176, 80, 192, 96};
	rcRight[2] = {192, 80, 208, 96};
	rcRight[3] = {160, 80, 176, 96};
	rcRight[4] = {208, 80, 224, 96};
	rcRight[5] = {160, 80, 176, 96};

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
			else if (npc->direct == 2 && npc->flag & 4)
				npc->direct = 0;

			if (npc->direct == 0)
				npc->xm = -0x200u;
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

//Critter (Last Cave)
void ActNpc241(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};
	rcLeft[2] = {32, 0, 48, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};
	rcRight[2] = {32, 16, 48, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->y += 0x600;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (npc->act_wait >= 8 && gMC.x > npc->x - 0x12000 && gMC.x < npc->x + 0x12000 && gMC.y > npc->y - 0xA000 && gMC.y < npc->y + 0xA000)
			{
				npc->ani_no = 1;
			}
			else
			{
				if ( npc->act_wait < 8 )
					++npc->act_wait;

				npc->ani_no = 0;
			}

			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			if (npc->act_wait >= 8 && gMC.x > npc->x - 0xC000 && gMC.x < npc->x + 0xC000 && gMC.y > npc->y - 0xA000 && gMC.y < npc->y + 0xC000)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 3;
				npc->ani_no = 2;
				npc->ym = -0x5FF;
				PlaySoundObject(30, 1);

				if (npc->direct == 0)
					npc->xm = -0x200;
				else
					npc->xm = 0x200;
			}

			break;

		case 3:
			if (npc->flag & 8)
			{
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;
				PlaySoundObject(23, 1);
			}

			break;
	}

	npc->ym += 0x55;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Bat (Last Cave)
void ActNpc242(NPCHAR *npc)
{
	if (npc->x < 0 || npc->x > gMap.width * 0x2000)
	{
		VanishNpChar(npc);
		return;
	}

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			npc->act_wait = Random(0, 50);
			// Fallthrough
		case 1:
			if (npc->act_wait)
			{
				--npc->act_wait;
				break;
			}

			npc->act_no = 2;
			npc->ym = 0x400;
			// Fallthrough
		case 2:
			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			if (npc->tgt_y < npc->y)
				npc->ym -= 0x10;
			if (npc->tgt_y > npc->y)
				npc->ym += 0x10;

			if (npc->ym > 0x300)
				npc->ym = 0x300;
			if (npc->ym < -0x300)
				npc->ym = -0x300;

			break;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[4];
	RECT rect_right[4];

	rect_left[0] = {32, 32, 48, 48};
	rect_left[1] = {48, 32, 64, 48};
	rect_left[2] = {64, 32, 80, 48};
	rect_left[3] = {80, 32, 96, 48};

	rect_right[0] = {32, 48, 48, 64};
	rect_right[1] = {48, 48, 64, 64};
	rect_right[2] = {64, 48, 80, 64};
	rect_right[3] = {80, 48, 96, 64};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}

//Bat generator (Last Cave)
void ActNpc243(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(0, 500);
			// Fallthrough
		case 1:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->act_no = 0;
				SetNpChar(242, npc->x, npc->y + (Random(-0x20, 0x20) * 0x100), 0, 0, npc->direct, 0, 0x100);
			}

			break;
	}
}

//Lava drop
void ActNpc244(NPCHAR *npc)
{
	RECT rc = {96, 0, 104, 16};
	npc->ym += 0x40;
	bool bHit = false;

	if (npc->flag & 0xFF)
		bHit = true;

	if (npc->act_wait > 10 && npc->flag & 0x100)
		bHit = true;

	if (bHit)
	{
		for (int i = 0; i < 3; ++i)
			SetCaret(npc->x, npc->y + 0x800, 1, 2);

		if (npc->x > gMC.x - 0x20000 && npc->x < gMC.x + 0x20000 && npc->y > gMC.y - 0x14000 && npc->y < gMC.y + 0x14000)
			PlaySoundObject(21, 1);

		npc->cond = 0;
	}
	else
	{
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;

		npc->y += npc->ym;

		npc->rect = rc;
	}
}

//Lava drop generator
void ActNpc245(NPCHAR *npc)
{
	RECT rc[4];

	rc[0] = {0, 0, 0, 0};
	rc[1] = {104, 0, 112, 16};
	rc[2] = {112, 0, 120, 16};
	rc[3] = {120, 0, 128, 16};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			npc->act_wait = npc->code_event;
			// Fallthrough
		case 1:
			npc->ani_no = 0;

			if ( npc->act_wait )
			{
				--npc->act_wait;
				return;
			}

			npc->act_no = 10;
			npc->ani_wait = 0;
			break;

		case 10:
			if (++npc->ani_wait > 10)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
			{
				npc->ani_no = 0;
				npc->act_no = 1;
				npc->act_wait = npc->code_flag;
				SetNpChar(244, npc->x, npc->y, 0, 0, 0, 0, 0x100);
			}

			break;
	}

	if (npc->ani_wait / 2 % 2)
		npc->x = npc->tgt_x;
	else
		npc->x = npc->tgt_x + 0x200;

	npc->rect = rc[npc->ani_no];
}

//Press (proximity)
void ActNpc246(NPCHAR *npc)
{
	RECT rcLeft[3];

	rcLeft[0] = {144, 112, 160, 136};
	rcLeft[1] = {160, 112, 176, 136};
	rcLeft[2] = {176, 112, 192, 136};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x800;
			// Fallthrough
		case 1:
			if (gMC.x < npc->x + 0x1000 && gMC.x > npc->x - 0x1000 && gMC.y > npc->y + 0x1000 && gMC.y < npc->y + 0x10000)
				npc->act_no = 5;

			break;

		case 5:
			if ((npc->flag & 8) == 0)
			{
				npc->act_no = 10;
				npc->ani_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 10:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
				npc->ani_no = 2;

			if (gMC.y > npc->y)
			{
				npc->bits &= ~0x40;
				npc->damage = 0x7F;
			}
			else
			{
				npc->bits |= 0x40;
				npc->damage = 0;
			}

			if (npc->flag & 8)
			{
				if (npc->ani_no > 1)
				{
					for (int i = 0; i < 4; ++i)
						SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

					PlaySoundObject(26, 1);
					SetQuake(10);
				}

				npc->act_no = 20;
				npc->ani_no = 0;
				npc->ani_wait = 0;
				npc->bits |= 0x40;
				npc->damage = 0;
			}

			break;
	}

	if (npc->act_no >= 5)
	{
		npc->ym += 0x80;
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;

		npc->y += npc->ym;
	}

	npc->rect = rcLeft[npc->ani_no];
}

//EXP capsule
void ActNpc253(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	if (npc->life <= 100)
	{
		SetExpObjects(npc->x, npc->y, npc->code_flag);
		SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
		PlaySoundObject(25, 1);
		npc->cond = 0;
	}

	RECT rc[2];

	rc[0] = {0, 64, 16, 80};
	rc[1] = {16, 64, 32, 80};

	npc->rect = rc[npc->ani_no];
}

//Helicopter
void ActNpc254(NPCHAR *npc)
{
	RECT rc[2];

	rc[0] = {0, 0, 128, 64};
	rc[1] = {0, 64, 128, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			SetNpChar(255, npc->x + 0x2400, npc->y - 0x7200, 0, 0, 0, npc, 0x100);
			SetNpChar(255, npc->x - 0x4000, npc->y - 0x6800, 0, 0, 2, npc, 0x100);
			break;
		case 20:
			npc->act_wait = 0;
			npc->count1 = 60;
			npc->act_no = 21;
			break;
		case 30:
			npc->act_no = 21;
			SetNpChar(223, npc->x - 0x1600, npc->y - 0x1C00, 0, 0, 0, 0, 0x100);
			break;
		case 40:
			npc->act_no = 21;
			SetNpChar(223, npc->x - 0x1200, npc->y - 0x1C00, 0, 0, 0, 0, 0x100);
			SetNpChar(40, npc->x - 0x2C00, npc->y - 0x1C00, 0, 0, 0, 0, 0x100);
			SetNpChar(93, npc->x - 0x4600, npc->y - 0x1C00, 0, 0, 0, 0, 0x100);
			break;
	}

	if (npc->direct == 0)
		npc->rect = rc[0];
	else
		npc->rect = rc[1];
}

//Helicopter blades
void ActNpc255(NPCHAR *npc)
{
	RECT rcLeft[4];
	RECT rcRight[4];

	rcLeft[0] = {128, 0, 240, 16};
	rcLeft[1] = {128, 16, 240, 32};
	rcLeft[2] = {128, 32, 240, 48};
	rcLeft[3] = {128, 16, 240, 32};

	rcRight[0] = {240, 0, 320, 16};
	rcRight[1] = {240, 16, 320, 32};
	rcRight[2] = {240, 32, 320, 48};
	rcRight[3] = {240, 16, 320, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 0)
			{
				npc->view.front = 0x7000;
				npc->view.back = 0x7000;
			}
			else
			{
				npc->view.front = 0x5000;
				npc->view.back = 0x5000;
			}
			// Fallthrough
		case 1:
			if (npc->pNpc->act_no >= 20)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			// Fallthrough
		case 11:
			if (++npc->ani_no > 3)
				npc->ani_no = 0;

			break;
	}

	if (npc->direct == 0)
	{
		npc->x = npc->pNpc->x + 0x2400;
		npc->y = npc->pNpc->y - 0x7200;
	}
	else
	{
		npc->x = npc->pNpc->x - 0x4000;
		npc->y = npc->pNpc->y - 0x6800;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Curly (carried and unconcious)
void ActNpc259(NPCHAR *npc)
{
	RECT rcLeft = {224, 96, 240, 112};
	RECT rcRight = {224, 112, 240, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->bits &= ~0x2000;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (gMC.direct == 0)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->y = gMC.y - 0x800;

			if (npc->direct == 0)
			{
				npc->x = gMC.x + 0x600;
				npc->rect = rcLeft;
			}
			else
			{
				npc->x = gMC.x - 0x600;
				npc->rect = rcRight;
			}

			if (gMC.ani_no % 2)
				++npc->rect.top;

			break;

		case 10:
			npc->act_no = 11;
			npc->xm = 0x40;
			npc->ym = -0x20;

			npc->rect = rcLeft;
			break;

		case 11:
			if (npc->y < 0x8000)
				npc->ym = 0x20;

			npc->x += npc->xm;
			npc->y += npc->ym;
			break;

		case 20:
			VanishNpChar(npc);
			SetDestroyNpCharUp(npc->x, npc->y, 0x2000, 0x40);
			break;
	}
}
