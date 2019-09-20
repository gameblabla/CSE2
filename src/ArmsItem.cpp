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

/// True if we're in the items section of the inventory (and not in the weapons section) (only relevant when the inventory is open)
static BOOL gCampActive;
static int gCampTitleY;

#define FIND_CODE_OR_0(iterator, array, arrayCount, codeVariable) \
	while ((iterator) < (arrayCount)) \
	{ \
		if ((array)[(iterator)].code == (codeVariable)) \
			break; /* Found identical */ \
\
		if ((array)[(iterator)].code == 0) \
			break; /* Found free slot */ \
\
		++(iterator); \
	}

#define FIND_CODE(iterator, array, arrayCount, codeVariable) \
	for ((iterator) = 0; (iterator) < (arrayCount); ++(iterator)) \
		if ((array)[(iterator)].code == (codeVariable)) \
			break; // Found

// Special version of FIND_CODE using while for accurate code
#define FIND_CODE_WHILE(iterator, array, arrayCount, codeVariable) \
	while ((iterator) < (arrayCount)) \
	{ \
		if ((array)[(iterator)].code == (codeVariable)) \
			break; /* Found */ \
		++(iterator); \
	} \

/*
	while (i < ARMS_MAX)
	{
		if (gArmsData[i].code == code1)
			break; // Found

		++i;
	} */

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
	int i = 0;
	FIND_CODE_OR_0(i, gArmsData, ARMS_MAX, code)

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
	// Find weapon index
	int i;
	FIND_CODE(i, gArmsData, ARMS_MAX, code)

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
	int i = 0;
	FIND_CODE_WHILE(i, gArmsData, ARMS_MAX, code1)

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
	int i = 0;
	FIND_CODE_OR_0(i, gItemData, ITEM_MAX, code)

	if (i == ITEM_MAX)
		return FALSE; // Not found

	gItemData[i].code = code;

	return TRUE;
}

BOOL SubItemData(long code)
{
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
static void MoveCampCursor()
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

	// True if we're currently changing inventory mode (weapons->items / items->weapons)
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
			if (item_num)
				gCampActive = TRUE;

			bChange = TRUE;
		}


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
				gSelectedItem = 6 * (gSelectedItem / 6);
			else if (gSelectedItem % 6 == 5)
				gSelectedItem -= 5;
			else
				++gSelectedItem;

			bChange = TRUE;
		}

		if (gKeyTrg & gKeyUp)
		{
			if (gSelectedItem / 6 == 0)
				gCampActive = FALSE;
			else
				gSelectedItem -= 6;

			bChange = TRUE;
		}

		if (gKeyTrg & gKeyDown)
		{
			if (gSelectedItem / 6 == (item_num - 1) / 6)
				gCampActive = FALSE;
			else
				gSelectedItem += 6;

			bChange = TRUE;
		}

		if (gSelectedItem >= item_num)
			gSelectedItem = item_num - 1;

		if (gCampActive && gKeyTrg & gKeyOk)
			StartTextScript(gItemData[gSelectedItem].code + 6000);
	}

	if (bChange)
	{
		if (gCampActive == FALSE)
		{
			// Weapons to items
			PlaySoundObject(SND_SWITCH_WEAPON, 1);

			if (arms_num)
				StartTextScript(gArmsData[gSelectedArms].code + 1000);
			else
				StartTextScript(1000);
		}
		else
		{
			// Items to weapons
#ifdef FIX_BUGS
			PlaySoundObject(SND_SWITCH_WEAPON, 1);
#else
			PlaySoundObject(SND_YES_NO_CHANGE_CHOICE, 1); // Pretty sure this is not intended
#endif

			if (item_num)
				StartTextScript(gItemData[gSelectedItem].code + 5000);
			else
				StartTextScript(5000);
		}
	}
}

/// Draw the inventory
static void PutCampObject()
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
			break; // Not a weapon

		// Get rect for current weapon
		rcArms.left = TILES_TO_PIXELS(gArmsData[i].code % 0x10);
		rcArms.right = rcArms.left + TILES_TO_PIXELS(1);
		rcArms.top = TILES_TO_PIXELS(PIXELS_TO_TILES(gArmsData[i].code));
		rcArms.bottom = rcArms.top + TILES_TO_PIXELS(1);

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
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 144) / 2, &rcNone, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&rcView, 40 * i + (WINDOW_WIDTH - 192) / 2, (WINDOW_HEIGHT - 128) / 2, &rcNone, SURFACE_ID_TEXT_BOX);
		}
	}

	// Draw items cursor
	if (gCampActive == TRUE)
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[(flash / 2) % 2],
		           SURFACE_ID_TEXT_BOX);
	else
		PutBitmap3(&rcView, 32 * (gSelectedItem % 6) + (WINDOW_WIDTH - 224) / 2, 16 * (gSelectedItem / 6) + (WINDOW_HEIGHT - 88) / 2, &rcCur2[1],
		           SURFACE_ID_TEXT_BOX);

	for (i = 0; i < ITEM_MAX; ++i)
	{
		if (gItemData[i].code == 0)
			break;

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

	// Load the inventory script
	GetTextScriptPath(old_script_path);

	LoadTextScript2("ArmsItem.tsc");

	gCampTitleY = (WINDOW_HEIGHT - 192) / 2;
	gCampActive = FALSE;
	gSelectedItem = 0;

	// Run script
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

		if (gKeyTrg & KEY_ESCAPE)
		{
			switch (Call_Escape(ghWnd))
			{
			case 0:
				return 0;
			case 2:
				return 2;
			}
		}

		if (g_GameFlags & 2)
			MoveCampCursor();

		switch (TextScriptProc())
		{
		case 0:
			return 0;
		case 2:
			return 2;
		}

		PutBitmap4(&rcView, 0, 0, &rcView, SURFACE_ID_SCREEN_GRAB);
		PutCampObject();
		PutTextScript();
		PutFramePerSecound();

		if (gCampActive)
		{
			if (g_GameFlags & 2 && gKeyTrg & (gKeyCancel | gKeyItem))
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
			return 0;
	}

	// Resume original script
	LoadTextScript_Stage(old_script_path);
	gArmsEnergyX = 0x20;
	return 1;
}

BOOL CheckItem(long a)
{
	for (int i = 0; i < ITEM_MAX; ++i)
		if (gItemData[i].code == a)
			return TRUE;

	return FALSE;
}

BOOL CheckArms(long a)
{
	for (int i = 0; i < ARMS_MAX; ++i)
		if (gArmsData[i].code == a)
			return TRUE;

	return FALSE;
}

BOOL UseArmsEnergy(long num)
{
	if (gArmsData[gSelectedArms].max_num == 0)
		return TRUE;
	if (gArmsData[gSelectedArms].num == 0)
		return FALSE;

	gArmsData[gSelectedArms].num -= num;

	if (gArmsData[gSelectedArms].num < 0)
		gArmsData[gSelectedArms].num = 0;

	return TRUE;
}

BOOL ChargeArmsEnergy(long num)
{
	gArmsData[gSelectedArms].num += num;

	if (gArmsData[gSelectedArms].num > gArmsData[gSelectedArms].max_num)
		gArmsData[gSelectedArms].num = gArmsData[gSelectedArms].max_num;

	return TRUE;
}

void FullArmsEnergy()
{
	for (int a = 0; a < ARMS_MAX; a++)
	{
		if (gArmsData[a].code == 0)
			continue;

		gArmsData[a].num = gArmsData[a].max_num;
	}
}

int RotationArms()
{
	int arms_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;

	if (arms_num == 0)
		return 0;

	ResetSpurCharge();

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
	PlaySoundObject(4, 1);

	return gArmsData[gSelectedArms].code;
}

int RotationArmsRev()
{
	int arms_num = 0;
	while (gArmsData[arms_num].code != 0)
		++arms_num;

	if (arms_num == 0)
		return 0;

	ResetSpurCharge();

	if (--gSelectedArms < 0)
		gSelectedArms = arms_num - 1;

	while (gSelectedArms < arms_num)
	{
		if (gArmsData[gSelectedArms].code)
			break;

		--gSelectedArms;
	}

	gArmsEnergyX = 0;
	PlaySoundObject(4, 1);

	return gArmsData[gSelectedArms].code;
}

void ChangeToFirstArms()
{
	gSelectedArms = 0;
	gArmsEnergyX = 0x20;
	PlaySoundObject(4, 1);
}
