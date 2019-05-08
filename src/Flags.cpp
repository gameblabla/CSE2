#include "Flags.h"

#include <string.h>

#include "WindowsWrapper.h"

unsigned char gFlagNPC[1000];
unsigned char gSkipFlag[8];

#define SET_BIT(x, i) ((x)[(i) / 8] |= 1 << (i) % 8;)
#define UNSET_BIT(x, i) ((x)[(i) / 8] &= ~(1 << (i) % 8);)
#define GET_BIT(x, i) ((x)[(i) / 8] & (1 << (i) % 8))

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
	SET_BIT(gFlagNPC, a);
}

void CutNPCFlag(long a)
{
	UNSET_BIT(gFlagNPC, a);
}

BOOL GetNPCFlag(long a)
{
	if (GET_BIT(gFlagNPC, a))
		return TRUE;
	else
		return FALSE;
}

//Skip flags
void SetSkipFlag(long a)
{
	SET_BIT(gSkipFlag, a);
}

void CutSkipFlag(long a)
{
	UNSET_BIT(gSkipFlag, a);
}

BOOL GetSkipFlag(long a)
{
	if (GET_BIT(gSkipFlag, a))
		return TRUE;
	else
		return FALSE;
}
