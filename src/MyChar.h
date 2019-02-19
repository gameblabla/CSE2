#pragma once

#include <stdint.h>

#include "WindowsWrapper.h"

struct MYCHAR
{
	uint8_t cond;
	unsigned int flag;
	int direct;
	int up;
	int down;
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
	RECT hit;
	RECT view;
	RECT rect;
	RECT rect_arms;
	int level;
	int exp_wait;
	int exp_count;
	uint8_t shock;
	uint8_t no_life;
	uint8_t rensha;
	uint8_t bubble;
	int16_t life;
	int16_t star;
	int16_t max_life;
	int16_t a;
	int lifeBr;
	int lifeBr_count;
	int air;
	int air_get;
	int8_t sprash;
	int8_t ques;
	int8_t boost_sw;
	int boost_cnt;
};

extern MYCHAR gMC;

void InitMyChar();
void AnimationMyChar(bool bKey);
void ShowMyChar(bool bShow);
void PutMyChar(int fx, int fy);
void ActMyChar_Normal(bool bKey);
void ActMyChar(bool bKey);
void GetMyCharPosition(int *x, int *y);
void SetMyCharPosition(int x, int y);
void MoveMyChar(int x, int y);
void ZeroMyCharXMove();
int GetUnitMyChar();
void SetMyCharDirect(uint8_t dir);
void ChangeMyUnit(uint8_t a);
void PitMyChar();
void EquipItem(int flag, bool b);
void ResetCheck();
void SetNoise(int no, int freq);
void CutNoise();
void ResetNoise();
void SleepNoise();
