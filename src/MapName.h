#pragma once

#include "WindowsWrapper.h"

typedef struct MAP_NAME
{
	BOOL flag;
	int wait;
	char name[0x20];
} MAP_NAME;

extern MAP_NAME gMapName;

void ReadyMapName(const char *str);
void PutMapName(BOOL bMini);
void StartMapName(void);
void RestoreMapName(void);
