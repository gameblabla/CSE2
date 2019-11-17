#pragma once

#include "WindowsWrapper.h"

// TODO - When I add bitmask constants for gMC.flags...
// 0x100 is a 'player is underwater' flag

struct MYCHAR
{
	unsigned char cond;
	unsigned int flag;
	int direct;
	BOOL up;
	BOOL down;
	int unit;
	int equip;
	int x;
	int y;
	int tgt_x;
	int tgt_y;
	int index_x;
	int index_y;
	int xm;
	int ym;
	int ani_wait;
	int ani_no;
	struct
	{
		int front;
		int top;
		int back;
		int bottom;
	} hit;
	struct
	{
		int front;
		int top;
		int back;
		int bottom;
	} view;
	RECT rect;
	RECT rect_arms;
	int level;
	int exp_wait;
	int exp_count;
	unsigned char shock;
	unsigned char no_life;
	unsigned char rensha;
	unsigned char bubble;
	short life;
	short star;
	short max_life;
	short a;
	int lifeBr;
	int lifeBr_count;
	int air;
	int air_get;
	signed char sprash; // This is explicitly a char, but used like a BOOL
	signed char ques;   // Same for this variable as well
	signed char boost_sw;
	int boost_cnt;
};

extern MYCHAR gMC;

void InitMyChar(void);
void AnimationMyChar(BOOL bKey);
void ShowMyChar(BOOL bShow);
void PutMyChar(int fx, int fy);
void ActMyChar_Normal(BOOL bKey);
void ActMyChar(BOOL bKey);
void GetMyCharPosition(int *x, int *y);
void SetMyCharPosition(int x, int y);
void MoveMyChar(int x, int y);
void ZeroMyCharXMove(void);
int GetUnitMyChar(void);
void SetMyCharDirect(unsigned char dir);
void ChangeMyUnit(unsigned char a);
void PitMyChar(void);
void EquipItem(int flag, BOOL b);
void ResetCheck(void);
void SetNoise(int no, int freq);
void CutNoise(void);
void ResetNoise(void);
void SleepNoise(void);
