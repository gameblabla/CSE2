#pragma once

#include "WindowsWrapper.h"

enum GameFlagsValues
{
	// To be continued
	GAME_FLAG_IS_CONTROL_ENABLED = 2, // Idk tbh
	// To be continued
};

extern int g_GameFlags;
extern int gCounter;

int Random(int min, int max);
void PutNumber4(int x, int y, int value, BOOL bZero);

BOOL Game(HWND hWnd);
