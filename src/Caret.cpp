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
	;
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

void ActCaret08(CARET *crt)
{
	if (crt->direct)
		crt->rect = {32, 80, 48, 96};
	else
		crt->rect = {16, 80, 32, 96};
}

void ActCaret09(CARET *crt)
{
	if (++crt->ani_wait <= 4)
		crt->y -= 0x800;
	if (crt->ani_wait == 32 )
		crt->cond = 0;
	
	if (crt->direct)
		crt->rect = {48, 64, 64, 80};
	else
		crt->rect = {0, 80, 16, 96};
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
	nullptr, //ActCaret04,
	nullptr, //ActCaret05,
	nullptr, //ActCaret04,
	nullptr, //ActCaret07,
	ActCaret08,
	ActCaret09,
	nullptr, //ActCaret10,
	ActCaret11,
	nullptr, //ActCaret12,
	ActCaret13,
	nullptr, //ActCaret14,
	nullptr, //ActCaret15,
	nullptr, //ActCaret16,
	nullptr, //ActCaret17,
};

void ActCaret()
{
	for (int i = 0; i < CARET_MAX; i++)
	{
		if ((gCrt[i].cond & 0x80) && gpCaretFuncTbl[gCrt[i].code] != nullptr)
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
