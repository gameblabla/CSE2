#pragma once

#include "WindowsWrapper.h"

void InitFade();
void SetFadeMask();
void ClearFade();
void StartFadeOut(char dir);
void StartFadeIn(char dir);
void ProcFade();
void PutFade();
BOOL GetFadeActive();
