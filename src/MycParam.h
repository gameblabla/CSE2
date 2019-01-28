#pragma once
struct ARMS_LEVEL
{
	int exp[3];
};

extern ARMS_LEVEL gArmsLevelTable[14];

void DamageMyChar(int damage);
void AddLifeMyChar(int x);
void AddMaxLifeMyChar(int val);
void PutArmsEnergy(bool flash);
void PutActiveArmsList();
void PutMyLife(bool flash);
void PutMyAir(int x, int y);
