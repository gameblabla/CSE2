#pragma once

#include <stdint.h>

#include "WindowsWrapper.h"

extern BOOLEAN gMapping[0x80];

int MiniMapLoop();
BOOL IsMapping();
void StartMapping();
void SetMapping(int a);
