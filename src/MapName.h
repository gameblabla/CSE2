#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

void ReadyMapName(const char *str);
void PutMapName(BOOL bMini);
void StartMapName(void);
void RestoreMapName(void);

#ifdef __cplusplus
}
#endif
