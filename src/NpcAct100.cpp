#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

// Grate
void ActNpc100(NPCHAR *npc)
{
	RECT rc[2];

	rc[0] = {272, 48, 288, 64};
	rc[1] = {272, 48, 288, 64};

	if (npc->act_no == 0)
	{
		npc->y += 0x2000;
		npc->act_no = 1;
	}

	if (npc->direct == 0)
		npc->rect = rc[0];
	else
		npc->rect = rc[1];
}

//Mannan projectile
void ActNpc103(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {192, 96, 208, 120};
	rcLeft[1] = {208, 96, 224, 120};
	rcLeft[2] = {224, 96, 240, 120};

	rcRight[0] = {192, 120, 208, 144};
	rcRight[1] = {208, 120, 224, 144};
	rcRight[2] = {224, 120, 240, 144};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (npc->direct == 0)
				npc->xm -= 0x20;
			else
				npc->xm += 0x20;

			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
				npc->ani_no = 0;

			break;
	}

	npc->x += npc->xm;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (++npc->count1 > 100)
		npc->cond = 0;

	if (npc->count1 % 4 == 1)
		PlaySoundObject(46, 1);
}

//Frog
void ActNpc104(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {0, 112, 32, 144};
	rcLeft[1] = {32, 112, 64, 144};
	rcLeft[2] = {64, 112, 96, 144};

	rcRight[0] = {0, 144, 32, 176};
	rcRight[1] = {32, 144, 64, 176};
	rcRight[2] = {64, 144, 96, 176};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = 0;
			npc->xm = 0;
			npc->ym = 0;

			if (npc->direct == 4)
			{
				if (Random(0, 1) != 0)
					npc->direct = 0;
				else
					npc->direct = 2;

				npc->bits |= 8;
				npc->ani_no = 2;
				npc->act_no = 3;
				break;
			}
			else
			{
				npc->bits &= ~8;
			}
			// Fallthrough
		case 1:
			++npc->act_wait;

			if (Random(0, 50) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->ani_wait = 0;
			}

			break;

		case 2:
			++npc->act_wait;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->act_wait > 18)
			{
				npc->act_no = 1;
				npc->act_no = 1;
			}

			break;

		case 3:
			if (++npc->act_wait > 40)
				npc->bits &= ~8;

			if (npc->flag & 8)
			{
				npc->act_no = 0;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			break;

		case 10:
			npc->act_no = 11;
			// Fallthrough
		case 11:
			if (npc->flag & 1 && npc->xm < 0)
			{
				npc->xm = -npc->xm;
				npc->direct = 2;
			}

			if (npc->flag & 4 && npc->xm > 0)
			{
				npc->xm = -npc->xm;
				npc->direct = 0;
			}

			if (npc->flag & 8)
			{
				npc->act_no = 0;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			break;
	}

	bool bJump = false;

	if (npc->act_no < 10 && npc->act_no != 3 && npc->act_wait > 10)
	{
		if (npc->shock)
			bJump = true;

		if (npc->x >= gMC.x - 0x14000 && npc->x <= gMC.x + 0x14000 && npc->y >= gMC.y - 0x8000 && npc->y <= gMC.y + 0x8000)
		{
			if (Random(0, 50) == 2)
				bJump = true;
		}
	}

	if (bJump)
	{
		if (gMC.x > npc->x)
			npc->direct = 0;
		else
			npc->direct = 2;

		npc->act_no = 10;
		npc->ani_no = 2;
		npc->ym = -0x5FF;

		if (!(gMC.cond & 2))
			PlaySoundObject(30, 1);

		if (npc->direct == 0)
			npc->xm = -0x200u;
		else
			npc->xm = 0x200;
	}

	npc->ym += 0x80;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//'HEY!' speech bubble (low)
void ActNpc105(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {128, 32, 144, 48};
	rect[1] = {128, 32, 128, 32};

	if (++npc->act_wait > 30)
		npc->cond = 0;

	if (npc->act_wait <= 4)
		npc->y -= 0x200;

	npc->rect = rect[npc->ani_no];
}

//'HEY!' speech bubble (high)
void ActNpc106(NPCHAR *npc)
{
	if (npc->act_no == 0)
	{
		SetNpChar(105, npc->x, npc->y - 0x1000, 0, 0, 0, 0, 0x180);
		npc->act_no = 1;
	}
}

//Quote (teleport out)
void ActNpc111(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};

	switch ( npc->act_no )
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->y -= 0x2000;
			break;

		case 1:
			if (++npc->act_wait > 20)
			{
				npc->act_wait = 0;
				npc->act_no = 2;
				npc->ani_no = 1;
				npc->ym = -0x2FF;
			}

			break;

		case 2:
			if (npc->ym > 0)
				npc->hit.bottom = 0x2000;

			if (npc->flag & 8)
			{
				npc->act_no = 3;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;

		case 3:
			if (++npc->act_wait > 40)
			{
				npc->act_no = 4;
				npc->act_wait = 64;
				PlaySoundObject(29, 1);
			}

			break;

		case 4:
			--npc->act_wait;
			npc->ani_no = 0;

			if (npc->act_wait == 0)
				npc->cond = 0;

			break;
	}

	npc->ym += 0x40;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (gMC.equip & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}

	if (npc->act_no == 4)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 4;

		if (npc->act_wait / 2 % 2)
			++npc->rect.left;
	}
}

//Quote (teleport in)
void ActNpc112(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->x += 0x2000;
			npc->y += 0x1000;
			PlaySoundObject(29, 1);
			// Fallthrough
		case 1:
			if (++npc->act_wait == 64)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
			}

			break;

		case 2:
			if (++npc->act_wait > 20)
			{
				npc->act_no = 3;
				npc->ani_no = 1;
				npc->hit.bottom = 0x1000;
			}

			break;

		case 3:
			if (npc->flag & 8)
			{
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;
	}

	npc->ym += 0x40;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];

	if (gMC.equip & 0x40)
	{
		npc->rect.top += 32;
		npc->rect.bottom += 32;
	}

	if (npc->act_no == 1)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 4;

		if ( npc->act_wait / 2 % 2 )
			++npc->rect.left;
	}
}

//Press
void ActNpc114(NPCHAR *npc)
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
			if (!(npc->flag & 8))
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

				npc->act_no = 1;
				npc->ani_no = 0;
				npc->damage = 0;
				npc->bits |= 0x40;
			}
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	npc->rect = rcLeft[npc->ani_no];
}

//Red petals
void ActNpc116(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {272, 184, 320, 200};

	npc->rect = rc[0];
}

//Table and chair
void ActNpc119(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {248, 184, 272, 200};

	npc->rect = rc[0];
}
