#include "Flags.h"

#include <string.h>

#include "WindowsWrapper.h"

unsigned char gFlagNPC[1000];
unsigned char gSkipFlag[8];

//Flag inits
void InitFlags()
{
	memset(gFlagNPC, 0, sizeof(gFlagNPC));
}

void InitSkipFlags()
{
	memset(gSkipFlag, 0, sizeof(gSkipFlag));
}

// Macros for setting, un-setting and getting flags
// Each flag is stored in a single bit
#define SET_BIT(x, i) ((x)[(i) / 8] |= 1 << (i) % 8)
#define UNSET_BIT(x, i) ((x)[(i) / 8] &= ~(1 << (i) % 8))
#define GET_BIT(x, i) ((x)[(i) / 8] & (1 << (i) % 8))
#define SET_FLAG SET_BIT
#define UNSET_FLAG UNSET_BIT
#define GET_FLAG GET_BIT

//NPC flags
void SetNPCFlag(long a)
{
	SET_FLAG(gFlagNPC, a);
}

void CutNPCFlag(long a)
{
	UNSET_FLAG(gFlagNPC, a);
}

BOOL GetNPCFlag(long a)
{
	if (GET_FLAG(gFlagNPC, a))
		return TRUE;
	else
		return FALSE;
}

//Skip flags
void SetSkipFlag(long a)
{
	SET_FLAG(gSkipFlag, a);
}

void CutSkipFlag(long a)
{
	UNSET_FLAG(gSkipFlag, a);
}

BOOL GetSkipFlag(long a)
{
	if (GET_FLAG(gSkipFlag, a))
		return TRUE;
	else
		return FALSE;
}
