#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitFade(void);
void SetFadeMask(void);
void ClearFade(void);
void StartFadeOut(signed char dir);
void StartFadeIn(signed char dir);
void ProcFade(void);
void PutFade(void);
BOOL GetFadeActive(void);

#ifdef __cplusplus
}
#endif
