#pragma once

#include "WindowsWrapper.h"

#include "MainLoop.h"

extern char gMapping[0x80];

void MiniMapLoop(MainLoopMeta *meta);
BOOL IsMapping();
void StartMapping();
void SetMapping(int a);
