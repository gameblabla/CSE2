#include "Flags.h"

#include <string.h>

#include "WindowsWrapper.h"

unsigned char gFlagNPC[1000];
unsigned char gSkipFlag[0x40];

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
void SetNPCFlag(long a)
{
	gFlagNPC[a / 8] |= 1 << a % 8;
}

void CutNPCFlag(long a)
{
	gFlagNPC[a / 8] &= ~(1 << a % 8);
}

BOOL GetNPCFlag(long a)
{
	if (gFlagNPC[a / 8] & (1 << a % 8))
		return TRUE;
	else
		return FALSE;
}

//Skip flags
void SetSkipFlag(long a)
{
	gSkipFlag[a / 8] |= 1 << a % 8;
}

void CutSkipFlag(long a)
{
	gSkipFlag[a / 8] &= ~(1 << a % 8);
}

BOOL GetSkipFlag(long a)
{
	if (gSkipFlag[a / 8] & (1 << a % 8))
		return TRUE;
	else
		return FALSE;
}
