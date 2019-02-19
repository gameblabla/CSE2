#pragma once

struct MAP_NAME
{
	int flag;
	int wait;
	char name[0x20];
};

void ReadyMapName(const char *str);
void PutMapName(bool bMini);
void StartMapName();
void RestoreMapName();
