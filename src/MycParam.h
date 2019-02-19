#pragma once

#include <stdint.h>

struct ARMS_LEVEL
{
	int exp[3];
};

struct REC
{
	int32_t counter[4];
	uint8_t random[4];
};

extern ARMS_LEVEL gArmsLevelTable[14];

void AddExpMyChar(int x);
void ZeroExpMyChar();
bool IsMaxExpMyChar();
void DamageMyChar(int damage);
void ZeroArmsEnergy_All();
void AddBulletMyChar(int no, int val);
void AddLifeMyChar(int x);
void AddMaxLifeMyChar(int val);
void PutArmsEnergy(bool flash);
void PutActiveArmsList();
void PutMyLife(bool flash);
void PutMyAir(int x, int y);
void PutTimeCounter(int x, int y);
bool SaveTimeCounter();
int LoadTimeCounter();
