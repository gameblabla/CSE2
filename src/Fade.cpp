#include <string.h>

#include "WindowsWrapper.h"

#include "Fade.h"
#include "Game.h"
#include "Draw.h"

FADE gFade;

void InitFade()
{
	memset(&gFade, 0, sizeof(FADE));
}

void SetFadeMask()
{
	gFade.bMask = true;
}

void ClearFade()
{
	gFade.bMask = false;
	gFade.mode = 0;
}

void StartFadeOut(char dir)
{
	gFade.mode = 2;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = false;
	
	for (int y = 0; y < FADE_HEIGHT; y++)
	{
		for (int x = 0; x < FADE_WIDTH; x++)
		{
			gFade.ani_no[y][x] = 0;
			gFade.flag[y][x] = 0;
		}
	}
}

void StartFadeIn(char dir)
{
	gFade.mode = 1;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = true;
	
	for (int y = 0; y < FADE_HEIGHT; y++)
	{
		for (int x = 0; x < FADE_WIDTH; x++)
		{
			gFade.ani_no[y][x] = 15;
			gFade.flag[y][x] = 0;
		}
	}
}

void ProcFade()
{
	if (gFade.mode == 1)
	{
		gFade.bMask = false;
		
		switch (gFade.dir)
		{
			case 0:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x <= FADE_WIDTH; x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == x)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 1:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if ((FADE_HEIGHT - 1) - gFade.count == y)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 2:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if (gFade.count == x)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 3:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if (gFade.count == y)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 4:
				for (int y = 0; y < (FADE_HEIGHT / 2); y++)
				{
					for (int x = 0; x < (FADE_WIDTH / 2); x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == x + y)
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = 0; y < (FADE_HEIGHT / 2); y++)
				{
					for (int x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == y + ((FADE_WIDTH - 1) - x))
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < (FADE_WIDTH / 2); x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == x + ((FADE_HEIGHT - 1) - y))
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
				{
					for (int x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			default:
				break;
		}
		
		for (int y = 0; y < FADE_HEIGHT; y++)
		{
			for (int x = 0; x < FADE_WIDTH; x++)
			{
				if (gFade.ani_no[y][x] > 0 && gFade.flag[y][x])
					--gFade.ani_no[y][x];
			}
		}
		
		if (++gFade.count > ((FADE_WIDTH > FADE_HEIGHT) ? FADE_WIDTH : FADE_HEIGHT) + 16)
			gFade.mode = 0;
	}
	else if (gFade.mode == 2)
	{
		switch (gFade.dir)
		{
			case 0:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x <= FADE_WIDTH; x++)
					{
						if ((FADE_WIDTH - 1) - gFade.count == x)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 1:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if ((FADE_HEIGHT - 1) - gFade.count == y)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 2:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if (gFade.count == x)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 3:
				for (int y = 0; y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < FADE_WIDTH; x++)
					{
						if (gFade.count == y)
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			case 4:
				for (int y = 0; y < (FADE_HEIGHT / 2); y++)
				{
					for (int x = 0; x < (FADE_WIDTH / 2); x++)
					{
						if (gFade.count == x + y)
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = 0; y < (FADE_HEIGHT / 2); y++)
				{
					for (int x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
					{
						if (gFade.count == y + ((FADE_WIDTH - 1) - x))
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
				{
					for (int x = 0; x < (FADE_WIDTH / 2); x++)
					{
						if (gFade.count == x + ((FADE_HEIGHT - 1) - y))
							gFade.flag[y][x] = 1;
					}
				}
				for (int y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
				{
					for (int x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
					{
						if (gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
							gFade.flag[y][x] = 1;
					}
				}
				break;
				
			default:
				break;
		}
		
		for (int y = 0; y < FADE_HEIGHT; y++)
		{
			for (int x = 0; x < FADE_WIDTH; x++)
			{
				if (gFade.ani_no[y][x] < 15 && gFade.flag[y][x])
					++gFade.ani_no[y][x];
			}
		}
		
		if (++gFade.count > ((FADE_WIDTH > FADE_HEIGHT) ? FADE_WIDTH : FADE_HEIGHT) + 16)
		{
			gFade.bMask = true;
			gFade.mode = 0;
		}
	}
}

void PutFade()
{
	RECT rect;
	rect.top = 0;
	rect.bottom = 16;
	
	if (gFade.bMask)
	{
		CortBox(&grcGame, 0x000020);
	}
	else if (gFade.mode)
	{
		for (int y = 0; y < FADE_HEIGHT; y++)
		{
			for (int x = 0; x < FADE_WIDTH; x++)
			{
				rect.left = 16 * gFade.ani_no[y][x];
				rect.right = rect.left + 16;
				PutBitmap3(&grcGame, 16 * x, 16 * y, &rect, SURFACE_ID_FADE);
			}
		}
	}
}

bool GetFadeActive()
{
	return gFade.mode != 0;
}
