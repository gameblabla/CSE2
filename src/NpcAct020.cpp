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

//Computer
void ActNpc020(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[3];

	rcLeft[0] = {288, 16, 320, 40};

	rcRight[0] = {288, 40, 320, 64};
	rcRight[1] = {288, 40, 320, 64};
	rcRight[2] = {288, 64, 320, 88};

	if (++npc->ani_wait > 3)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	if (npc->direct == 0)
		npc->rect = rcLeft[0];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Chest (open)
void ActNpc021(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
				npc->y += 0x2000;

			break;
	}

	RECT rect[0];

	rect[0]	= {224, 40, 240, 48};

	npc->rect = rect[0];
}

//Teleporter
void ActNpc022(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {240, 16, 264, 48};
	rect[1] = {248, 152, 272, 184};

	switch (npc->act_no)
	{
		case 0:
			npc->ani_no = 0;
			break;

		case 1:
			if (++npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	npc->rect = rect[npc->ani_no];
}

//Teleporter lights
void ActNpc023(NPCHAR *npc)
{
	RECT rect[8];

	rect[0] = {264, 16, 288, 20};
	rect[1] = {264, 20, 288, 24};
	rect[2] = {264, 24, 288, 28};
	rect[3] = {264, 28, 288, 32};
	rect[4] = {264, 32, 288, 36};
	rect[5] = {264, 36, 288, 40};
	rect[6] = {264, 40, 288, 44};
	rect[7] = {264, 44, 288, 48};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 7)
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
}

//Power Critter
void ActNpc024(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {0, 0, 24, 24};
	rcLeft[1] = {24, 0, 48, 24};
	rcLeft[2] = {48, 0, 72, 24};
	rcLeft[3] = {72, 0, 96, 24};
	rcLeft[4] = {96, 0, 120, 24};
	rcLeft[5] = {120, 0, 144, 24};

	rcRight[0] = {0, 24, 24, 48};
	rcRight[1] = {24, 24, 48, 48};
	rcRight[2] = {48, 24, 72, 48};
	rcRight[3] = {72, 24, 96, 48};
	rcRight[4] = {96, 24, 120, 48};
	rcRight[5] = {120, 24, 144, 48};

	switch (npc->act_no)
	{
		case 0:
			npc->y += 0x600;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (npc->act_wait >= 8 && gMC.x > npc->x - 0x10000 && gMC.x < npc->x + 0x10000 && gMC.y > npc->y - 0x10000 && gMC.y < npc->y + 0x6000)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;

				npc->ani_no = 1;
			}
			else
			{
				if (npc->act_wait < 8)
					++npc->act_wait;

				npc->ani_no = 0;
			}

			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			if (npc->act_wait >= 8 && gMC.x > npc->x - 0xC000 && gMC.x < npc->x + 0xC000 && gMC.y > npc->y - 0xC000 && gMC.y < npc->y + 0x6000)
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
				PlaySoundObject(108, 1);

				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;

				if (npc->direct == 0)
					npc->xm = -0x100;
				else
					npc->xm = 0x100;
			}

			break;

		case 3:
			if (npc->ym > 0x200)
			{
				npc->tgt_y = npc->y;
				npc->act_no = 4;
				npc->ani_no = 3;
				npc->act_wait = 0;
				npc->act_wait = 0;	// lol duplicate line
			}

			break;

		case 4:
			if (gMC.x > npc->x)
				npc->direct = 2;
			else
				npc->direct = 0;

			++npc->act_wait;

			if (npc->flag & 7 || npc->act_wait > 100)
			{
				npc->damage = 12;
				npc->act_no = 5;
				npc->ani_no = 2;
				npc->xm /= 2;
			}
			else
			{
				if (npc->act_wait % 4 == 1)
					PlaySoundObject(110, 1);

				if (++npc->ani_wait > 0)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 5)
					npc->ani_no = 3;
			}

			break;

		case 5:
			if (npc->flag & 8)
			{
				npc->damage = 2;
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;
	}

	if (npc->act_no == 4)
	{
		if (gMC.x > npc->x)
			npc->xm += 0x20;
		else
			npc->xm -= 0x20;

		if (npc->tgt_y < npc->y)
			npc->ym -= 0x10;
		else
			npc->ym += 0x10;

		if (npc->ym > 0x200)
			npc->ym = 0x200;
		if (npc->ym < -0x200)
			npc->ym = -0x200;

		if (npc->xm > 0x200)
			npc->xm = 0x200;
		if (npc->xm < -0x200)
			npc->xm = -0x200;
	}
	else
	{
		npc->ym += 0x20;
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Egg Corridor lift
void ActNpc025(NPCHAR *npc)
{
	RECT rcLeft[2];

	rcLeft[0] = {256, 64, 288, 80};
	rcLeft[1] = {256, 80, 288, 96};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->x += 0x1000;
			// Fallthrough
		case 1:
			if (++npc->act_wait > 150)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}

			break;

		case 2:
			if (++npc->act_wait > 0x40)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}
			else
			{
				npc->y -= 0x200;
			}

			break;

		case 3:
			if (++npc->act_wait > 150)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}

			break;

		case 4:
			if (++npc->act_wait > 0x40)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}
			else
			{
				npc->y -= 0x200;
			}

			break;

		case 5:
			if (++npc->act_wait > 150)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}

			break;

		case 6:
			if (++npc->act_wait > 0x40)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}
			else
			{
				npc->y += 0x200;
			}

			break;

		case 7:
			if (++npc->act_wait > 150)
			{
				npc->act_wait = 0;
				++npc->act_no;
			}

			break;

		case 8:
			if (++npc->act_wait > 0x40)
			{
				npc->act_wait = 0;
				npc->act_no = 1;
			}
			else
			{
				npc->y += 0x200;
			}

			break;
	}

	switch ( npc->act_no )
	{
		case 2:
		case 4:
		case 6:
		case 8:
			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	npc->rect = rcLeft[npc->ani_no];
}

//Flying Critter (Grasstown)
void ActNpc028(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {0, 48, 16, 64};
	rcLeft[1] = {16, 48, 32, 64};
	rcLeft[2] = {32, 48, 48, 64};
	rcLeft[3] = {48, 48, 64, 64};
	rcLeft[4] = {64, 48, 80, 64};
	rcLeft[5] = {80, 48, 96, 64};

	rcRight[0] = {0, 64, 16, 80};
	rcRight[1] = {16, 64, 32, 80};
	rcRight[2] = {32, 64, 48, 80};
	rcRight[3] = {48, 64, 64, 80};
	rcRight[4] = {64, 64, 80, 80};
	rcRight[5] = {80, 64, 96, 80};

	switch (npc->act_no)
	{
		case 0:
			npc->y += 0x600;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (npc->act_wait >= 8 && gMC.x > npc->x - 0x10000 && gMC.x < npc->x + 0x10000 && gMC.y > npc->y - 0x10000 && gMC.y < npc->y + 0x6000)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;

				npc->ani_no = 1;
			}
			else
			{
				if (npc->act_wait < 8)
					++npc->act_wait;

				npc->ani_no = 0;
			}

			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			if ( npc->act_wait >= 8 && gMC.x > npc->x - 0xC000 && gMC.x < npc->x + 0xC000 && gMC.y > npc->y - 0xC000 && gMC.y < npc->y + 0x6000)
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
				npc->ym = -1228;
				PlaySoundObject(30, 1);

				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;

				if (npc->direct == 0)
					npc->xm = -0x100;
				else
					npc->xm = 0x100;
			}

			break;

		case 3:
			if (npc->ym > 0x100)
			{
				npc->tgt_y = npc->y;
				npc->act_no = 4;
				npc->ani_no = 3;
				npc->act_wait = 0;
				npc->act_wait = 0;	// lol duplicate line
			}

			break;

		case 4:
			if (gMC.x > npc->x)
				npc->direct = 2;
			else
				npc->direct = 0;

			++npc->act_wait;

			if (npc->flag & 7 || npc->act_wait > 100)
			{
				npc->damage = 3;
				npc->act_no = 5;
				npc->ani_no = 2;
				npc->xm /= 2;
			}
			else
			{
				if (npc->act_wait % 4 == 1)
					PlaySoundObject(109, 1);

				if (npc->flag & 8)
					npc->ym = -0x200;

				if (++npc->ani_wait > 0)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 5)
					npc->ani_no = 3;
			}

			break;

		case 5:
			if (npc->flag & 8)
			{
				npc->damage = 2;
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;
				PlaySoundObject(23, 1);
			}

			break;
	}

	if (npc->act_no == 4)
	{
		if (gMC.x > npc->x)
			npc->xm += 0x20;
		else
			npc->xm -= 0x20;

		if (npc->tgt_y < npc->y)
			npc->ym -= 0x10;
		else
			npc->ym += 0x10;

		if (npc->ym > 0x200)
			npc->ym = 0x200;
		if (npc->ym < -0x200)
			npc->ym = -0x200;

		if (npc->xm > 0x200)
			npc->xm = 0x200;
		if (npc->xm < -0x200)
			npc->xm = -0x200;
	}
	else
	{
		npc->ym += 0x40;
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Cthulhu
void ActNpc029(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {0, 192, 16, 216};
	rcLeft[1] = {16, 192, 32, 216};

	rcRight[0] = {0, 216, 16, 240};
	rcRight[1] = {16, 216, 32, 240};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (npc->x - 0x6000 < gMC.x && npc->x + 0x6000 > gMC.x && npc->y - 0x6000 < gMC.y && npc->y + 0x2000 > gMC.y)
				npc->ani_no = 1;
			else
				npc->ani_no = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Gunsmith
void ActNpc030(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {48, 0, 64, 16};
	rc[1] = {48, 16, 64, 32};
	rc[2] = {0, 32, 16, 48};

	if (npc->direct != 0)
	{
		switch (npc->act_no)
		{
			case 0:
				npc->act_no = 1;
				npc->y += 0x2000;
				npc->ani_no = 2;
				break;
		}

		if (++npc->act_wait > 100)
		{
			npc->act_wait = 0;
			SetCaret(npc->x, npc->y - 0x400, 5, 0);
		}
	}
	else
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
	}

	npc->rect = rc[npc->ani_no];
}

//Life capsule
void ActNpc032(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {32, 96, 48, 112};
	rect[1] = {48, 96, 64, 112};

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if ( npc->ani_no > 1 )
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
}

// Bed
void ActNpc034(NPCHAR *npc)
{
	RECT rcLeft[1];
	RECT rcRight[1];

	rcLeft[0] = {192, 48, 224, 64};
	rcRight[0] = {192, 184, 224, 200};

	if (npc->direct == 0)
		npc->rect = rcLeft[0];
	else
		npc->rect = rcRight[0];
}

//Signpost
void ActNpc037(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {192, 64, 208, 80};
	rect[1] = {208, 64, 224, 80};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 1)
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
}

//Fireplace
void ActNpc038(NPCHAR *npc)
{
	RECT rect[4];

	rect[0] = {128, 64, 144, 80};
	rect[1] = {144, 64, 160, 80};
	rect[2] = {160, 64, 176, 80};
	rect[3] = {176, 64, 192, 80};

	switch (npc->act_no)
	{
		case 0:
			if (++npc->ani_wait > 3)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 0;

			npc->rect = rect[npc->ani_no];
			break;

		case 10:
			npc->act_no = 11;
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
			// Fallthrough
		case 11:
			npc->rect.left = 0;
			npc->rect.right = 0;
			break;
	}
}

//Save sign
void ActNpc039(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {224, 64, 240, 80};
	rect[1] = {240, 64, 256, 80};

	if (npc->direct == 0)
		npc->ani_no = 0;
	else
		npc->ani_no = 1;

	npc->rect = rect[npc->ani_no];
}
