#pragma once

#include <stdint.h>

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

#define MAX_STRIP 16

void ActionStripper();
void PutStripper();
void SetStripper(int x, int y, char *text, int cast);
void RestoreStripper();
void ActionIllust();
void PutIllust();
void ReloadIllust(int a);
void InitCreditScript();
void ReleaseCreditScript();
bool StartCreditScript();
void ActionCredit();
void SetCreditIllust(int a);
void CutCreditIllust();
int Scene_DownIsland(int mode);
