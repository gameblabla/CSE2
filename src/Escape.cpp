#include "Escape.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "KeyControl.h"
#include "Main.h"

int Call_Escape(void)
{
	RECT rc = {0, 128, 208, 144};

	while (1)
	{
		// Get pressed keys
		GetTrg();

		if (gKeyTrg & KEY_ESCAPE) // Escape is pressed, quit game
		{
			gKeyTrg = 0;
			return 0;
		}
		if (gKeyTrg & KEY_F1) // F1 is pressed, continue
		{
			gKeyTrg = 0;
			return 1;
		}
		if (gKeyTrg & KEY_F2) // F2 is pressed, reset
		{
			gKeyTrg = 0;
			return 2;
		}

		// Draw screen
		CortBox(&grcFull, 0x000000);
		PutBitmap3(&grcFull, PixelToScreenCoord((WINDOW_WIDTH / 2) - 104), PixelToScreenCoord((WINDOW_HEIGHT / 2) - 8), &rc, SURFACE_ID_TEXT_BOX);
		PutFramePerSecound();

		if (!Flip_SystemTask())
		{
			// Quit if window is closed
			gKeyTrg = 0;
			return 0;
		}
	}

	return 0;
}
