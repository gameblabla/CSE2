#include "ValueView.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "Draw.h"

#define VALUEVIEW_MAX 0x10

VALUEVIEW gVV[VALUEVIEW_MAX];
int gVVIndex;

void ClearValueView(void)
{
	memset(gVV, 0, sizeof(gVV));
	gVVIndex = 0;
}

void SetValueView(int *px, int *py, int value)
{
	int index;
	BOOL minus;
	int v;
	int width;
	int dig[4];
	int fig[4];
	BOOL sw;
	int i;
	RECT rect[20];
	RECT rcPlus;
	RECT rcMinus;

	for (i = 0; i < VALUEVIEW_MAX; ++i)
	{
		if (gVV[i].flag && gVV[i].px == px)
		{
			if (gVV[i].value < 0 && value < 0)
				break;

			if (gVV[i].value > 0 && value > 0)
				break;
		}
	}

	if (i == VALUEVIEW_MAX)
	{
		index = gVVIndex++;

		if (gVVIndex == VALUEVIEW_MAX)
			gVVIndex = 0;

		gVV[index].count = 0;
		gVV[index].offset_y = 0;
		gVV[index].value = value;
	}
	else
	{
		index = i;

		gVV[index].count = 32;
		gVV[index].value += value;
		value = gVV[index].value;
	}

	// Get if negative or not
	if (value < 0)
	{
		value *= -1;
		minus = TRUE;
	}
	else
	{
		minus = FALSE;
	}

	// Get width
	v = value;

	if (value > 999)
		width = 40;
	else if (value > 99)
		width = 32;
	else if (value > 9)
		width = 24;
	else
		width = 16;

	// Set properties
	gVV[index].flag = TRUE;
	gVV[index].px = px;
	gVV[index].py = py;
	gVV[index].rect.left = 40 - width;
	gVV[index].rect.top = 8 * index;
	gVV[index].rect.right = 40;
	gVV[index].rect.bottom = 8 * (index + 1);

	SET_RECT(rect[0], 0, 56, 8, 64);
	SET_RECT(rect[1], 8, 56, 16, 64);
	SET_RECT(rect[2], 16, 56, 24, 64);
	SET_RECT(rect[3], 24, 56, 32, 64);
	SET_RECT(rect[4], 32, 56, 40, 64);
	SET_RECT(rect[5], 40, 56, 48, 64);
	SET_RECT(rect[6], 48, 56, 56, 64);
	SET_RECT(rect[7], 56, 56, 64, 64);
	SET_RECT(rect[8], 64, 56, 72, 64);
	SET_RECT(rect[9], 72, 56, 80, 64);
	SET_RECT(rect[10], 0, 64, 8, 72);
	SET_RECT(rect[11], 8, 64, 16, 72);
	SET_RECT(rect[12], 16, 64, 24, 72);
	SET_RECT(rect[13], 24, 64, 32, 72);
	SET_RECT(rect[14], 32, 64, 40, 72);
	SET_RECT(rect[15], 40, 64, 48, 72);
	SET_RECT(rect[16], 48, 64, 56, 72);
	SET_RECT(rect[17], 56, 64, 64, 72);
	SET_RECT(rect[18], 64, 64, 72, 72);
	SET_RECT(rect[19], 72, 64, 80, 72);

	// Get digits
	dig[0] = 1;
	dig[1] = 10;
	dig[2] = 100;
	dig[3] = 1000;

	for (i = 3; i >= 0; --i)
	{
		fig[i] = 0;

		while (v >= dig[i])
		{
			v -= dig[i];
			++fig[i];
		}
	}

	sw = FALSE;

	SET_RECT(rcPlus, 32, 48, 40, 56);
	SET_RECT(rcMinus, 40, 48, 48, 56);

	// Draw value
	CortBox2(&gVV[index].rect, 0x000000, SURFACE_ID_VALUE_VIEW);

	if (minus)
		Surface2Surface(gVV[index].rect.left, gVV[index].rect.top, &rcMinus, SURFACE_ID_VALUE_VIEW, SURFACE_ID_TEXT_BOX);
	else
		Surface2Surface(gVV[index].rect.left, gVV[index].rect.top, &rcPlus, SURFACE_ID_VALUE_VIEW, SURFACE_ID_TEXT_BOX);

	for (i = 3; i >= 0; i--)
	{
		if (!sw && i != 0 && fig[i] == 0)
			continue;

		sw = TRUE;

		if (minus)
			fig[i] += 10;

		Surface2Surface(((3 - i) * 8) + 8, gVV[index].rect.top, &rect[fig[i]], SURFACE_ID_VALUE_VIEW, SURFACE_ID_TEXT_BOX);
	}
}

void ActValueView(void)
{
	int v;

	for (v = 0; v < VALUEVIEW_MAX; ++v)
	{
		if (gVV[v].flag == FALSE)
			continue;

		if (++gVV[v].count < 32)
			gVV[v].offset_y -= 0x100;

		if (gVV[v].count > 72)
			++gVV[v].rect.top;

		if (gVV[v].count > 80)
			gVV[v].flag = FALSE;
	}
}

void PutValueView(int flx, int fly)
{
	int v;
	int offset_x;

	for (v = 0; v < VALUEVIEW_MAX; ++v)
	{
		if (gVV[v].flag == FALSE)
			continue;

		offset_x = (gVV[v].rect.right - gVV[v].rect.left) / 2;

		PutBitmap3(
			&grcGame,
			(*gVV[v].px / 0x200) - offset_x - (flx / 0x200),
			(*gVV[v].py / 0x200) + (gVV[v].offset_y / 0x200) - 4 - (fly / 0x200),
			&gVV[v].rect,
			SURFACE_ID_VALUE_VIEW);
	}
}
