#include <stdint.h>
#include <string>

#include "Map.h"

#define PXM_BUFFER_SIZE 0x4B000

MAP_DATA gMap;

bool InitMapData2()
{
	gMap.data = (uint8_t*)malloc(PXM_BUFFER_SIZE);
	return true;
}
