#pragma once

#include "../WindowsWrapper.h"

extern BOOL bActive;

void PlatformBackend_Init(void);

void PlatformBackend_GetBasePath(char *string_buffer);

BOOL PlatformBackend_SystemTask(void);
