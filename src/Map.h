#pragma once

#include <stdint.h>

#include "WindowsWrapper.h"

struct MAP_DATA
{
	uint8_t *data;
	uint8_t atrb[0x101]; //Why is this 257 bytes?
	int16_t width;
	int16_t length;
};

extern MAP_DATA gMap;

BOOL InitMapData2();
BOOL LoadMapData2(const char *path_map);
BOOL LoadAttributeData(const char *path_atrb);
void EndMapData();
void ReleasePartsImage();
void GetMapData(uint8_t **data, int16_t *mw, int16_t *ml);
unsigned char GetAttribute(int x, int y);
void DeleteMapParts(int x, int y);
void ShiftMapParts(int x, int y);
BOOL ChangeMapParts(int x, int y, uint8_t no);
void PutStage_Back(int fx, int fy);
void PutStage_Front(int fx, int fy);
void PutMapDataVector(int fx, int fy);
