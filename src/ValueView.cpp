#include <string.h>

#include "WindowsWrapper.h"

#include "ValueView.h"
#include "Game.h"
#include "Draw.h"

#define VALUEVIEW_MAX 0x10
VALUEVIEW gVV[VALUEVIEW_MAX];
int gVVIndex;

void ClearValueView()
{
	memset(gVV, 0, sizeof(gVV));
	gVVIndex = 0;
}

void SetValueView(int *px, int *py, int value)
{
	int i;
	
	//TODO: Boi I'm reaaaally boutta getcho pickle chin aah boi, egg head like collard greens head ass boi, ol' hell dat bell dirt aaah boi stank ah boi afro head ass, lip gloss chin ah boi ugly ahhh boi *snort* uglaaaa
	for (i = 0; i < VALUEVIEW_MAX && (!gVV[i].flag || gVV[i].px != px || ((gVV[i].value >= 0 || value >= 0) && (gVV[i].value <= 0 || value <= 0))); i++);
	
	int index;
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
		gVV[i].count = 32;
		gVV[index].value += value;
		value = gVV[index].value;
	}
	
	//Get if negative or not
	bool minus;
	if ( value >= 0 )
	{
		minus = false;
	}
	else
	{
		value = -value;
		minus = true;
	}
	
	//Get width
	int v = value;
	int width;
	
	if (value < 1000)
	{
		if (value < 100)
		{
			if (value < 10)
				width = 16;
			else
				width = 24;
		}
		else
		{
			width = 32;
		}
	}
	else
	{
		width = 40;
	}
	
	//Set properties
	gVV[index].flag = 1;
	gVV[index].px = px;
	gVV[index].py = py;
	gVV[index].rect.left = 40 - width;
	gVV[index].rect.top = 8 * index;
	gVV[index].rect.right = 40;
	gVV[index].rect.bottom = 8 * (index + 1);
	
	//Get digits
	int dig[4];
	dig[0] = 1;
	dig[1] = 10;
	dig[2] = 100;
	dig[3] = 1000;
	
	int fig[4];
	for (int d = 3; d >= 0; d--)
	{
		fig[d] = 0;
		while (dig[d] <= v)
		{
			v -= dig[d];
			++fig[d];
		}
	}
	
	int sw = 0;
	
	RECT rcPlus = {32, 48, 40, 56};
	RECT rcMinus = {40, 48, 48, 56};
	
	//Draw value
	CortBox2(&gVV[index].rect, 0x000000, 29);
	
	if (minus)
		Surface2Surface(gVV[index].rect.left, gVV[index].rect.top, &rcMinus, 29, 26);
	else
		Surface2Surface(gVV[index].rect.left, gVV[index].rect.top, &rcPlus, 29, 26);
	
	for (int i = 3; i >= 0; i--)
	{
		if (sw || !i || fig[i])
		{
			sw = 1;
			
			RECT rect;
			if (minus)
				rect = {fig[i] << 3, 64, (fig[i] + 1) << 3, 72};
			else
				rect = {fig[i] << 3, 56, (fig[i] + 1) << 3, 64};
			
			Surface2Surface(8 * (4 - i), gVV[index].rect.top, &rect, 29, 26);
		}
	}
}

void ActValueView()
{
	for (int v = 0; v < VALUEVIEW_MAX; v++)
	{
		if (gVV[v].flag)
		{
			if (++gVV[v].count < 32)
				gVV[v].offset_y -= 0x100;

			if ( gVV[v].count > 72 )
				++gVV[v].rect.top;

			if ( gVV[v].count > 80 )
				gVV[v].flag = 0;
		}
	}
}

void PutValueView(int flx, int fly)
{
	for (int v = 0; v < VALUEVIEW_MAX; v++)
	{
		if (gVV[v].flag)
		{
			PutBitmap3(
				&grcGame,
				(*gVV[v].px) / 0x200 - (gVV[v].rect.right - gVV[v].rect.left) / 2 - flx / 0x200,
				(*gVV[v].py) / 0x200 + gVV[v].offset_y / 0x200 - 4 - fly / 0x200,
				&gVV[v].rect,
				29);
		}
	}
}
