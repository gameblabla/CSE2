#pragma once

#include "../WindowsWrapper.h"

extern BOOL bActive;

void PlatformBackend_Init(void);

void PlatformBackend_GetBasePath(char *string_buffer);

BOOL PlatformBackend_SystemTask(void);

void PlatformBackend_ShowMessageBox(const char *title, const char *message);
