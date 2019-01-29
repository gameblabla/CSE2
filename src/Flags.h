#pragma once
#include <stdint.h>
extern uint8_t gFlagNPC[1000];
extern uint8_t gSkipFlag[0x40];

void InitFlags();
void InitSkipFlags();
void SetNPCFlag(int a);
void CutNPCFlag(int a);
bool GetNPCFlag(int a);
void SetSkipFlag(int a);
void CutSkipFlag(int a);
bool GetSkipFlag(int a);
