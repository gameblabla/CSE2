#include "Fade.h"

#include <string.h>
#include <stdlib.h>

#include "CommonDefines.h"
#include "WindowsWrapper.h"

#include "Draw.h"
#include "Game.h"

#define FADE_WIDTH	(((WINDOW_WIDTH - 1) / 16) + 1)
#define FADE_HEIGHT	(((WINDOW_HEIGHT - 1) / 16) + 1)

struct FADE
{
	int mode;
	BOOL bMask;
	int count;
	char *ani_no; //char ani_no[FADE_HEIGHT][FADE_WIDTH];
	char *flag; //char flag[FADE_HEIGHT][FADE_WIDTH];	// Not a BOOLEAN (those are unsigned)
	char dir;
};

static FADE gFade;
static unsigned long mask_color;

void InitFade()
{
	free(gFade.ani_no);
	free(gFade.flag);
	
	memset(&gFade, 0, sizeof(FADE));
	
	gFade.ani_no = (char*)malloc(FADE_WIDTH * FADE_HEIGHT * sizeof(char));
	gFade.flag = (char*)malloc(FADE_WIDTH * FADE_HEIGHT * sizeof(char));
	
	mask_color = GetCortBoxColor(RGB(0, 0, 0x20));
}

void SetFadeMask()
{
	gFade.bMask = TRUE;
}

void ClearFade()
{
	gFade.bMask = FALSE;
	gFade.mode = 0;
}

void StartFadeOut(char dir)
{
	gFade.mode = 2;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = FALSE;
	
	for (int y = 0; y < FADE_HEIGHT; y++)
	{
		for (int x = 0; x < FADE_WIDTH; x++)
		{
			gFade.ani_no[y * FADE_WIDTH + x] = 0;
			gFade.flag[y * FADE_WIDTH + x] = FALSE;
		}
	}
}

void StartFadeIn(char dir)
{
	int x;
	int y;

	gFade.mode = 1;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = TRUE;
	
	for (y = 0; y < FADE_HEIGHT; y++)
	{
		for (x = 0; x < FADE_WIDTH; x++)
		{
			gFade.ani_no[y * FADE_WIDTH + x] = 15;
			gFade.flag[y * FADE_WIDTH + x] = FALSE;
		}
	}

	x = x;	// What
}

void ProcFade()
{
	int x;
	int y;

	switch (gFade.mode)
	{
		case 2:
			switch (gFade.dir)
			{
				case 0:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == x)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 2:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if (gFade.count == x)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 1:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if ((FADE_HEIGHT - 1) - gFade.count == y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 3:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if (gFade.count == y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 4:
					for (y = 0; y < (FADE_HEIGHT / 2); y++)
					{
						for (x = 0; x < (FADE_WIDTH / 2); x++)
						{
							if (gFade.count == x + y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = 0; y < (FADE_HEIGHT / 2); y++)
					{
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
						{
							if (gFade.count == y + ((FADE_WIDTH - 1) - x))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < (FADE_WIDTH / 2); x++)
						{
							if (gFade.count == x + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
					{
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
						{
							if (gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
			}
			
			for (y = 0; y < FADE_HEIGHT; y++)
			{
				for (x = 0; x < FADE_WIDTH; x++)
				{
					if (gFade.ani_no[y * FADE_WIDTH + x] < 15 && gFade.flag[y * FADE_WIDTH + x])
						++gFade.ani_no[y * FADE_WIDTH + x];
				}
			}
			
			if (++gFade.count > ((FADE_WIDTH > FADE_HEIGHT) ? FADE_WIDTH : FADE_HEIGHT) + 16)
			{
				gFade.bMask = TRUE;
				gFade.mode = 0;
			}

			break;

		case 1:
			gFade.bMask = FALSE;
			
			switch (gFade.dir)
			{
				case 0:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == x)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 2:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if (gFade.count == x)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 1:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if ((FADE_HEIGHT - 1) - gFade.count == y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 3:
					for (y = 0; y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < FADE_WIDTH; x++)
						{
							if (gFade.count == y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
					
				case 4:
					for (y = 0; y < (FADE_HEIGHT / 2); y++)
					{
						for (x = 0; x < (FADE_WIDTH / 2); x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == x + y)
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = 0; y < (FADE_HEIGHT / 2); y++)
					{
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == y + ((FADE_WIDTH - 1) - x))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
					{
						for (x = 0; x < (FADE_WIDTH / 2); x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == x + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; y++)
					{
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; x++)
						{
							if ((FADE_WIDTH - 1) - gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y * FADE_WIDTH + x] = TRUE;
						}
					}
					break;
			}
			
			for (y = 0; y < FADE_HEIGHT; y++)
			{
				for (x = 0; x < FADE_WIDTH; x++)
				{
					if (gFade.ani_no[y * FADE_WIDTH + x] > 0 && gFade.flag[y * FADE_WIDTH + x])
						--gFade.ani_no[y * FADE_WIDTH + x];
				}
			}
			
			if (++gFade.count > ((FADE_WIDTH > FADE_HEIGHT) ? FADE_WIDTH : FADE_HEIGHT) + 16)
				gFade.mode = 0;

			break;
	}
}

void PutFade()
{
	RECT rect;
	rect.top = 0;
	rect.bottom = 16;
	
	if (gFade.bMask)
	{
		CortBox(&grcGame, mask_color);
		return;
	}

	if (gFade.mode == 0)
		return;

	for (int y = 0; y < FADE_HEIGHT; y++)
	{
		for (int x = 0; x < FADE_WIDTH; x++)
		{
			rect.left = 16 * gFade.ani_no[y * FADE_WIDTH + x];
			rect.right = rect.left + 16;
			PutBitmap3(&grcGame, 16 * x, 16 * y, &rect, SURFACE_ID_FADE);
		}
	}
}

BOOL GetFadeActive()
{
	if (gFade.mode == 0)
		return FALSE;
	else
		return TRUE;
}
