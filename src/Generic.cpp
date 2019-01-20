#include <stdint.h>

bool GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	*v1 = 1;
	*v2 = 0;
	*v3 = 0;
	*v4 = 6;
	return true;
}

bool IsShiftJIS(uint8_t c)
{
	if ( c > 0x80 && c < 0xA0 )
		return true;
	if ( c < 0xE0 || c >= 0xF0 )
		return false;
	return true;
}
