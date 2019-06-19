#include "MiniMap.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Escape.h"
#include "KeyControl.h"
#include "Main.h"
#include "MainLoop.h"
#include "Map.h"
#include "MapName.h"
#include "MyChar.h"
#include "Stage.h"

char gMapping[0x80];

void WriteMiniMapLine(int line)
{
	int x;
	unsigned char a;

	RECT rcLevel[4] = {
		{240, 24, 241, 25},
		{241, 24, 242, 25},
		{242, 24, 243, 25},
		{243, 24, 244, 25},
	};

	for (x = 0; x < gMap.width; x++)
	{
		a = GetAttribute(x, line);

		// Yup. This really is an if/else chain.
		// No switch here.
		if (a == 0)
			Surface2Surface(x, line, &rcLevel[0], 9, 26);
		else if (a == 68 ||
			a == 1 ||
			a == 64 ||
			a == 128 ||
			a == 129 ||
			a == 130 ||
			a == 131 ||
			a == 81 ||
			a == 82 ||
			a == 85 ||
			a == 86 ||
			a == 2 ||
			a == 96 ||
			a == 113 ||
			a == 114 ||
			a == 117 ||
			a == 118 ||
			a == 160 ||
			a == 161 ||
			a == 162 ||
			a == 163)
			Surface2Surface(x, line, &rcLevel[1], 9, 26);
		else if (a == 67 ||
			a == 99 ||
			a == 80 ||
			a == 83 ||
			a == 84 ||
			a == 87 ||
			a == 96 ||	// This is already listed above
			a == 112 ||
			a == 115 ||
			a == 116 ||
			a == 119)
			Surface2Surface(x, line, &rcLevel[2], 9, 26);
		else
			Surface2Surface(x, line, &rcLevel[3], 9, 26);
	}
}

static void MiniMapLoopReturn(MainLoopMeta *meta, int return_value)
{
	(void)meta;

	switch (return_value)
	{
		case 0:
			ExitMainLoop(0);
			return;

		case 2:
			ExitMainLoop(2);
			return;
	}
}

void MiniMapLoop(MainLoopMeta *meta)
{
	static int f;
	static int line;
	static unsigned char my_wait;

	static RECT rcMiniMap;
	static RECT rcView;

	RECT my_rect = {0, 57, 1, 58};
	int my_x = (gMC.x / 0x200 + 8) / 16;
	int my_y = (gMC.y / 0x200 + 8) / 16;

	switch (meta->routine)
	{
		case 0:
			f = 0;
			++meta->routine;
			// Fallthrough
		case 1:
			if (f <= 8)
			{
				GetTrg();

				if (gKey & KEY_ESCAPE)
				{
					EnterMainLoop(Call_Escape, MiniMapLoopReturn, &ghWnd);
					return;
				}

				PutBitmap4(&grcGame, 0, 0, &grcGame, SURFACE_ID_SCREEN_GRAB);

				rcView.left = (WINDOW_WIDTH / 2) - gMap.width * f / 8 / 2;
				rcView.right = (WINDOW_WIDTH / 2) + gMap.width * f / 8 / 2;
				rcView.top = (WINDOW_HEIGHT / 2) - gMap.length * f / 8 / 2;
				rcView.bottom = (WINDOW_HEIGHT / 2) + gMap.length * f / 8 / 2;

				PutMapName(TRUE);
				CortBox(&rcView, 0);

				PutFramePerSecound();
				if (!Flip_SystemTask(ghWnd))
				{
					ExitMainLoop(0);
					return;
				}

				++f;

				break;
			}

			++meta->routine;
			// Fallthrough
		case 2:
			rcMiniMap.left = 0;
			rcMiniMap.right = gMap.width;
			rcMiniMap.top = 0;
			rcMiniMap.bottom = gMap.length;

			rcView.right = --rcView.left + gMap.width + 2;
			rcView.bottom = --rcView.top + gMap.length + 2;
			CortBox2(&rcMiniMap, 0, SURFACE_ID_MAP);

			line = 0;
			my_wait = 0;

			++meta->routine;
			// Fallthrough
		case 3:
			GetTrg();

			if (!((gKeyCancel | gKeyOk) & gKeyTrg))
			{
				if (gKey & KEY_ESCAPE)
				{
					EnterMainLoop(Call_Escape, MiniMapLoopReturn, &ghWnd);
					return;
				}

				PutBitmap4(&grcGame, 0, 0, &grcGame, SURFACE_ID_SCREEN_GRAB);
				CortBox(&rcView, 0);

				if (line < gMap.length)
				{
					WriteMiniMapLine(line);
					line++;
				}
				if (line < gMap.length)
				{
					WriteMiniMapLine(line);
					line++;
				}

				PutBitmap3(&grcGame, rcView.left + 1, rcView.top + 1, &rcMiniMap, SURFACE_ID_MAP);

				PutMapName(TRUE);

				if (++my_wait / 8 % 2)
					PutBitmap3(&grcGame, my_x + rcView.left + 1, my_y + rcView.top + 1, &my_rect, SURFACE_ID_TEXT_BOX);

				PutFramePerSecound();
				if (!Flip_SystemTask(ghWnd))
				{
					ExitMainLoop(0);
					return;
				}

				break;
			}

			f = 8;

			++meta->routine;
			// Fallthrough
		case 4:
			if (f >= -1)
			{
				GetTrg();

				if (gKey & KEY_ESCAPE)
				{
					EnterMainLoop(Call_Escape, MiniMapLoopReturn, &ghWnd);
					return;
				}

				PutBitmap4(&grcGame, 0, 0, &grcGame, SURFACE_ID_SCREEN_GRAB);

				rcView.left = (WINDOW_WIDTH / 2) - gMap.width * f / 8 / 2;
				rcView.right = (WINDOW_WIDTH / 2) + gMap.width * f / 8 / 2;
				rcView.top = (WINDOW_HEIGHT / 2) - gMap.length * f / 8 / 2;
				rcView.bottom = (WINDOW_HEIGHT / 2) + gMap.length * f / 8 / 2;

				PutMapName(TRUE);
				CortBox(&rcView, 0);

				PutFramePerSecound();
				if (!Flip_SystemTask(ghWnd))
				{
					ExitMainLoop(0);
					return;
				}

				--f;

				break;
			}

			ExitMainLoop(1);
			return;
	}
}

BOOL IsMapping()
{
	if (gMapping[gStageNo] == FALSE)
		return FALSE;
	else
		return TRUE;
}

void StartMapping()
{
	memset(gMapping, FALSE, 0x80);
}

void SetMapping(int a)
{
	gMapping[a] = TRUE;
}
