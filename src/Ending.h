#pragma once

#include "WindowsWrapper.h"

#include "CommonDefines.h"

struct CREDIT
{
	int size;
	char *pData;
	int offset;
	int wait;
	int mode;
	int start_x;
};

struct STRIP
{
	int flag;
	int x;
	int y;
	int cast;
	char str[0x40];
};

struct ILLUSTRATION
{
	int act_no;
	int x;
};

struct ISLAND_SPRITE
{
	int x;
	int y;
};

#define MAX_STRIP ((WINDOW_HEIGHT / 16) + 1)

void ActionStripper();
void PutStripper();
void SetStripper(int x, int y, const char *text, int cast);
void RestoreStripper();
void ActionIllust();
void PutIllust();
void ReloadIllust(int a);
void InitCreditScript();
void ReleaseCreditScript();
BOOL StartCreditScript();
void ActionCredit();
void SetCreditIllust(int a);
void CutCreditIllust();
int Scene_DownIsland(HWND hWnd, int mode);
