#include "Triangle.h"

#include <math.h>

int gSin[0x100];
short gTan[0x21];

void InitTriangleTable()
{
	int i;
	float a;
	float b;

	//Sine
	for (i = 0; i < 0x100; ++i)
	{
		gSin[i] = (int)(sin(i * 6.2831998 / 256.0) * 512.0);
	}

	//Tangent
	for (i = 0; i < 0x21; ++i)
	{
		a = (float)(i * 6.2831855f / 256.0f);
		b = sinf(a) / cosf(a);
		gTan[i] = (short)(b * 8192.0f);
	}
}

int GetSin(unsigned char deg)
{
	return gSin[deg];
}

int GetCos(unsigned char deg)
{
	deg += 0x40;
	return gSin[deg];
}

unsigned char GetArktan(int x, int y)
{
	x *= -1;
	y *= -1;
	unsigned char a = 0;
	short k;

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
			if (x > -y)
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
