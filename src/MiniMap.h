#pragma once

#include "WindowsWrapper.h"

extern char gMapping[0x80];

int MiniMapLoop();
BOOL IsMapping();
void StartMapping();
void SetMapping(int a);
