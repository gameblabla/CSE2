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
#include "MainLoop.h"
#include "MycParam.h"
#include "Organya.h"
#include "Stage.h"
#include "TextScr.h"
#include "Tags.h"

CREDIT Credit;
STRIP Strip[MAX_STRIP];
ILLUSTRATION Illust;

// Update casts
void ActionStripper()
{
	for (int s = 0; s < MAX_STRIP; s++)
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
void PutStripper()
{
	RECT rc;

	for (int s = 0; s < MAX_STRIP; s++)
	{
		if (Strip[s].flag & 0x80)
		{
			// Draw text
			rc.left = 0;
			rc.right = 320;
			rc.top = s * 0x10;
			rc.bottom = rc.top + 0x10;

			PutBitmap3(&grcFull, (Strip[s].x / 0x200) + ((WINDOW_WIDTH - 320) / 2), (Strip[s].y / 0x200), &rc, SURFACE_ID_CREDIT_CAST);

			// Draw character
			rc.left = 24 * (Strip[s].cast % 13);
			rc.right = rc.left + 24;
			rc.top = 24 * (Strip[s].cast / 13);
			rc.bottom = rc.top + 24;

			PutBitmap3(&grcFull, (Strip[s].x / 0x200) + ((WINDOW_WIDTH - 320) / 2) - 24, (Strip[s].y / 0x200) - 8, &rc, SURFACE_ID_CASTS);
		}
	}
}

// Create a cast object
void SetStripper(int x, int y, const char *text, int cast)
{
	RECT rc;
	int s;

	for (s = 0; s < MAX_STRIP; s++)
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
	rc.top = s * 0x10;
	rc.bottom = rc.top + 0x10;

	CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
	PutText2(0, rc.top, text, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_CREDIT_CAST);
}

// Regenerate cast text
void RestoreStripper()
{
	RECT rc;

	for (int s = 0; s < MAX_STRIP; s++)
	{
		if (Strip[s].flag & 0x80)
		{
			rc.left = 0;
			rc.right = 320;
			rc.top = s * 0x10;
			rc.bottom = rc.top + 0x10;

			CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
			PutText2(0, rc.top, Strip[s].str, RGB(0xFF, 0xFF, 0xFE), SURFACE_ID_CREDIT_CAST);
		}
	}
}

// Handle the illustration
void ActionIllust()
{
	switch (Illust.act_no)
	{
		case 0: // Off-screen to the left
			Illust.x = -0x14000;
			break;

		case 1: // Move in from the left
			Illust.x += 0x5000;
			if (Illust.x > 0)
				Illust.x = 0;
			break;

		case 2: // Move out from the right
			Illust.x -= 0x5000;
			if (Illust.x < -0x14000)
				Illust.x = -0x14000;
			break;
	}
}

// Draw illustration
void PutIllust()
{
	RECT rcIllust = {0, 0, 160, 240};
#if WINDOW_WIDTH != 320 || WINDOW_HEIGHT != 240
	// Widescreen edit
	RECT rcClip = {(WINDOW_WIDTH - 320) / 2, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	PutBitmap3(&rcClip, (Illust.x / 0x200) + ((WINDOW_WIDTH - 320) / 2), (WINDOW_HEIGHT - 240) / 2, &rcIllust, SURFACE_ID_CREDITS_IMAGE);
#else
	PutBitmap3(&grcFull, (Illust.x / 0x200) + ((WINDOW_WIDTH - 320) / 2), (WINDOW_HEIGHT - 240) / 2, &rcIllust, SURFACE_ID_CREDITS_IMAGE);
#endif
}

// Load illustration
void ReloadIllust(int a)
{
	char name[16];
	sprintf(name, "CREDIT%02d", a);
	ReloadBitmap_Resource(name, SURFACE_ID_CREDITS_IMAGE);
}

// Initialize and release credits
void InitCreditScript()
{
	// Clear script state and casts
	memset(&Credit, 0, sizeof(CREDIT));
	memset(Strip, 0, sizeof(Strip));
}

void ReleaseCreditScript()
{
	if (Credit.pData)
	{
		// Free script data
		free(Credit.pData);
		Credit.pData = NULL;
	}
}

const char *credit_script = "Credit.tsc";

// Start playing credits
BOOL StartCreditScript()
{
	// Clear previously existing credits data
	if (Credit.pData)
	{
		free(Credit.pData);
		Credit.pData = NULL;
	}

	// Open file
	char path[PATH_LENGTH];
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
	Illust.x = -0x14000;
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

// Get number from text (4 digit)
int GetScriptNumber(const char *text)
{
	return (text[0] - '0') * 1000 +
		(text[1] - '0') * 100 +
		(text[2] - '0') * 10 +
		text[3] - '0';
}

// Parse credits
void ActionCredit_Read()
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
				ChangeMusic(a);
				return;

			case '~': // Start fading out music
				++Credit.offset;
				SetOrganyaFadeout();
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

// Update credits
void ActionCredit()
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
	}
}

// Change illustration
void SetCreditIllust(int a)
{
	ReloadIllust(a);
	Illust.act_no = 1;
}

// Slide illustration off-screen
void CutCreditIllust()
{
	Illust.act_no = 2;
}

static void Scene_DownIslandReturn(MainLoopMeta *meta, int return_value)
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

// Scene of the island falling
void Scene_DownIsland(MainLoopMeta *meta)
{
	Scene_DownIsland_Data *data = (Scene_DownIsland_Data*)meta->user_data;

	// Setup background
	RECT rc_frame = {(WINDOW_WIDTH - 160) / 2, (WINDOW_HEIGHT - 80) / 2, (WINDOW_WIDTH + 160) / 2, (WINDOW_HEIGHT + 80) / 2};
	RECT rc_sky = {0, 0, 160, 80};
	RECT rc_ground = {160, 48, 320, 80};

	// Setup island
	RECT rc_sprite = {160, 0, 200, 24};

	static ISLAND_SPRITE sprite;
	static int wait;

	switch (meta->routine)
	{
		case 0:
			sprite.x = 0x15000;
			sprite.y = 0x8000;

			wait = 0;

			++meta->routine;
			// Fallthrough
		case 1:
			if (wait < 900)
			{
				// Get pressed keys
				GetTrg();

				// Escape menu
				if (gKey & 0x8000)
				{
					EnterMainLoop(Call_Escape, Scene_DownIslandReturn, &data->hWnd);
					return;
				}

				switch (data->mode)
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
				PutBitmap3(&rc_frame, 80 + (WINDOW_WIDTH - 320) / 2, 80 + (WINDOW_HEIGHT - 240) / 2, &rc_sky, SURFACE_ID_LEVEL_SPRITESET_1);
				PutBitmap3(&rc_frame, sprite.x / 0x200 - 20 + (WINDOW_WIDTH - 320) / 2, sprite.y / 512 - 12 + (WINDOW_HEIGHT - 240) / 2, &rc_sprite, SURFACE_ID_LEVEL_SPRITESET_1);
				PutBitmap3(&rc_frame, 80 + (WINDOW_WIDTH - 320) / 2, 128 + (WINDOW_HEIGHT - 240) / 2, &rc_ground, SURFACE_ID_LEVEL_SPRITESET_1);
				PutTimeCounter(16, 8);

				// Draw window
				PutFramePerSecound();
				if (!Flip_SystemTask(data->hWnd))
				{
					ExitMainLoop(0);
					return;
				}

				++wait;
			}
			else
			{
				ExitMainLoop(1);
				return;
			}

			break;
	}
}
