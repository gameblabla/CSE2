#include <string>

#include "WindowsWrapper.h"

#include "Caret.h"
#include "Draw.h"
#include "Triangle.h"
#include "Game.h"

#define CARET_MAX 0x40
CARET gCrt[CARET_MAX];

void InitCaret()
{
	memset(gCrt, 0, sizeof(gCrt));
}

void ActCaret00(CARET *crt)
{
	(void)crt;
}

void ActCaret01(CARET *crt)
{
	RECT rcLeft[4];
	RECT rcRight[4];
	
	rcLeft[0] = {0, 64, 8, 72};
	rcLeft[1] = {8, 64, 16, 72};
	rcLeft[2] = {16, 64, 24, 72};
	rcLeft[3] = {24, 64, 32, 72};
	
	rcRight[0] = {64, 24, 72, 32};
	rcRight[1] = {72, 24, 80, 32};
	rcRight[2] = {80, 24, 88, 32};
	rcRight[3] = {88, 24, 92, 32};
	
	if (!crt->act_no)
	{
		crt->act_no = 1;
		crt->xm = Random(-0x400, 0x400);
		crt->ym = Random(-0x400, 0);
	}
	
	crt->ym += 0x40;
	crt->x += crt->xm;
	crt->y += crt->ym;
	
	if (++crt->ani_wait > 5)
	{
		crt->ani_wait = 0;
		if (++crt->ani_no > 3)
			crt->cond = 0;
	}
	
	if (crt->direct)
		crt->rect = rcRight[crt->ani_no];
	else
		crt->rect = rcLeft[crt->ani_no];
}

void ActCaret02(CARET *crt)
{
	RECT rect_right[4];
	RECT rect_left[4];
	RECT rect_up[3];
	rect_left[0].left = 0;
	rect_left[0].top = 32;
	rect_left[0].right = 16;
	rect_left[0].bottom = 48;
	rect_left[1].left = 16;
	rect_left[1].top = 32;
	rect_left[1].right = 32;
	rect_left[1].bottom = 48;
	rect_left[2].left = 32;
	rect_left[2].top = 32;
	rect_left[2].right = 48;
	rect_left[2].bottom = 48;
	rect_left[3].left = 48;
	rect_left[3].top = 32;
	rect_left[3].right = 64;
	rect_left[3].bottom = 48;
	rect_right[0].left = 176;
	rect_right[0].top = 0;
	rect_right[0].right = 192;
	rect_right[0].bottom = 16;
	rect_right[1].left = 192;
	rect_right[1].top = 0;
	rect_right[1].right = 208;
	rect_right[1].bottom = 16;
	rect_right[2].left = 208;
	rect_right[2].top = 0;
	rect_right[2].right = 224;
	rect_right[2].bottom = 16;
	rect_right[3].left = 224;
	rect_right[3].top = 0;
	rect_right[3].right = 240;
	rect_right[3].bottom = 16;
	rect_up[0].left = 0;
	rect_up[0].top = 32;
	rect_up[0].right = 16;
	rect_up[0].bottom = 48;
	rect_up[1].left = 32;
	rect_up[1].top = 32;
	rect_up[1].right = 48;
	rect_up[1].bottom = 48;
	rect_up[2].left = 16;
	rect_up[2].top = 32;
	rect_up[2].right = 32;
	rect_up[2].bottom = 48;

	switch (crt->direct)
	{
		case 0:
			crt->ym -= 0x10;
			crt->y += crt->ym;
			if (++crt->ani_wait > 5)
			{
				crt->ani_wait = 0;
				++crt->ani_no;
			}
			if ( crt->ani_no > 3 )
				crt->cond = 0;
			crt->rect = rect_left[crt->ani_no];
			break;
			
		case 1:
			crt->rect = rect_up[++crt->ani_wait / 2 % 3];
			if (crt->ani_wait > 24)
				crt->cond = 0;
			break;
			
		case 2:
			if ( ++crt->ani_wait > 2 )
			{
				crt->ani_wait = 0;
				++crt->ani_no;
			}
			if ( crt->ani_no > 3 )
				crt->cond = 0;
			crt->rect = rect_right[crt->ani_no];
			break;
	}
}

void ActCaret03(CARET *crt)
{
	RECT rect[4];
	rect[0] = {0, 48, 16, 64};
	rect[1] = {16, 48, 32, 64};
	rect[2] = {32, 48, 48, 64};
	rect[3] = {48, 48, 64, 64};
	
	if (++crt->ani_wait > 2)
	{
		crt->ani_wait = 0;
		if (++crt->ani_no > 3)
			crt->cond = 0;
	}
	
	crt->rect = rect[crt->ani_no];
}

void ActCaret04(CARET *crt)
{
	RECT rect[9];

	rect[0] = {64, 32, 80, 48};
	rect[1] = {80, 32, 96, 48};
	rect[2] = {96, 32, 112, 48};
	rect[3] = {64, 48, 80, 64};
	rect[4] = {80, 48, 96, 64};
	rect[5] = {96, 48, 112, 64};
	rect[6] = {64, 64, 80, 80};
	rect[7] = {80, 64, 96, 80};
	rect[8] = {96, 64, 112, 80};

	if (++crt->ani_wait > 1)
	{
		crt->ani_wait = 0;

		if (++crt->ani_no > 2)
			crt->cond = 0;
	}

	crt->rect = rect[crt->ani_no + 3 * crt->direct];
}

void ActCaret05(CARET *crt)
{
	RECT rect[7];

	rect[0] = {32, 64, 40, 72};
	rect[1] = {32, 72, 40, 80};
	rect[2] = {40, 64, 48, 72};
	rect[3] = {40, 72, 48, 80};
	rect[4] = {40, 64, 48, 72};
	rect[5] = {40, 72, 48, 80};
	rect[6] = {40, 64, 48, 72};

	if (++crt->ani_wait > 4)
	{
		crt->ani_wait = 0;
		++crt->ani_no;
	}

	if ( crt->ani_no > 6 )
		crt->cond = 0;

	crt->x += 0x80;
	crt->y -= 0x80;

	crt->rect = rect[crt->ani_no];
}

void ActCaret07(CARET *crt)
{
	RECT rcLeft[7];

	rcLeft[0] = {56, 0, 64, 8};
	rcLeft[1] = {64, 0, 72, 8};
	rcLeft[2] = {72, 0, 80, 8};
	rcLeft[3] = {80, 0, 88, 8};
	rcLeft[4] = {88, 0, 96, 8};
	rcLeft[5] = {96, 0, 104, 8};
	rcLeft[6] = {104, 0, 112, 8};

	if (++crt->ani_wait > 1)
	{
		crt->ani_wait = 0;

		if (++crt->ani_no > 6)
			crt->cond = 0;
	}

	crt->rect = rcLeft[crt->ani_no];

	switch (crt->direct)
	{
		case 0:
			crt->x -= 0x400;
			break;
		case 1:
			crt->y -= 0x400;
			break;
		case 2:
			crt->x += 0x400;
			break;
		case 3:
			crt->y += 0x400;
			break;
	}
}

void ActCaret08(CARET *crt)
{
	RECT rcLeft = {16, 80, 32, 96};
	RECT rcRight = {32, 80, 48, 96};

	if (crt->direct == 0)
		crt->rect = rcLeft;
	else
		crt->rect = rcRight;
}

void ActCaret09(CARET *crt)
{
	RECT rcLeft = {0, 80, 16, 96};
	RECT rcRight = {48, 64, 64, 80};

	if (++crt->ani_wait <= 4)
		crt->y -= 0x800;
	if (crt->ani_wait == 32 )
		crt->cond = 0;
	
	if (crt->direct == 0)
		crt->rect = rcLeft;
	else
		crt->rect = rcRight;
}

void ActCaret10(CARET *crt)
{
	RECT rcLeft[2];
	RECT rcRight[2];
	rcLeft[0] = {0, 0, 56, 16};
	rcLeft[1] = {0, 16, 56, 32};
	rcRight[0] = {0, 96, 56, 112};
	rcRight[1] = {0, 112, 56, 128};
	
	++crt->ani_wait;
	
	if (crt->direct)
	{
		if (crt->ani_wait < 20)
			crt->y -= 0x200;
		if (crt->ani_wait == 80)
		crt->cond = 0;
	}
	else
	{
		if (crt->ani_wait < 20)
			crt->y -= 0x400;
		if (crt->ani_wait == 80)
			crt->cond = 0;
	}
	
	if (crt->direct)
		crt->rect = rcRight[crt->ani_wait / 2 % 2];
	else
		crt->rect = rcLeft[crt->ani_wait / 2 % 2];
}

void ActCaret11(CARET *crt)
{
	if (!crt->act_no)
	{
		crt->act_no = 1;
		uint8_t deg = Random(0, 0xFF);
		crt->xm = 2 * GetCos(deg);
		crt->ym = 2 * GetSin(deg);
	}
	
	crt->x += crt->xm;
	crt->y += crt->ym;
	
	RECT rcRight[7];
	rcRight[0] = {56, 8, 64, 16};
	rcRight[1] = {64, 8, 72, 16};
	rcRight[2] = {72, 8, 80, 16};
	rcRight[3] = {80, 8, 88, 16};
	rcRight[4] = {88, 8, 96, 16};
	rcRight[5] = {96, 8, 104, 16};
	rcRight[6] = {104, 8, 112, 16};

	if (++crt->ani_wait > 2)
	{
		crt->ani_wait = 0;
		if (++crt->ani_no > 6)
			crt->cond = 0;
	}
	
	crt->rect = rcRight[crt->ani_no];
}

void ActCaret12(CARET *crt)
{
	RECT rcLeft[2];
	rcLeft[0] = {112, 0, 144, 32};
	rcLeft[1] = {144, 0, 176, 32};
	
	if (++crt->ani_wait > 2)
	{
		crt->ani_wait = 0;
		if (++crt->ani_no > 1)
		  crt->cond = 0;
	}
	
	crt->rect = rcLeft[crt->ani_no];
}

void ActCaret13(CARET *crt)
{
	RECT rcLeft[2];
	rcLeft[0] = {56, 24, 64, 32};
	rcLeft[1] = {0, 0, 0, 0};
	
	if (!crt->act_no)
	{
		crt->act_no = 1;
		
		switch (crt->direct)
		{
			case 0:
				crt->xm = Random(-0x600, 0x600);
				crt->ym = Random(-0x200, 0x200);
				break;
				
			case 1:
				crt->ym = -0x200 * Random(1, 3);
				break;
		}
	}
	
	if (!crt->direct)
	{
		crt->xm = 4 * crt->xm / 5;
		crt->ym = 4 * crt->ym / 5;
	}
	
	crt->x += crt->xm;
	crt->y += crt->ym;
	
	if (++crt->ani_wait > 20)
		crt->cond = 0;

	crt->rect = rcLeft[crt->ani_wait / 2 % 2];
	
	if (crt->direct == 5)
		crt->x -= 0x800;
}

void ActCaret14(CARET *crt)
{
	RECT rect[5];

	rect[0] = {0, 96, 40, 136};
	rect[1] = {40, 96, 80, 136};
	rect[2] = {80, 96, 120, 136};
	rect[3] = {120, 96, 160, 136};
	rect[4] = {160, 96, 200, 136};

	if (++crt->ani_wait > 1)
	{
		crt->ani_wait = 0;

		if (++crt->ani_no > 4)
			crt->cond = 0;
	}

	crt->rect = rect[crt->ani_no];
}

void ActCaret15(CARET *crt)
{
	RECT rcLeft[4];

	rcLeft[0] = {0, 72, 8, 80};
	rcLeft[1] = {8, 72, 16, 80};
	rcLeft[2] = {16, 72, 24, 80};
	rcLeft[3] = {24, 72, 32, 80};

	if (++crt->ani_wait > 2)
	{
		crt->ani_wait = 0;

		if (++crt->ani_no > 3)
			crt->cond = 0;
	}

	crt->rect = rcLeft[crt->ani_no];
}

void ActCaret16(CARET *crt)
{
	RECT rcLeft[2];

	rcLeft[0] = {104, 96, 144, 104};
	rcLeft[1] = {104, 104, 144, 112};

	if (++crt->ani_wait < 10)
		crt->y -= 0x400;

	if (crt->ani_wait == 40)
		crt->cond = 0;

	crt->rect = rcLeft[crt->ani_wait / 2 % 2];
}

void ActCaret17(CARET *crt)
{
	RECT rcLeft[2];

	rcLeft[0] = {0, 144, 144, 152};
	rcLeft[1] = {0, 0, 0, 0};

	if (++crt->ani_wait >= 40)
		crt->ani_wait = 0;

	if (crt->ani_wait < 30)
		crt->rect = rcLeft[0];
	else
		crt->rect = rcLeft[1];
}

//Tables
CARET_TABLE gCaretTable[18] =
{
	{0, 0},
	{0x800, 0x800},
	{0x1000, 0x1000},
	{0x1000, 0x1000},
	{0x1000, 0x1000},
	{0x800, 0x800},
	{0x1000, 0x1000},
	{0x800, 0x800},
	{0x1000, 0x1000},
	{0x1000, 0x1000},
	{0x3800, 0x1000},
	{0x800, 0x800},
	{0x2000, 0x2000},
	{0x800, 0x800},
	{0x2800, 0x2800},
	{0x800, 0x800},
	{0x2800, 0x800},
	{0x6800, 0x800}
};

typedef void (*CARETFUNCTION)(CARET*);
CARETFUNCTION gpCaretFuncTbl[] =
{
	ActCaret00,
	ActCaret01,
	ActCaret02,
	ActCaret03,
	ActCaret04,
	ActCaret05,
	ActCaret04,
	ActCaret07,
	ActCaret08,
	ActCaret09,
	ActCaret10,
	ActCaret11,
	ActCaret12,
	ActCaret13,
	ActCaret14,
	ActCaret15,
	ActCaret16,
	ActCaret17,
};

void ActCaret()
{
	for (int i = 0; i < CARET_MAX; i++)
	{
		if (gCrt[i].cond & 0x80)
			gpCaretFuncTbl[gCrt[i].code](&gCrt[i]);
	}
}

void PutCaret(int fx, int fy)
{
	for (int i = 0; i < CARET_MAX; i++)
	{
		if (gCrt[i].cond & 0x80)
		{
			PutBitmap3(
				&grcGame,
				(gCrt[i].x - gCrt[i].view_left) / 0x200 - fx / 0x200,
				(gCrt[i].y - gCrt[i].view_top) / 0x200 - fy / 0x200,
				&gCrt[i].rect,
				19);
		}
	}
}

void SetCaret(int x, int y, int code, int dir)
{
	for (int c = 0; c < CARET_MAX; c++)
	{
		if (!gCrt[c].cond)
		{
			memset(&gCrt[c], 0, sizeof(CARET));
			gCrt[c].cond = 0x80;
			gCrt[c].code = code;
			gCrt[c].x = x;
			gCrt[c].y = y;
			gCrt[c].view_left = gCaretTable[code].view_left;
			gCrt[c].view_top = gCaretTable[code].view_top;
			gCrt[c].direct = dir;
			break;
		}
	}
}
