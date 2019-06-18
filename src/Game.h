#pragma once

#include "WindowsWrapper.h"

#include "MainLoop.h"

extern int g_GameFlags;
extern int gCounter;

int Random(int min, int max);
void PutNumber4(int x, int y, int value, BOOL bZero);

void Game(MainLoopMeta *meta);
