#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char gMapping[0x80];

int MiniMapLoop(void);
BOOL IsMapping(void);
void StartMapping(void);
void SetMapping(int a);

#ifdef __cplusplus
}
#endif
