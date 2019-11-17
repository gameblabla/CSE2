#include "Fade.h"

#include <string.h>

#include "CommonDefines.h"
#include "WindowsWrapper.h"

#include "Draw.h"

#define FADE_WIDTH	(((WINDOW_WIDTH - 1) / 16) + 1)
#define FADE_HEIGHT	(((WINDOW_HEIGHT - 1) / 16) + 1)

struct FADE
{
	int mode;
	BOOL bMask;
	int count;
	signed char ani_no[FADE_HEIGHT][FADE_WIDTH];
	signed char flag[FADE_HEIGHT][FADE_WIDTH];	// Not a BOOLEAN (those are unsigned)
	signed char dir;
};

static FADE gFade;
static unsigned long mask_color;

void InitFade(void)
{
	memset(&gFade, 0, sizeof(FADE));
	mask_color = GetCortBoxColor(RGB(0, 0, 0x20));
}

void SetFadeMask(void)
{
	gFade.bMask = TRUE;
}

void ClearFade(void)
{
	gFade.bMask = FALSE;
	gFade.mode = 0;
}

void StartFadeOut(signed char dir)
{
	int x;
	int y;

	gFade.mode = 2;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = FALSE;

	for (y = 0; y < FADE_HEIGHT; ++y)
	{
		for (x = 0; x < FADE_WIDTH; ++x)
		{
			gFade.ani_no[y][x] = 0;
			gFade.flag[y][x] = FALSE;
		}
	}
}

void StartFadeIn(signed char dir)
{
	int x;
	int y;

	gFade.mode = 1;
	gFade.count = 0;
	gFade.dir = dir;
	gFade.bMask = TRUE;

	for (y = 0; y < FADE_HEIGHT; ++y)
	{
		for (x = 0; x < FADE_WIDTH; ++x)
		{
			gFade.ani_no[y][x] = 15;
			gFade.flag[y][x] = FALSE;
		}
	}

	x = x;	// This probably doesn't match the original source code, but it produces the same assembly
}

void ProcFade(void)
{
	int x;
	int y;

	switch (gFade.mode)
	{
		case 2:
			switch (gFade.dir)
			{
				case 0:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if ((FADE_WIDTH - 1) - gFade.count == x)
								gFade.flag[y][x] = TRUE;

					break;

				case 2:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if (gFade.count == x)
								gFade.flag[y][x] = TRUE;

					break;

				case 1:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if ((FADE_HEIGHT - 1) - gFade.count == y)
								gFade.flag[y][x] = TRUE;

					break;

				case 3:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if (gFade.count == y)
								gFade.flag[y][x] = TRUE;

					break;

				case 4:
					for (y = 0; y < (FADE_HEIGHT / 2); ++y)
						for (x = 0; x < (FADE_WIDTH / 2); ++x)
							if (gFade.count == x + y)
								gFade.flag[y][x] = TRUE;

					for (y = 0; y < (FADE_HEIGHT / 2); ++y)
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
							if (gFade.count == y + ((FADE_WIDTH - 1) - x))
								gFade.flag[y][x] = TRUE;

					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
						for (x = 0; x < (FADE_WIDTH / 2); ++x)
							if (gFade.count == x + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y][x] = TRUE;

					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
							if (gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y][x] = TRUE;

					break;
			}

			for (y = 0; y < FADE_HEIGHT; ++y)
				for (x = 0; x < FADE_WIDTH; ++x)
					if (gFade.ani_no[y][x] < 15 && gFade.flag[y][x])
						++gFade.ani_no[y][x];

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
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if ((FADE_WIDTH - 1) - gFade.count == x)
								gFade.flag[y][x] = TRUE;

					break;

				case 2:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if (gFade.count == x)
								gFade.flag[y][x] = TRUE;

					break;

				case 1:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if ((FADE_HEIGHT - 1) - gFade.count == y)
								gFade.flag[y][x] = TRUE;

					break;

				case 3:
					for (y = 0; y < FADE_HEIGHT; ++y)
						for (x = 0; x < FADE_WIDTH; ++x)
							if (gFade.count == y)
								gFade.flag[y][x] = TRUE;

					break;

				case 4:
					for (y = 0; y < (FADE_HEIGHT / 2); ++y)
						for (x = 0; x < (FADE_WIDTH / 2); ++x)
							if ((FADE_WIDTH - 1) - gFade.count == x + y)
								gFade.flag[y][x] = TRUE;

					for (y = 0; y < (FADE_HEIGHT / 2); ++y)
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
							if ((FADE_WIDTH - 1) - gFade.count == y + ((FADE_WIDTH - 1) - x))
								gFade.flag[y][x] = TRUE;

					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
						for (x = 0; x < (FADE_WIDTH / 2); ++x)
							if ((FADE_WIDTH - 1) - gFade.count == x + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y][x] = TRUE;

					for (y = (FADE_HEIGHT / 2); y < FADE_HEIGHT; ++y)
						for (x = (FADE_WIDTH / 2); x < FADE_WIDTH; ++x)
							if ((FADE_WIDTH - 1) - gFade.count == ((FADE_WIDTH - 1) - x) + ((FADE_HEIGHT - 1) - y))
								gFade.flag[y][x] = TRUE;

					break;
			}

			for (y = 0; y < FADE_HEIGHT; ++y)
				for (x = 0; x < FADE_WIDTH; ++x)
					if (gFade.ani_no[y][x] > 0 && gFade.flag[y][x])
						--gFade.ani_no[y][x];

			if (++gFade.count > ((FADE_WIDTH > FADE_HEIGHT) ? FADE_WIDTH : FADE_HEIGHT) + 16)
				gFade.mode = 0;

			break;
	}
}

void PutFade(void)
{
	int x;
	int y;

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

	for (y = 0; y < FADE_HEIGHT; ++y)
	{
		for (x = 0; x < FADE_WIDTH; ++x)
		{
			rect.left = gFade.ani_no[y][x] * 16;
			rect.right = rect.left + 16;
			PutBitmap3(&grcGame, PixelToScreenCoord(x * 16), PixelToScreenCoord(y * 16), &rect, SURFACE_ID_FADE);
		}
	}
}

BOOL GetFadeActive(void)
{
	if (gFade.mode == 0)
		return FALSE;
	else
		return TRUE;
}
