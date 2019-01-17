#include <cstring>
#include <stdint.h>

uint8_t gFlagNPC[1000];
uint8_t gSkipFlag[0x40];

//Flag inits
void InitFlags()
{
	memset(gFlagNPC, 0, sizeof(gFlagNPC));
}

void InitSkipFlags()
{
	memset(gSkipFlag, 0, sizeof(gSkipFlag));
}

//NPC flags
void SetNPCFlag(int a)
{
	gFlagNPC[a / 8] |= 1 << a % 8;
}

void CutNPCFlag(int a)
{
	gFlagNPC[a / 8] &= ~(1 << a % 8);
}

bool GetNPCFlag(int a)
{
	return ((gFlagNPC[a / 8] >> a % 8) & 1) != 0;
}

//Skip flags
void SetSkipFlag(int a)
{
	gSkipFlag[a / 8] |= 1 << a % 8;
}

void CutSkipFlag(int a)
{
	gSkipFlag[a / 8] &= ~(1 << a % 8);
}

bool GetSkipFlag(int a)
{
	return ((gSkipFlag[a / 8] >> a % 8) & 1) != 0;
}
