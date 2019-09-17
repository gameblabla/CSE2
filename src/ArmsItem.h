#pragma once

#include "WindowsWrapper.h"

// "Arms" is a synonym of "weapon" here
// "Code" means "ID" here
// "Num" often means "ammo" here

/// Weapon struct
struct ARMS
{
	/// ID of the weapon
	int code;

	int level;
	int exp;

	/// Maximum ammunition
	int max_num;

	/// Current ammunition
	int num;
};

struct ITEM
{
	/// ID of the item
	int code;
};


// Limits for the amount of weapons and items
#define ARMS_MAX 8
#define ITEM_MAX 0x20


/// X coordinate for the weapons energy
extern int gArmsEnergyX;

extern int gSelectedArms;
extern int gSelectedItem;

extern ARMS gArmsData[ARMS_MAX];
extern ITEM gItemData[ITEM_MAX];


/// Clear the weapons array, reverting it to the default state (no weapons)
void ClearArmsData();

/// Clear the item array, reverting it to the default state (no items)
void ClearItemData();


/// Add code to the weapons, setting max_num as the max ammo, or find code and add max_num to its ammo. Fails if no space is available and the weapon isn't
/// already present
BOOL AddArmsData(long code, long max_num);

/// Remove code from the weapons. Fails if code is not found
BOOL SubArmsData(long code);

/// Replace code1 with code2, setting max_num as its max ammo. Fails if code1 is not found
BOOL TradeArms(long code1, long code2, long max_num);


/// Add code to the items. Fails if no space is left
BOOL AddItemData(long code);

/// Remove code from the items. Fails if code is not found
BOOL SubItemData(long code);


/// Inventory loop. Returns mode.
int CampLoop();


/// Search for a in the items. Returns whether a was found
BOOL CheckItem(long a);

/// Search for a in the weapons. Returns whether a was found
BOOL CheckArms(long a);


/// Remove num ammo from the currently selected weapon. Returns whether there was any ammo left to fire
BOOL UseArmsEnergy(long num);

/// Add num ammo to the currently selected weapon (capped at the maximum ammunition). Returns true
BOOL ChargeArmsEnergy(long num);

/// Set every weapons ammunition to its maximum ammunition
void FullArmsEnergy();


// "Rotation" means "Weapons currently owned by the player (present in the weapons array)"

/// Change the current weapon to the next one in the rotation. Returns the ID of the newly selected weapon
int RotationArms();

/// Change the current weapon to the previous one in the rotation. Returns the ID of the newly selected weapon
int RotationArmsRev();

/// Change the current weapon to be the first one and play the usual rotation animation
void ChangeToFirstArms();
