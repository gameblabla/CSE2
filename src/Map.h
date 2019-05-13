#pragma once

#include "WindowsWrapper.h"

struct MAP_DATA
{
	unsigned char *data;
	unsigned char atrb[0x101]; //Why is this 257 bytes?
	short width;
	short length;
};

extern MAP_DATA gMap;

BOOL InitMapData2();
BOOL LoadMapData2(const char *path_map);
BOOL LoadAttributeData(const char *path_atrb);
void EndMapData();
void ReleasePartsImage();
void GetMapData(unsigned char **data, short *mw, short *ml);
unsigned char GetAttribute(int x, int y);
void DeleteMapParts(int x, int y);
void ShiftMapParts(int x, int y);
BOOL ChangeMapParts(int x, int y, unsigned char no);
void PutStage_Back(int fx, int fy);
void PutStage_Front(int fx, int fy);
void PutMapDataVector(int fx, int fy);
