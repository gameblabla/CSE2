#pragma once
#include <stdint.h>

extern int8_t gMapping[0x80];

int MiniMapLoop();
bool IsMapping();
void StartMapping();
void SetMapping(int a);
