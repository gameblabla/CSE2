#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Frame.h"
#include "Caret.h"
#include "Bullet.h"

//Grate
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

//Malco computer screen
void ActNpc101(NPCHAR *npc)
{
	RECT rect[3];

	rect[0] = {240, 136, 256, 152};
	rect[1] = {240, 136, 256, 152};
	rect[2] = {256, 136, 272, 152};

	if (++npc->ani_wait > 3)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
}

//Malco computer wave
void ActNpc102(NPCHAR *npc)
{
	RECT rect[4];

	rect[0] = {208, 120, 224, 136};
	rect[1] = {224, 120, 240, 136};
	rect[2] = {240, 120, 256, 136};
	rect[3] = {256, 120, 272, 136};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;
		npc->y += 0x1000;
	}

	if (++npc->ani_wait > 0)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 3)
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
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
			npc->direct = 2;
		else
			npc->direct = 0;

		npc->act_no = 10;
		npc->ani_no = 2;
		npc->ym = -0x5FF;

		if (!(gMC.cond & 2))
			PlaySoundObject(30, 1);

		if (npc->direct == 0)
			npc->xm = -0x200;
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

//Malco
void ActNpc107(NPCHAR *npc)
{
	switch ( npc->act_no )
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
				npc->ani_no = 5;

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->ani_wait = 0;

			for (int i = 0; i < 4; ++i)
				SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			// Fallthrough
		case 11:
			if (++npc->ani_wait > 1)
			{
				PlaySoundObject(43, 1);
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (++npc->act_wait > 100)
				npc->act_no = 12;

			break;

		case 12:
			npc->act_no = 13;
			npc->act_wait = 0;
			npc->ani_no = 1;
			// Fallthrough
		case 13:
			if (++npc->act_wait > 50)
				npc->act_no = 14;

			break;

		case 14:
			npc->act_no = 15;
			npc->act_wait = 0;
			// Fallthrough
		case 15:
			if (npc->act_wait / 2 % 2)
			{
				npc->x += 0x200;
				PlaySoundObject(11, 1);
			}
			else
			{
				npc->x -= 0x200;
			}

			if (++npc->act_wait > 50)
				npc->act_no = 16;

			break;

		case 16:
			npc->act_no = 17;
			npc->act_wait = 0;
			npc->ani_no = 2;
			PlaySoundObject(12, 1);

			for (int i = 0; i < 8; ++i)
				SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			// Fallthrough
		case 17:
			if (++npc->act_wait > 150)
				npc->act_no = 18;

			break;

		case 18:
			npc->act_no = 19;
			npc->act_wait = 0;
			npc->ani_no = 3;
			npc->ani_wait = 0;
			// Fallthrough
		case 19:
			if (++npc->ani_wait > 3)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
			{
				PlaySoundObject(11, 1);
				npc->ani_no = 3;
			}

			if (++npc->act_wait > 100)
			{
				npc->act_no = 20;
				PlaySoundObject(12, 1);

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			break;

		case 20:
			npc->ani_no = 4;
			break;

		case 21:
			npc->act_no = 22;
			npc->ani_no = 5;
			PlaySoundObject(51, 1);
			break;

		case 100:
			npc->act_no = 101;
			npc->ani_no = 6;
			npc->ani_wait = 0;
			// Fallthrough
		case 101:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 9)
				npc->ani_no = 6;

			break;

		case 110:
			SetDestroyNpChar(npc->x, npc->y, 0x2000, 16);
			npc->cond = 0;
			break;
	}

	RECT rcPoweron[10];

	rcPoweron[0] = {144, 0, 160, 24};
	rcPoweron[1] = {160, 0, 176, 24};
	rcPoweron[2] = {176, 0, 192, 24};
	rcPoweron[3] = {192, 0, 208, 24};
	rcPoweron[4] = {208, 0, 224, 24};
	rcPoweron[5] = {224, 0, 240, 24};
	rcPoweron[6] = {176, 0, 192, 24};
	rcPoweron[7] = {192, 0, 208, 24};
	rcPoweron[8] = {208, 0, 224, 24};
	rcPoweron[9] = {192, 0, 208, 24};

	npc->rect = rcPoweron[npc->ani_no];
}

//Balfrog projectile
void ActNpc108(NPCHAR *npc)
{
	if (npc->flag & 0xFF)
	{
		SetCaret(npc->x, npc->y, 2, 0);
		npc->cond = 0;
	}

	npc->y += npc->ym;
	npc->x += npc->xm;

	RECT rect_left[3];

	rect_left[0] = {96, 48, 112, 64};
	rect_left[1] = {112, 48, 128, 64};
	rect_left[2] = {128, 48, 144, 64};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	npc->rect = rect_left[npc->ani_no];

	if (++npc->count1 > 300)
	{
		SetCaret(npc->x, npc->y, 2, 0);
		npc->cond = 0;
	}
}

//Malco (broken)
void ActNpc109(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {240, 0, 256, 24};
	rcLeft[1] = {256, 0, 272, 24};

	rcRight[0] = {240, 24, 256, 48};
	rcRight[1] = {256, 24, 272, 48};

	switch (npc->act_no)
	{
		case 0:
			if (--npc->act_wait)
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
			npc->act_no = 0;
			PlaySoundObject(12, 1);

			for (int i = 0; i < 8; ++i)
				SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

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

//Puchi
void ActNpc110(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {96, 128, 112, 144};
	rcLeft[1] = {112, 128, 128, 144};
	rcLeft[2] = {128, 128, 144, 144};

	rcRight[0] = {96, 144, 112, 160};
	rcRight[1] = {112, 144, 128, 160};
	rcRight[2] = {128, 144, 144, 160};

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
			npc->direct = 2;
		else
			npc->direct = 0;

		npc->act_no = 10;
		npc->ani_no = 2;
		npc->ym = -0x2FF;
		PlaySoundObject(6, 1);

		if (npc->direct == 0)
			npc->xm = -0x100u;
		else
			npc->xm = 0x100;
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

//Professor Booster
void ActNpc113(NPCHAR *npc)
{
	RECT rcLeft[7];
	RECT rcRight[7];

	rcLeft[0] = {224, 0, 240, 16};
	rcLeft[1] = {240, 0, 256, 16};
	rcLeft[2] = {256, 0, 272, 16};
	rcLeft[3] = {224, 0, 240, 16};
	rcLeft[4] = {272, 0, 288, 16};
	rcLeft[5] = {224, 0, 240, 16};
	rcLeft[6] = {288, 0, 304, 16};

	rcRight[0] = {224, 16, 240, 32};
	rcRight[1] = {240, 16, 256, 32};
	rcRight[2] = {256, 16, 272, 32};
	rcRight[3] = {224, 16, 240, 32};
	rcRight[4] = {272, 16, 288, 32};
	rcRight[5] = {224, 16, 240, 32};
	rcRight[6] = {288, 16, 304, 32};

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

		case 3:
			npc->act_no = 4;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			// Fallthrough
		case 4:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			if (npc->direct == 0)
				npc->x -= 0x200;
			else
				npc->x += 0x200;

			break;

		case 5:
			npc->ani_no = 6;
			break;

		case 30:
			npc->act_no = 31;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->hit.bottom = 0x2000;
			npc->x -= 0x2000;
			npc->y += 0x1000;
			PlaySoundObject(29, 1);
			// Fallthrough
		case 31:
			if (++npc->act_wait == 64)
			{
				npc->act_no = 32;
				npc->act_wait = 0;
			}

			break;

		case 32:
			if (++npc->act_wait > 20)
			{
				npc->act_no = 33;
				npc->ani_no = 1;
				npc->hit.bottom = 0x1000;
			}

			break;

		case 33:
			if (npc->flag & 8)
			{
				npc->act_no = 34;
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

	if (npc->act_no == 31)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 4;

		if (npc->act_wait / 2 % 2)
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

//Ravil
void ActNpc115(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {0, 120, 24, 144};
	rcLeft[1] = {24, 120, 48, 144};
	rcLeft[2] = {48, 120, 72, 144};
	rcLeft[3] = {72, 120, 96, 144};
	rcLeft[4] = {96, 120, 120, 144};
	rcLeft[5] = {120, 120, 144, 144};

	rcRight[0] = {0, 144, 24, 168};
	rcRight[1] = {24, 144, 48, 168};
	rcRight[2] = {48, 144, 72, 168};
	rcRight[3] = {72, 144, 96, 168};
	rcRight[4] = {96, 144, 120, 168};
	rcRight[5] = {120, 144, 144, 168};

	switch (npc->act_no)
	{
		case 0:
			npc->xm = 0;
			npc->act_no = 1;
			npc->act_wait = 0;
			npc->count1 = 0;
			// Fallthrough
		case 1:
			if (gMC.x < npc->x + 0xC000 && gMC.x > npc->x - 0xC000 && gMC.y < npc->y + 0x4000 && gMC.y > npc->y - 0xC000)
				npc->act_no = 10;

			if (npc->shock)
				npc->act_no = 10;

			break;

		case 10:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->ani_no = 1;

			if (++npc->act_wait > 20)
			{
				npc->act_wait = 0;
				npc->act_no = 20;
			}

			break;

		case 20:
			npc->damage = 0;
			npc->xm = 0;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;

				if (npc->direct == 0)
					npc->xm = -0x200u;
				else
					npc->xm = 0x200;

				if (++npc->count1 > 2)
				{
					npc->count1 = 0;
					npc->ani_no = 4;
					npc->act_no = 21;
					npc->ym = -0x400u;
					npc->xm *= 2;
					npc->damage = 5;
					PlaySoundObject(102, 1);
				}
				else
				{
					npc->act_no = 21;
					npc->ym = -0x400u;
					PlaySoundObject(30, 1);
				}
			}

			break;

		case 21:
			if (npc->flag & 8)
			{
				PlaySoundObject(23, 1);
				npc->act_no = 20;
				npc->ani_no = 1;
				npc->ani_wait = 0;
				npc->damage = 0;

				if (gMC.x > npc->x + 0x12000 || gMC.x < npc->x - 0x12000 || gMC.y > npc->y + 0x6000 || gMC.y < npc->y - 0x12000)
					npc->act_no = 0;
			}

			break;

		case 30:
			for (int i = 0; i < 8; ++i)
				SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			npc->ani_no = 0;
			npc->act_no = 0;

			break;

		case 50:
			npc->act_no = 51;
			npc->ani_no = 4;
			npc->damage = 0;
			npc->ym = -0x200;
			npc->bits &= ~0x21;
			PlaySoundObject(51, 1);
			// Fallthrough
		case 51:
			if (npc->flag & 8)
			{
				PlaySoundObject(23, 1);
				npc->act_no = 52;
				npc->ani_no = 5;
				npc->xm = 0;
			}

			break;
	}

	if (npc->act_no > 50)
		npc->ym += 0x20;
	else
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

//Red petals
void ActNpc116(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {272, 184, 320, 200};

	npc->rect = rc[0];
}

//Curly
void ActNpc117(NPCHAR *npc)
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = {0, 96, 16, 112};
	rcLeft[1] = {16, 96, 32, 112};
	rcLeft[2] = {0, 96, 16, 112};
	rcLeft[3] = {32, 96, 48, 112};
	rcLeft[4] = {0, 96, 16, 112};
	rcLeft[5] = {176, 96, 192, 112};
	rcLeft[6] = {112, 96, 128, 112};
	rcLeft[7] = {160, 96, 176, 112};
	rcLeft[8] = {144, 96, 160, 112};
	rcLeft[9] = {48, 96, 64, 112};

	rcRight[0] = {0, 112, 16, 128};
	rcRight[1] = {16, 112, 32, 128};
	rcRight[2] = {0, 112, 16, 128};
	rcRight[3] = {32, 112, 48, 128};
	rcRight[4] = {0, 112, 16, 128};
	rcRight[5] = {176, 112, 192, 128};
	rcRight[6] = {112, 112, 128, 128};
	rcRight[7] = {160, 112, 176, 128};
	rcRight[8] = {144, 112, 160, 128};
	rcRight[9] = {48, 112, 64, 128};

	switch (npc->act_no)
	{
		case 0:
			if (npc->direct == 4)
			{
				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			npc->xm = 0;
			npc->ym += 0x40;
			break;

		case 3:
			npc->act_no = 4;
			npc->ani_no = 1;
			npc->ani_wait = 0;
			// Fallthrough
		case 4:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
				npc->ani_no = 1;

			npc->ym += 0x40;

			if (npc->direct == 0)
				npc->xm = -0x200;
			else
				npc->xm = 0x200;

			break;

		case 5:
			npc->act_no = 6;
			npc->ani_no = 5;
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
			break;

		case 6:
			npc->ani_no = 5;
			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 1;
			npc->ani_wait = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			// Fallthrough
		case 11:
			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
				npc->ani_no = 1;

			if (npc->direct == 0)
				npc->x -= 0x200;
			else
				npc->x += 0x200;

			if (gMC.x < npc->x + 0x2800 && gMC.x > npc->x - 0x2800)
				npc->act_no = 0;

			break;

		case 20:
			npc->xm = 0;
			npc->ani_no = 6;
			break;

		case 21:
			npc->xm = 0;
			npc->ani_no = 9;
			break;

		case 30:
			npc->act_no = 31;
			npc->act_wait = 0;
			npc->ym = -0x400u;
			// Fallthrough
		case 31:
			npc->ani_no = 7;

			if (npc->direct == 0)
				npc->xm = 0x200;
			else
				npc->xm = -0x200u;

			npc->ym += 0x40;

			if (npc->act_wait++ && npc->flag & 8)
				npc->act_no = 32;

			break;

		case 32:
			npc->ym += 0x40;
			npc->ani_no = 8;
			npc->xm = 0;
			break;

		case 70:
			npc->act_no = 71;
			npc->act_wait = 0;
			npc->ani_no = 1;
			npc->ani_wait = 0;
			// Fallthrough
		case 71:
			if (npc->direct == 0)
				npc->x += 0x100;
			else
				npc->x -= 0x100;

			if (++npc->ani_wait > 8)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 4)
				npc->ani_no = 1;

			break;
	}

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Curly (boss)
void ActNpc118(NPCHAR *npc)
{
	RECT rcLeft[9];
	RECT rcRight[9];

	rcLeft[0] = {0, 32, 32, 56};
	rcLeft[1] = {32, 32, 64, 56};
	rcLeft[2] = {64, 32, 96, 56};
	rcLeft[3] = {96, 32, 128, 56};
	rcLeft[4] = {0, 32, 32, 56};
	rcLeft[5] = {128, 32, 160, 56};
	rcLeft[6] = {0, 32, 32, 56};
	rcLeft[7] = {0, 32, 32, 56};
	rcLeft[8] = {160, 32, 192, 56};

	rcRight[0] = {0, 56, 32, 80};
	rcRight[1] = {32, 56, 64, 80};
	rcRight[2] = {64, 56, 96, 80};
	rcRight[3] = {96, 56, 128, 80};
	rcRight[4] = {0, 56, 32, 80};
	rcRight[5] = {128, 56, 160, 80};
	rcRight[6] = {0, 56, 32, 80};
	rcRight[7] = {0, 56, 32, 80};
	rcRight[8] = {160, 56, 192, 80};

	bool bUpper = false;

	if (npc->direct == 0 && gMC.x > npc->x)
		bUpper = true;
	if ( npc->direct == 2 && gMC.x < npc->x)
		bUpper = true;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = Random(50, 100);
			npc->ani_no = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->bits |= 0x20u;
			npc->bits &= ~4u;
			// Fallthrough
		case 11:
			if (npc->act_wait)
				--npc->act_wait;
			else
				npc->act_no = 13;

			break;

		case 13:
			npc->act_no = 14;
			npc->ani_no = 3;
			npc->act_wait = Random(50, 100);

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

			if (npc->ani_no > 6)
				npc->ani_no = 3;

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
				npc->bits |= 0x20u;
				npc->act_no = 20;
				npc->act_wait = 0;
				PlaySoundObject(103, 1);
			}

			break;

		case 20:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			npc->xm = 8 * npc->xm / 9;

			if (++npc->ani_no > 1)
				npc->ani_no = 0;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 21;
				npc->act_wait = 0;
			}

			break;

		case 21:
			if (++npc->act_wait % 4 == 1)
			{
				if (npc->direct == 0)
				{
					if (bUpper)
					{
						npc->ani_no = 2;
						SetNpChar(123, npc->x, npc->y - 0x1000, 0, 0, 1, 0, 0x100);
					}
					else
					{
						npc->ani_no = 0;
						SetNpChar(123, npc->x - 0x1000, npc->y + 0x800, 0, 0, 0, 0, 0x100);
						npc->x += 0x200;
					}
				}
				else
				{
					if (bUpper)
					{
						npc->ani_no = 2;
						SetNpChar(123, npc->x, npc->y - 0x1000, 0, 0, 1, 0, 0x100);
					}
					else
					{
						npc->ani_no = 0;
						SetNpChar(123, npc->x + 0x1000, npc->y + 0x800, 0, 0, 2, 0, 0x100);
						npc->x -= 0x200;
					}
				}
			}

			if (npc->act_wait > 30)
				npc->act_no = 10;

			break;

		case 30:
			if (++npc->ani_no > 8)
				npc->ani_no = 7;

			if (++npc->act_wait > 30)
			{
				npc->act_no = 10;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->act_no > 10 && npc->act_no < 30 && CountArmsBullet(6))
	{
		npc->act_wait = 0;
		npc->act_no = 30;
		npc->ani_no = 7;
		npc->bits &= ~0x20;
		npc->bits |= 4;
		npc->xm = 0;
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

//Table and chair
void ActNpc119(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {248, 184, 272, 200};

	npc->rect = rc[0];
}
