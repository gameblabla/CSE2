#pragma once

#include "CommonDefines.h"
#include "WindowsWrapper.h"

#define FADE_WIDTH	(((WINDOW_WIDTH - 1) >> 4) + 1)
#define FADE_HEIGHT	(((WINDOW_HEIGHT - 1) >> 4) + 1)

struct FADE
{
	int mode;
	BOOL bMask;
	int count;
	char ani_no[FADE_HEIGHT][FADE_WIDTH];
	BOOLEAN flag[FADE_HEIGHT][FADE_WIDTH];
	char dir;
};

void InitFade();
void SetFadeMask();
void ClearFade();
void StartFadeOut(char dir);
void StartFadeIn(char dir);
void ProcFade();
void PutFade();
BOOL GetFadeActive();
