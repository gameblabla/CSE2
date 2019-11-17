#pragma once

#include "WindowsWrapper.h"

void InitFade(void);
void SetFadeMask(void);
void ClearFade(void);
void StartFadeOut(signed char dir);
void StartFadeIn(signed char dir);
void ProcFade(void);
void PutFade(void);
BOOL GetFadeActive(void);
