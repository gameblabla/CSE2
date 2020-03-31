#pragma once

#include "../WindowsWrapper.h"

extern BOOL bActive;

void PlatformBackend_Init(void);
void PlatformBackend_Deinit(void);
BOOL PlatformBackend_GetBasePath(char *string_buffer);
BOOL PlatformBackend_SystemTask(void);
void PlatformBackend_ShowMessageBox(const char *title, const char *message);
unsigned long PlatformBackend_GetTicks(void);
void PlatformBackend_Delay(unsigned int ticks);
