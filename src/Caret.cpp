#include <string>

#include "WindowsWrapper.h"

#include "Caret.h"
#include "Draw.h"
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
	nullptr, //&ActCaret02,
	nullptr, //&ActCaret03,
	nullptr, //&ActCaret04,
	nullptr, //&ActCaret05,
	nullptr, //&ActCaret04,
	nullptr, //&ActCaret07,
	ActCaret08,
	ActCaret09,
	nullptr, //&ActCaret10,
	nullptr, //&ActCaret11,
	nullptr, //&ActCaret12,
	ActCaret13,
	nullptr, //&ActCaret14,
	nullptr, //&ActCaret15,
	nullptr, //&ActCaret16,
	nullptr, //&ActCaret17
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
