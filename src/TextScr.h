#pragma once

#include "WindowsWrapper.h"

struct TEXT_SCRIPT
{
	// Path (reload when exit teleporter menu/inventory)
	char path[MAX_PATH];

	// Script buffer
	long size;
	char *data;

	// Mode (ex. NOD, WAI)
	signed char mode;

	// Flags
	signed char flags;

	// Current positions (read position in buffer, x position in line)
	int p_read;
	int p_write;

	// Current line to write to
	int line;

	// Line y positions
	int ypos_line[4];

	// Event stuff
	int wait;
	int wait_next;
	int next_event;

	// Yes/no selected
	signed char select;

	// Current face
	int face;
	int face_x;

	// Current item
	int item;
	int item_y;

	// Text rect
	RECT rcText;

	// ..?
	int offsetY;

	// NOD cursor blink
	unsigned char wait_beam;
};

/// Contains the latest value given through <MIM
extern unsigned int gMIMCurrentNum;

BOOL InitTextScript2(void);
void EndTextScript(void);
void EncryptionBinaryData2(unsigned char *pData, long size);
BOOL LoadTextScript2(const char *name);
BOOL LoadTextScript_Stage(const char *name);
void GetTextScriptPath(char *path);
BOOL StartTextScript(int no);
void StopTextScript(void);
void PutTextScript(void);
int TextScriptProc(void);
void RestoreTextScript(void);


/// Array used by <PHY to contain the values of the different variables it may contain. The first 8 values contain the values for normal physics and the last 8 for when the character is underwater.
/// The values are, in order : max_dash, max_move, gravity1, gravity2, dash1, dash2, resist, jump
/// The seventeenth value determines whether the behaviours in which entering water causes a splash, being in water draws the air timer and being in water decreases your air are enabled (0 means it's enabled, non-0 means it's disabled)
extern int gPHYArray[17];
