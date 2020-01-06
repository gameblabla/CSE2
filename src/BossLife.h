#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitBossLife(void);
BOOL StartBossLife(int code_event);
BOOL StartBossLife2(void);
void PutBossLife(void);

#ifdef __cplusplus
}
#endif
