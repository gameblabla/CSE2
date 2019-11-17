#pragma once

#include "WindowsWrapper.h"

extern unsigned char gFlagNPC[1000];
extern unsigned char gSkipFlag[8];

void InitFlags(void);
void InitSkipFlags(void);
void SetNPCFlag(long a);
void CutNPCFlag(long a);
BOOL GetNPCFlag(long a);
void SetSkipFlag(long a);
void CutSkipFlag(long a);
BOOL GetSkipFlag(long a);
