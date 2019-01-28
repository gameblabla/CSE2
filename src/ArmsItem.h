#pragma once
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
extern int gCampActive;

void ClearArmsData();
void ClearItemData();
bool AddArmsData(int code, int max_num);
bool SubArmsData(int code);
bool TradeArms(int code1, int code2, int max_num);
bool AddItemData(int code);
bool SubItemData(int code);
int CampLoop();
bool CheckItem(int a);
bool CheckArms(int a);
bool UseArmsEnergy(int num);
bool ChargeArmsEnergy(int num);
void FullArmsEnergy();
int RotationArms();
int RotationArmsRev();
void ChangeToFirstArms();
