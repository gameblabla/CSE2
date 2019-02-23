#include "Triangle.h"

#include <stdint.h>
#include <math.h>

int gSin[0x100];
int16_t gTan[0x21];

void InitTriangleTable()
{
	int i;

	//Sine
	for (i = 0; i < 0x100; ++i)
	{
		gSin[i] = (int)(sin(i * 6.2831998 / 256.0) * 512.0);
	}
	
	//Tangent
	for (i = 0; i < 0x21; ++i)
	{
		float a = (float)(i * 6.2831855 / 256.0);
		float b = sinf(a) / cosf(a);
		gTan[i] = (int16_t)(b * 8192.0);
	}
}

int GetSin(uint8_t deg)
{
	return gSin[deg];
}

int GetCos(uint8_t deg)
{
	deg += 0x40;
	return gSin[deg];
}

uint8_t GetArktan(int x, int y)
{
	x *= -1;
	y *= -1;
	uint8_t a = 0;
	int16_t k;

	if (x > 0)
	{
		if (y > 0)
		{
			if (x > y)
			{
				k = (y * 0x2000) / x;
				while (k > gTan[a])
					++a;
			}
			else
			{
				k = (x * 0x2000) / y;
				while (k > gTan[a])
					++a;
				a = 0x40 - a;
			}
		}
		else
		{
			if (-y < x)
			{
				k = (-y * 0x2000) / x;
				while (k > gTan[a])
					++a;
				a = 0x100 - a;
			}
			else
			{
				k = (x * 0x2000) / -y;
				while (k > gTan[a])
					++a;
				a = 0x100 - 0x40 + a;
			}
		}
	}
	else
	{
		if (y > 0)
		{
			if (-x > y)
			{
				k = (y * 0x2000) / -x;
				while (k > gTan[a])
					++a;
				a = 0x80 - a;
			}
			else
			{
				k = (-x * 0x2000) / y;
				while (k > gTan[a])
					++a;
				a = 0x40 + a;
			}
		}
		else
		{
			if (-x > -y)
			{
				k = (-y * 0x2000) / -x;
				while (k > gTan[a])
					++a;
				a = 0x80 + a;
			}
			else
			{
				k = (-x * 0x2000) / -y;
				while (k > gTan[a])
					++a;
				a = 0x100 - 0x40 - a;
			}
		}
	}
	
	return a;
}