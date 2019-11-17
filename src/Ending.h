#pragma once

#include "WindowsWrapper.h"

#include "CommonDefines.h"

struct CREDIT
{
	long size;
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

void ActionStripper(void);
void PutStripper(void);
void SetStripper(int x, int y, const char *text, int cast);
void RestoreStripper(void);
void ActionIllust(void);
void PutIllust(void);
void ReloadIllust(int a);
void InitCreditScript(void);
void ReleaseCreditScript(void);
BOOL StartCreditScript(void);
void ActionCredit(void);
void ActionCredit_Read(void);
int GetScriptNumber(const char *text);
void SetCreditIllust(int a);
void CutCreditIllust(void);
int Scene_DownIsland(int mode);
