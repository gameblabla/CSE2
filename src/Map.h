#pragma once
#include <stdint.h>

struct MAP_DATA
{
	uint8_t *data;
	uint8_t atrb[0x101]; //Why is this 257 bytes?
	int16_t width;
	int16_t length;
};

bool InitMapData2();
