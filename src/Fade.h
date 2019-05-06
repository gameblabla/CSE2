#pragma once

#include "WindowsWrapper.h"

void InitFade();
void SetFadeMask();
void ClearFade();
void StartFadeOut(signed char dir);
void StartFadeIn(signed char dir);
void ProcFade();
void PutFade();
BOOL GetFadeActive();
