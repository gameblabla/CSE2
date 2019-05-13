#pragma once

#include "WindowsWrapper.h"

struct ARMS_LEVEL
{
	int exp[3];
};

struct REC
{
	long counter[4];
	unsigned char random[4];
};

extern ARMS_LEVEL gArmsLevelTable[14];

void AddExpMyChar(int x);
void ZeroExpMyChar();
BOOL IsMaxExpMyChar();
void DamageMyChar(int damage);
void ZeroArmsEnergy_All();
void AddBulletMyChar(int no, int val);
void AddLifeMyChar(int x);
void AddMaxLifeMyChar(int val);
void PutArmsEnergy(BOOL flash);
void PutActiveArmsList();
void PutMyLife(BOOL flash);
void PutMyAir(int x, int y);
void PutTimeCounter(int x, int y);
BOOL SaveTimeCounter();
int LoadTimeCounter();
