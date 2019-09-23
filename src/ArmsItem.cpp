#include "ArmsItem.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Escape.h"
#include "Game.h"
#include "KeyControl.h"
#include "Main.h"
#include "Shoot.h"
#include "Sound.h"
#include "TextScr.h"

int gArmsEnergyX = 0x10;

int gSelectedArms;
int gSelectedItem;

ARMS gArmsData[ARMS_MAX];
ITEM gItemData[ITEM_MAX];

/// True if we're in the items section of the inventory (not in the weapons section) (only relevant when the inventory is open)
static BOOL gCampActive;
static int gCampTitleY;

void ClearArmsData()
{
#ifdef FIX_BUGS
	gSelectedArms = 0; // Should probably be done in order to avoid potential problems with the selected weapon being invalid (like is done in SubArmsData)
#endif
	gArmsEnergyX = 0x20;
	memset(gArmsData, 0, sizeof(gArmsData));
}

void ClearItemData()
{
	memset(gItemData, 0, sizeof(gItemData));
}

BOOL AddArmsData(long code, long max_num)
{
	// Search for code
	int i = 0;
	while (i < ARMS_MAX)
	{
		if (gArmsData[i].code == code)
			break; // Found identical

		if (gArmsData[i].code == 0)
			break; // Found free slot

		++i;
	}

	if (i == ARMS_MAX)
		return FALSE; // No space left

	if (gArmsData[i].code == 0)
	{
		// Initialize new weapon
		memset(&gArmsData[i], 0, sizeof(ARMS));
		gArmsData[i].level = 1;
	}

	// Set weapon and ammo
	gArmsData[i].code = code;
	gArmsData[i].max_num += max_num;
	gArmsData[i].num += max_num;

	// Cap the amount of current ammo to the maximum amount of ammo
	if (gArmsData[i].num > gArmsData[i].max_num)
		gArmsData[i].num = gArmsData[i].max_num;

	return TRUE;
}

BOOL SubArmsData(long code)
{
	// Search for code
	int i;
	for (i = 0; i < ARMS_MAX; ++i)
		if (gArmsData[i].code == code)
			break; // Found

#ifdef FIX_BUGS
	if (i == ARMS_MAX)
#else
	if (i == ITEM_MAX) // Wrong
#endif
		return FALSE; // Not found

	// Shift all arms from the right to the left
	for (++i; i < ARMS_MAX; ++i)
		gArmsData[i - 1] = gArmsData[i];

	// Clear farthest weapon and select first
	gArmsData[i - 1].code = 0;
	gSelectedArms = 0;

	return TRUE;
}

BOOL TradeArms(long code1, long code2, long max_num)
{
	// Search for code1
	int i = 0;
	while (i < ARMS_MAX)
	{
		if (gArmsData[i].code == code1)
			break; // Found identical

		++i;
	}

	if (i == ARMS_MAX)
		return FALSE; // Not found

	// Initialize new weapon replacing old one, but adding the maximum ammunition to that of the old weapon.
	gArmsData[i].level = 1;
	gArmsData[i].code = code2;
	gArmsData[i].max_num += max_num;
	gArmsData[i].num += max_num;
	gArmsData[i].exp = 0;

	return TRUE;
}

BOOL AddItemData(long code)
{
	// Search for code
	int i = 0;
	while (i < ITEM_MAX)
	{
		if (gItemData[i].code == code)
			break; // Found identical

		if (gItemData[i].code == 0)
			break; // Found free slot

		++i;
	}

	if (i == ITEM_MAX)
		return FALSE; // Not found

	gItemData[i].code = code;

	return TRUE;
}

BOOL SubItemData(long code)
{
	// Search for code
	int i;
	for (i = 0; i < ITEM_MAX; ++i)
		if (gItemData[i].code == code)
			break; // Found

	if (i == ITEM_MAX)
		return FALSE; // Not found

	// Shift all items from the right to the left
	for (++i; i < ITEM_MAX; ++i)
		gItemData[i - 1] = gItemData[i];

	gItemData[i - 1].code = 0;
	gSelectedItem = 0;

	return TRUE;
}

/// Update the inventory cursor
void MoveCampCursor()
{
	// Compute the current amount of weapons and items
	int arms_num = 0;
	int item_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;
	while (gItemData[item_num].code != 0)
		++item_num;

	if (arms_num == 0 && item_num == 0)
		return;	// Empty inventory

	/// True if we're currently changing cursor position
	BOOL bChange = FALSE;

	if (gCampActive == FALSE)
	{
		// Handle selected weapon
		if (gKeyTrg & gKeyLeft)
		{
			--gSelectedArms;
			bChange = TRUE;
		}

		if (gKeyTrg & gKeyRight)
		{
			++gSelectedArms;
			bChange = TRUE;
		}

		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			// If there are any items, we're changing to the items section, since the weapons section has only 1 row
			if (item_num)
				gCampActive = TRUE;

			bChange = TRUE;
		}

		// Loop around gSelectedArms if needed
		if (gSelectedArms < 0)
			gSelectedArms = arms_num - 1;

		if (gSelectedArms > arms_num - 1)
			gSelectedArms = 0;
	}
	else
	{
		// Handle selected item
		if (gKeyTrg & gKeyLeft)
		{
			if (gSelectedItem % 6 == 0)
				gSelectedItem += 5;
			else
				--gSelectedItem;

			bChange = TRUE;
		}

		if (gKeyTrg & gKeyRight)
		{
			if (gSelectedItem == item_num - 1)
				gSelectedItem = 6 * (gSelectedItem / 6); // Round down to multiple of 6
			else if (gSelectedItem % 6 == 5)
				gSelectedItem -= 5; // Loop around row
			else
				++gSelectedItem;

			bChange = TRUE;
		}

		if (gKeyTrg & gKeyUp)
		{
			if (gSelectedItem / 6 == 0)
				gCampActive = FALSE; // We're on the first row, transition to weapons
			else
				gSelectedItem -= 6;

			bChange = TRUE;
		}

		if (gKeyTrg & gKeyDown)
		{
			if (gSelectedItem / 6 == (item_num - 1) / 6)
				gCampActive = FALSE; // We're on the last row, transition to weapons
			else
				gSelectedItem += 6;

			bChange = TRUE;
		}

		if (gSelectedItem >= item_num)
			gSelectedItem = item_num - 1; // Don't allow selecting a non-existing item

		if (gCampActive && gKeyTrg & gKeyOk)
			StartTextScript(gItemData[gSelectedItem].code + 6000);
	}

	if (bChange)
	{
		if (gCampActive == FALSE)
		{
			// Switch to a weapon
			PlaySoundObject(SND_SWITCH_WEAPON, 1);

			if (arms_num)
				StartTextScript(gArmsData[gSelectedArms].code + 1000);
			else
				StartTextScript(1000);
		}
		else
		{
			// Switch to an item
			PlaySoundObject(SND_YES_NO_CHANGE_CHOICE, 1);

			if (item_num)
				StartTextScript(gItemData[gSelectedItem].code + 5000);
			else
				StartTextScript(5000);
		}
	}
}

/// Draw the inventory
void PutCampObject()
{
	int i;

	/// Rect for the current weapon
	RECT rcArms;

	/// Rect for the current item
	RECT rcItem;

	/// Probably the rect for the slash
	RECT rcPer = {72, 48, 80, 56};

	/// Rect for when there is no ammo (double dashes)
	RECT rcNone = {80, 48, 96, 56};

	/// Rect for the "Lv" text!
	RECT rcLv = {80, 80, 96, 88};

	/// Final rect drawn on the screen
	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	/// Cursor rect array for weapons, element [1] being for when the cursor is flashing
	RECT rcCur1[2] = {{0, 88, 40, 128}, {40, 88, 80, 128}};

	/// Cursor rect array for items, element [1] being for when the cursor is flashing
	RECT rcCur2[2] = {{80, 88, 112, 104}, {80, 104, 112, 120}};

	RECT rcTitle1 = {80, 48, 144, 56};
	RECT rcTitle2 = {80, 56, 144, 64};
	RECT rcBoxTop = {0, 0, 244, 8};
	RECT rcBoxBody = {0, 8, 244, 16};
	RECT rcBoxBottom = {0, 16, 244, 24};

	// Draw box
	PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, (WINDOW_HEIGHT - 224) / 2, &rcBoxTop, SURFACE_ID_TEXT_BOX);
	for (i = 1; i < 18; ++i)
		PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, ((WINDOW_HEIGHT - 240) / 2) + (8 * (i + 1)), &rcBoxBody, SURFACE_ID_TEXT_BOX);
	PutBitmap3(&rcView, (WINDOW_WIDTH - 244) / 2, ((WINDOW_HEIGHT - 240) / 2) + (8 * (i + 1)), &rcBoxBottom, SURFACE_ID_TEXT_BOX);

	// Move titles
	if (gCampTitleY > (WINDOW_HEIGHT - 208) / 2)
		--gCampTitleY;

	// Draw titles
	PutBitmap3(&rcView, (WINDOW_WIDTH - 224) / 2, gCampTitleY, &rcTitle1, SURFACE_ID_TEXT_BOX);
	PutBitmap3(&rcView, (WINDOW_WIDTH - 224) / 2, gCampTitleY + 52, &rcTitle2, SURFACE_ID_TEXT_BOX);

	// Draw arms cursor
	static unsigned int flash;
	++flash;

	if (gCampActive == FALSE)
		PutBitmap3(&rcView, 40 * gSelectedArms + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT / 2) - 96, &rcCur1[(flash / 2) % 2], SURFACE_ID_TEXT_BOX);
	else
		PutBitmap3(&rcView, 40 * gSelectedArms + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT / 2) - 96, &rcCur1[1], SURFACE_ID_TEXT_BOX);

	// Draw weapons
	for (i = 0; i < ARMS_MAX; ++i)
	{
		if (gArmsData[i].code == 0)
			break; // Invalid weapon

		// Get icon rect for next weapon
		rcArms.left = (gArmsData[i].code % 16) * 16;
		rcArms.right = rcArms.left + 16;
		rcArms.top = ((gArmsData[i].code) / 16) * 16;
		rcArms.bottom = rcArms.top + 16;

		// Draw the icon, slash and "Lv"
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 192) / 2, &rcArms, SURFACE_ID_ARMS_IMAGE);
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 128) / 2, &rcPer, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 160) / 2, &rcLv, SURFACE_ID_TEXT_BOX);
		PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 160) / 2, gArmsData[i].level, FALSE);

		// Draw ammo
		if (gArmsData[i].max_num)
		{
			PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 144) / 2, gArmsData[i].num, FALSE);
			PutNumber4(40 * i + (WINDOW_WIDTH - 224) / 2, (WINDOW_HEIGHT - 128) / 2, gArmsData[i].max_num, FALSE);
		}
		else
		{
			// Weapon doesn't use ammunition
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 144) / 2, &rcNone, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 128) / 2, &rcNone, SURFACE_ID_TEXT_BOX);
		}
	}

	// Draw items cursor
	if (gCampActive == TRUE)
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[(flash / 2) % 2], SURFACE_ID_TEXT_BOX);
	else
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[1], SURFACE_ID_TEXT_BOX);

	for (i = 0; i < ITEM_MAX; ++i)
	{
		if (gItemData[i].code == 0)
			break; // Invalid item

		// Get rect for next item
		rcItem.left = 32 * (gItemData[i].code % 8);
		rcItem.right = rcItem.left + 32;
		rcItem.top = 16 * (gItemData[i].code / 8);
		rcItem.bottom = rcItem.top + 16;

		PutBitmap3(&rcView, 32 * (i % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (i / 6) + (WINDOW_HEIGHT - 88) / 2, &rcItem, SURFACE_ID_ITEM_IMAGE);
	}
}

int CampLoop()
{
	int arms_num;
	char old_script_path[MAX_PATH];

	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	// Save the current script path (to restore it when we get out of the inventory)
	GetTextScriptPath(old_script_path);

	// Load the inventory script
	LoadTextScript2("ArmsItem.tsc");

	gCampTitleY = (WINDOW_HEIGHT - 192) / 2;

	// Put the cursor on the first weapon
	gCampActive = FALSE;
	gSelectedItem = 0;

	// Compute current amount of weapons
	arms_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;

	if (arms_num)
		StartTextScript(gArmsData[gSelectedArms].code + 1000);
	else
		StartTextScript(gItemData[gSelectedItem].code + 5000);

	for (;;)
	{
		GetTrg();

		// Handle ESC
		if (gKeyTrg & KEY_ESCAPE)
		{
			switch (Call_Escape(ghWnd))
			{
				case 0:
					return 0; // Quit game
				case 2:
					return 2; // Go to game intro
			}
		}

		if (g_GameFlags & GAME_FLAG_IS_CONTROL_ENABLED)
			MoveCampCursor();

		switch (TextScriptProc())
		{
			case 0:
				return 0; // Quit game
			case 2:
				return 2; // Go to game intro
		}

		// Get currently displayed image
		PutBitmap4(&rcView, 0, 0, &rcView, SURFACE_ID_SCREEN_GRAB);
		PutCampObject();
		PutTextScript();
		PutFramePerSecound();

		// Check whether we're getting out of the loop
		if (gCampActive)
		{
			if (g_GameFlags & GAME_FLAG_IS_CONTROL_ENABLED && gKeyTrg & (gKeyCancel | gKeyItem))
			{
				StopTextScript();
				break;
			}
		}
		else
		{
			if (gKeyTrg & (gKeyOk | gKeyCancel | gKeyItem))
			{
				StopTextScript();
				break;
			}
		}

		if (!Flip_SystemTask(ghWnd))
			return 0; // Quit game
	}

	// Resume original script
	LoadTextScript_Stage(old_script_path);
	gArmsEnergyX = 0x20; // ?
	return 1; // Go to game
}

BOOL CheckItem(long a)
{
	for (int i = 0; i < ITEM_MAX; ++i)
		if (gItemData[i].code == a)
			return TRUE; // Found

	return FALSE; // Not found
}

BOOL CheckArms(long a)
{
	for (int i = 0; i < ARMS_MAX; ++i)
		if (gArmsData[i].code == a)
			return TRUE; // Found

	return FALSE; // Not found
}

BOOL UseArmsEnergy(long num)
{
	if (gArmsData[gSelectedArms].max_num == 0)
		return TRUE; // No ammo needed
	if (gArmsData[gSelectedArms].num == 0)
		return FALSE; // No ammo left

	gArmsData[gSelectedArms].num -= num;

	if (gArmsData[gSelectedArms].num < 0)
		gArmsData[gSelectedArms].num = 0;

	return TRUE; // Was able to spend ammo
}

BOOL ChargeArmsEnergy(long num)
{
	gArmsData[gSelectedArms].num += num;

	// Cap the ammo to the maximum ammunition
	if (gArmsData[gSelectedArms].num > gArmsData[gSelectedArms].max_num)
		gArmsData[gSelectedArms].num = gArmsData[gSelectedArms].max_num;

	return TRUE; // Always successfull
}

void FullArmsEnergy()
{
	for (int a = 0; a < ARMS_MAX; a++)
	{
		if (gArmsData[a].code == 0)
			continue; // Don't change empty weapons

		gArmsData[a].num = gArmsData[a].max_num;
	}
}

int RotationArms()
{
	// Get amount of weapons
	int arms_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;

	if (arms_num == 0)
		return 0;

	ResetSpurCharge();

	// Select next valid weapon
	++gSelectedArms;

	while (gSelectedArms < arms_num)
	{
		if (gArmsData[gSelectedArms].code)
			break;

		++gSelectedArms;
	}

	if (gSelectedArms == arms_num)
		gSelectedArms = 0;

	gArmsEnergyX = 0x20;
	PlaySoundObject(SND_SWITCH_WEAPON, 1);

	return gArmsData[gSelectedArms].code;
}

int RotationArmsRev()
{
	// Get amount of weapons
	int arms_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;

	if (arms_num == 0)
		return 0;

	ResetSpurCharge();

	// Select previous valid weapon
	if (--gSelectedArms < 0)
		gSelectedArms = arms_num - 1;

	while (gSelectedArms < arms_num)
	{
		if (gArmsData[gSelectedArms].code)
			break;

		--gSelectedArms;
	}

	gArmsEnergyX = 0;
	PlaySoundObject(SND_SWITCH_WEAPON, 1);

	return gArmsData[gSelectedArms].code;
}

void ChangeToFirstArms()
{
	gSelectedArms = 0;
	gArmsEnergyX = 0x20;
	PlaySoundObject(SND_SWITCH_WEAPON, 1);
}
