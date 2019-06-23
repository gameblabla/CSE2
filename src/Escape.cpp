#include "Escape.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Main.h"
#include "MainLoop.h"

void Call_Escape(MainLoopMeta *meta)
{
	HWND hWnd = *(HWND*)meta->user_data;

	RECT rc = {0, 128, 208, 144};

	// Get pressed keys
	GetTrg();

	if (gKeyTrg & KEY_ESCAPE) // Escape is pressed, quit game
	{
		gKeyTrg = 0;
		ExitMainLoop(0);
		return;
	}
	if (gKeyTrg & KEY_F1) // F1 is pressed, continue
	{
		gKeyTrg = 0;
		ExitMainLoop(1);
		return;
	}
	if (gKeyTrg & KEY_F2) // F2 is pressed, reset
	{
		gKeyTrg = 0;
		ExitMainLoop(2);
		return;
	}

	// Draw screen
	CortBox(&grcFull, 0x000000);
	PutBitmap3(&grcFull, (WINDOW_WIDTH - 208) / 2, (WINDOW_HEIGHT - 16) / 2, &rc, SURFACE_ID_TEXT_BOX);
	PutFramePerSecound();

	if (!Flip_SystemTask(hWnd))
	{
		// Quit if window is closed
		gKeyTrg = 0;
		ExitMainLoop(0);
		return;
	}
}
