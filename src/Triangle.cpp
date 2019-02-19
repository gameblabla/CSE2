#include "Triangle.h"

#include <stdint.h>
#include <math.h>

int gSin[0x100];
int16_t gTan[0x21];

void InitTriangleTable()
{
	//Sine
	for (int i = 0; i < 0x100; ++i )
	{
		float v0 = i * 6.2831998 / 256.0;
		gSin[i] = (int)(sinf(v0) * 512.0);
	}
	
	//Tangent
	for (int i = 0; i < 0x21; ++i )
	{
		float a = i * 6.2831855 / 256.0;
		float v2 = sinf(a);
		float b = v2 / cosf(a);
		gTan[i] = (int16_t)(b * 8192.0);
	}
}

int GetSin(uint8_t deg)
{
	return gSin[deg];
}

int GetCos(uint8_t deg)
{
	return gSin[(uint8_t)(deg + 0x40)];
}

int GetArktan(int x, int y)
{
	int xa = -x;
	int ya = -y;
	uint8_t a = 0;
	
	if (xa <= 0)
	{
		if (ya <= 0)
		{
			if (-xa <= -ya)
			{
				while (gTan[a] < (int16_t)(-0x2000 * xa / -ya))
					++a;
				a = -0x40 - a;
			}
			else
			{
				while (gTan[a] < (int16_t)(-0x2000 * ya / -xa))
					++a;
				a += -0x80;
			}
		}
		else if (-xa <= ya)
		{
			while (gTan[a] < (int16_t)(-0x2000 * xa / ya))
				++a;
			a += 0x40;
		}
		else
		{
			while (gTan[a] < (int16_t)((ya << 13) / -xa))
				++a;
			a = -0x80 - a;
		}
	}
	else if (ya <= 0)
	{
		if (-ya >= xa)
		{
			while (gTan[a] < (int16_t)((xa << 13) / -ya))
				++a;
			a -= 0x40;
		}
		else
		{
			while (gTan[a] < (int16_t)(-0x2000 * ya / xa))
				++a;
			a = -a;
		}
	}
	else if (xa <= ya)
	{
		while (gTan[a] < (int16_t)((xa << 13) / ya))
			++a;
		a = 0x40 - a;
	}
	else
	{
		while (gTan[a] < (int16_t)((ya << 13) / xa))
			++a;
	}
	
	return a;
}