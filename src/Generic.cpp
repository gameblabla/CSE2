#include <stdint.h>

bool IsShiftJIS(uint8_t c)
{
	if ( c > 0x80 && c < 0xA0 )
		return true;
	if ( c < 0xE0 || c >= 0xF0 )
		return false;
	return true;
}
