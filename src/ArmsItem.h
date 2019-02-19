#pragma once

#include "WindowsWrapper.h"

struct ARMS
{
	int code;
	int level;
	int exp;
	int max_num;
	int num;
};

struct ITEM
{
	int code;
};

#define ARMS_MAX 8
#define ITEM_MAX 32

extern int gArmsEnergyX;

extern int gSelectedArms;
extern int gSelectedItem;

extern ARMS gArmsData[ARMS_MAX];
extern ITEM gItemData[ITEM_MAX];
extern int gSelectedArms;
extern int gSelectedItem;
extern int gCampTitleY;
extern BOOL gCampActive;

void ClearArmsData();
void ClearItemData();
BOOL AddArmsData(long code, long max_num);
BOOL SubArmsData(long code);
BOOL TradeArms(long code1, long code2, long max_num);
BOOL AddItemData(long code);
BOOL SubItemData(long code);
int CampLoop();
BOOL CheckItem(long a);
BOOL CheckArms(long a);
BOOL UseArmsEnergy(long num);
BOOL ChargeArmsEnergy(long num);
void FullArmsEnergy();
int RotationArms();
int RotationArmsRev();
void ChangeToFirstArms();
