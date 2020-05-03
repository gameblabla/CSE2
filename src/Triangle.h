#pragma once

extern int gSin[0x100];
extern short gTan[0x21];

void InitTriangleTable(void);
int GetSin(unsigned char deg);
int GetCos(unsigned char deg);
unsigned char GetArktan(int x, int y);
