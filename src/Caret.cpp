#include <string>

#include "WindowsWrapper.h"

#include "Caret.h"
#include "Draw.h"

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
	&ActCaret00,
	nullptr, //&ActCaret01,
	nullptr, //&ActCaret02,
	nullptr, //&ActCaret03,
	nullptr, //&ActCaret04,
	nullptr, //&ActCaret05,
	nullptr, //&ActCaret04,
	nullptr, //&ActCaret07,
	nullptr, //&ActCaret08,
	nullptr, //&ActCaret09,
	nullptr, //&ActCaret10,
	nullptr, //&ActCaret11,
	nullptr, //&ActCaret12,
	nullptr, //&ActCaret13,
	nullptr, //&ActCaret14,
	nullptr, //&ActCaret15,
	nullptr, //&ActCaret16,
	nullptr //&ActCaret17
};

void ActCaret()
{
	for (int i = 0; i < CARET_MAX; i++)
	{
		if (gCrt[i].cond & 0x80 && gpCaretFuncTbl[gCrt[i].code] != nullptr)
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
		if (gCrt[c].cond)
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
