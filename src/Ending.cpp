#include "Ending.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Draw.h"
#include "Escape.h"
#include "Flags.h"
#include "Generic.h"
#include "KeyControl.h"
#include "Main.h"
#include "MycParam.h"
#ifdef EXTRA_SOUND_FORMATS
#include "ExtraSoundFormats.h"
#endif
#include "Organya.h"
#include "Stage.h"
#include "TextScr.h"
#include "Tags.h"

CREDIT Credit;
STRIP Strip[MAX_STRIP];
ILLUSTRATION Illust;

// Update casts
void ActionStripper(void)
{
	int s;

	for (s = 0; s < MAX_STRIP; ++s)
	{
		// Move up
		if (Strip[s].flag & 0x80 && Credit.mode)
			Strip[s].y -= 0x100;
		// Get removed when off-screen
		if (Strip[s].y <= -0x2000)
			Strip[s].flag = 0;
	}
}

// Draw casts
void PutStripper(void)
{
	RECT rc;
	int s;

	for (s = 0; s < MAX_STRIP; ++s)
	{
		if (Strip[s].flag & 0x80)
		{
			// Draw text
			rc.left = 0;
			rc.right = 320;
			rc.top = s * 16;
			rc.bottom = rc.top + 16;

			PutBitmap3(&grcFull, SubpixelToScreenCoord(Strip[s].x) + PixelToScreenCoord((WINDOW_WIDTH - 320) / 2), SubpixelToScreenCoord(Strip[s].y), &rc, SURFACE_ID_CREDIT_CAST);

			// Draw character
			rc.left = (Strip[s].cast % 13) * 24;
			rc.right = rc.left + 24;
			rc.top = (Strip[s].cast / 13) * 24;
			rc.bottom = rc.top + 24;

			PutBitmap3(&grcFull, SubpixelToScreenCoord(Strip[s].x) + PixelToScreenCoord(((WINDOW_WIDTH - 320) / 2) - 24), SubpixelToScreenCoord(Strip[s].y) - PixelToScreenCoord(8), &rc, SURFACE_ID_CASTS);
		}
	}
}

// Create a cast object
void SetStripper(int x, int y, const char *text, int cast)
{
	RECT rc;
	int s;

	for (s = 0; s < MAX_STRIP; ++s)
		if (!(Strip[s].flag & 0x80))
			break;

	if (s == MAX_STRIP)
		return;

	// Initialize cast property
	Strip[s].flag = 0x80;
	Strip[s].x = x;
	Strip[s].y = y;
	Strip[s].cast = cast;
	strcpy(Strip[s].str, text);

	// Draw text
	rc.left = 0;
	rc.right = 320;
	rc.top = s * 16;
	rc.bottom = rc.top + 16;

	CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
	PutText2(0, rc.top, text, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_CREDIT_CAST);
}

// Regenerate cast text
void RestoreStripper(void)
{
	RECT rc;
	int s;

	for (s = 0; s < MAX_STRIP; ++s)
	{
		if (Strip[s].flag & 0x80)
		{
			rc.left = 0;
			rc.right = 320;
			rc.top = s * 16;
			rc.bottom = rc.top + 16;

			CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
			PutText2(0, rc.top, Strip[s].str, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_CREDIT_CAST);
		}
	}
}

// Handle the illustration
void ActionIllust(void)
{
	switch (Illust.act_no)
	{
		case 0: // Off-screen to the left
			Illust.x = -160 * 0x200;
			break;

		case 1: // Move in from the left
			Illust.x += 40 * 0x200;
			if (Illust.x > 0)
				Illust.x = 0;
			break;

		case 2: // Move out from the right
			Illust.x -= 0x5000;
			if (Illust.x < -160 * 0x200)
				Illust.x = -160 * 0x200;
			break;
	}
}

// Draw illustration
void PutIllust(void)
{
	RECT rcIllust = {0, 0, 160, 240};
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Widescreen edit
	RECT rcClip = {(WINDOW_WIDTH - 320) / 2, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	PutBitmap3(&rcClip, SubpixelToScreenCoord(Illust.x) + PixelToScreenCoord((WINDOW_WIDTH - 320) / 2), PixelToScreenCoord((WINDOW_HEIGHT - 240) / 2), &rcIllust, SURFACE_ID_CREDITS_IMAGE);
#else
	PutBitmap3(&grcFull, SubpixelToScreenCoord(Illust.x) + PixelToScreenCoord((WINDOW_WIDTH - 320) / 2), PixelToScreenCoord((WINDOW_HEIGHT - 240) / 2), &rcIllust, SURFACE_ID_CREDITS_IMAGE);
#endif
}

// Load illustration
void ReloadIllust(int a)
{
	char path[MAX_PATH];
	sprintf(path, "Resource/BITMAP/Credit%02d", a);
	ReloadBitmap_File(path, SURFACE_ID_CREDITS_IMAGE);
}

// Initialize and release credits
void InitCreditScript(void)
{
	// Clear script state and casts
	memset(&Credit, 0, sizeof(CREDIT));
	memset(Strip, 0, sizeof(Strip));
}

void ReleaseCreditScript(void)
{
	if (Credit.pData != NULL)
	{
		// Free script data
		free(Credit.pData);
		Credit.pData = NULL;
	}
}

const char *credit_script = "Credit.tsc";

// Start playing credits
BOOL StartCreditScript(void)
{
	// Clear previously existing credits data
	if (Credit.pData != NULL)
	{
		free(Credit.pData);
		Credit.pData = NULL;
	}

	// Open file
	char path[MAX_PATH];
	sprintf(path, "%s/%s", gDataPath, credit_script);

	Credit.size = GetFileSizeLong(path);
	if (Credit.size == -1)
		return FALSE;

	// Allocate buffer data
	Credit.pData = (char*)malloc(Credit.size);
	if (Credit.pData == NULL)
		return FALSE;

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
	{
		free(Credit.pData);
		return FALSE;
	}

	// Read data
	fread(Credit.pData, 1, Credit.size, fp);
	EncryptionBinaryData2((unsigned char*)Credit.pData, Credit.size);

#ifdef FIX_BUGS
	// The original game forgot to close the file
	fclose(fp);
#endif

	// Reset credits
	Credit.offset = 0;
	Credit.wait = 0;
	Credit.mode = 1;
	Illust.x = -160 * 0x200;
	Illust.act_no = 0;

	// Modify cliprect
	grcGame.left = WINDOW_WIDTH / 2;
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// These three are non-vanilla: for wide/tallscreen support
	grcGame.right = ((WINDOW_WIDTH - 320) / 2) + 320;
	grcGame.top = (WINDOW_HEIGHT - 240) / 2;
	grcGame.bottom = ((WINDOW_HEIGHT - 240) / 2) + 240;
#endif

	// Reload casts
	if (!ReloadBitmap_File("casts", SURFACE_ID_CASTS))
		return FALSE;

	// Clear casts
	memset(Strip, 0, sizeof(Strip));
	return TRUE;
}

// Update credits
void ActionCredit(void)
{
	if (Credit.offset >= Credit.size)
		return;

	// Update script, or if waiting, decrement the wait value
	switch (Credit.mode)
	{
		case 1:
			ActionCredit_Read();
			break;

		case 2:
			if (--Credit.wait <= 0)
				Credit.mode = 1;
			break;
	}
}

// Parse credits
void ActionCredit_Read(void)
{
	int a, b, len;
	char text[40];

	while (1)
	{
		if (Credit.offset >= Credit.size)
			break;

		switch (Credit.pData[Credit.offset])
		{
			case '[': // Create cast
				// Get the range for the cast text
				++Credit.offset;

				a = Credit.offset;

				while (Credit.pData[a] != ']')
				{
					if (IsShiftJIS(Credit.pData[a]))
						a += 2;
					else
						a += 1;
				}

				len = a - Credit.offset;

				// Copy the text to the cast text
				memcpy(text, &Credit.pData[Credit.offset], len);
				text[len] = 0;

				// Get cast id
				Credit.offset = a;
				len = GetScriptNumber(&Credit.pData[++Credit.offset]);

				// Create cast object
				SetStripper(Credit.start_x, (WINDOW_HEIGHT * 0x200) + (8 * 0x200), text, len);

				// Change offset
				Credit.offset += 4;
				return;

			case '-': // Wait for X amount of frames
				++Credit.offset;
				Credit.wait = GetScriptNumber(&Credit.pData[Credit.offset]);
				Credit.offset += 4;
				Credit.mode = 2;
				return;

			case '+': // Change casts x-position
				++Credit.offset;
				Credit.start_x = GetScriptNumber(&Credit.pData[Credit.offset]) * 0x200;
				Credit.offset += 4;
				return;

			case '/': // Stop credits
				Credit.mode = 0;
				return;

			case '!': // Change music
				++Credit.offset;
				a = GetScriptNumber(&Credit.pData[Credit.offset]);
				Credit.offset += 4;
				ChangeMusic((MusicID)a);
				return;

			case '~': // Start fading out music
				++Credit.offset;
				SetOrganyaFadeout();
				#ifdef EXTRA_SOUND_FORMATS
				ExtraSound_FadeOutMusic();
				#endif
				return;

			case 'j': // Jump to label
				++Credit.offset;

				// Get number
				b = GetScriptNumber(&Credit.pData[Credit.offset]);

				// Change offset
				Credit.offset += 4;

				// Jump to specific label
				if (1)
				{
					while (Credit.offset < Credit.size)
					{
						if (Credit.pData[Credit.offset] == 'l')
						{
							// What is this
							a = GetScriptNumber(&Credit.pData[++Credit.offset]);
							Credit.offset += 4;
							if (b == a)
								break;
						}
						else if (IsShiftJIS(Credit.pData[Credit.offset]))
						{
							Credit.offset += 2;
						}
						else
						{
							++Credit.offset;
						}
					}
				}

				return;

			case 'f': // Flag jump
				++Credit.offset;

				// Read numbers XXXX:YYYY
				a = GetScriptNumber(&Credit.pData[Credit.offset]);
				Credit.offset += 5;
				b = GetScriptNumber(&Credit.pData[Credit.offset]);
				Credit.offset += 4;

				// If flag is set
				if (GetNPCFlag(a))
				{
					// Jump to label
					while (Credit.offset < Credit.size)
					{
						if (Credit.pData[Credit.offset] == 'l')
						{
							a = GetScriptNumber(&Credit.pData[++Credit.offset]);
							Credit.offset += 4;
							if (b == a)
								break;
						}
						else if (IsShiftJIS(Credit.pData[Credit.offset]))
						{
							Credit.offset += 2;
						}
						else
						{
							++Credit.offset;
						}
					}
				}
				return;

			default:
				// Progress through file
				++Credit.offset;
				break;
		}
	}
}

// Get number from text (4 digit)
int GetScriptNumber(const char *text)
{
	return (text[0] - '0') * 1000 +
		(text[1] - '0') * 100 +
		(text[2] - '0') * 10 +
		text[3] - '0';
}


// Change illustration
void SetCreditIllust(int a)
{
	ReloadIllust(a);
	Illust.act_no = 1;
}

// Slide illustration off-screen
void CutCreditIllust(void)
{
	Illust.act_no = 2;
}

// Scene of the island falling
int Scene_DownIsland(int mode)
{
	int wait;

	// Setup background
	RECT rc_frame = {(WINDOW_WIDTH / 2) - 80, (WINDOW_HEIGHT / 2) - 40, (WINDOW_WIDTH / 2) + 80, (WINDOW_HEIGHT / 2) + 40};
	RECT rc_sky = {0, 0, 160, 80};
	RECT rc_ground = {160, 48, 320, 80};

	// Setup island
	RECT rc_sprite = {160, 0, 200, 24};

	ISLAND_SPRITE sprite;
	sprite.x = 168 * 0x200;
	sprite.y = 64 * 0x200;

	for (wait = 0; wait < 900; ++wait)
	{
		// Get pressed keys
		GetTrg();

		// Escape menu
		if (gKey & 0x8000)
		{
			switch (Call_Escape())
			{
				case 0:
					return 0;
				case 2:
					return 2;
			}
		}

		switch (mode)
		{
			case 0:
				// Move down
				sprite.y += 0x33;
				break;

			case 1:
				if (wait < 350)
				{
					// Move down at normal speed
					sprite.y += 0x33;
				}
				else if (wait < 500)
				{
					// Move down slower
					sprite.y += 0x19;
				}
				else if (wait < 600)
				{
					// Move down slow
					sprite.y += 0xC;
				}
				else if (wait == 750)
				{
					// End scene
					wait = 900;
				}

				break;
		}

		// Draw scene
		CortBox(&grcFull, 0);
		PutBitmap3(&rc_frame, PixelToScreenCoord(80 + ((WINDOW_WIDTH - 320) / 2)), PixelToScreenCoord(80 + ((WINDOW_HEIGHT - 240) / 2)), &rc_sky, SURFACE_ID_LEVEL_SPRITESET_1);
		PutBitmap3(&rc_frame, SubpixelToScreenCoord(sprite.x) - PixelToScreenCoord(20) + PixelToScreenCoord((WINDOW_WIDTH - 320) / 2), SubpixelToScreenCoord(sprite.y) - PixelToScreenCoord(12) + PixelToScreenCoord((WINDOW_HEIGHT - 240) / 2), &rc_sprite, SURFACE_ID_LEVEL_SPRITESET_1);
		PutBitmap3(&rc_frame, PixelToScreenCoord(80 + ((WINDOW_WIDTH - 320) / 2)), PixelToScreenCoord(128 + ((WINDOW_HEIGHT - 240) / 2)), &rc_ground, SURFACE_ID_LEVEL_SPRITESET_1);
		PutTimeCounter(16, 8);

		// Draw window
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}

	return 1;
}
