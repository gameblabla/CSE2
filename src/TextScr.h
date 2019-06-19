#pragma once

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "MainLoop.h"

struct TEXT_SCRIPT
{
	//Path (reload when exit teleporter menu/inventory)
	char path[PATH_LENGTH];

	//Script buffer
	int size;
	char *data;

	//Mode (ex. NOD, WAI)
	signed char mode;

	//Flags
	signed char flags;

	//Current positions (read position in buffer, x position in line)
	unsigned int p_read;
	int p_write;

	//Current line to write to
	int line;

	//Line y positions
	int ypos_line[4];

	//Event stuff
	int wait;
	int wait_next;
	int next_event;

	//Yes/no selected
	signed char select;

	//Current face
	int face;
	int face_x;

	//Current item
	int item;
	int item_y;

	//Text rect
	RECT rcText;

	//..?
	int offsetY;

	//NOD cursor blink
	unsigned char wait_beam;
};

BOOL InitTextScript2();
void EndTextScript();
void EncryptionBinaryData2(unsigned char *pData, int size);
BOOL LoadTextScript2(const char *name);
BOOL LoadTextScript_Stage(const char *name);
void GetTextScriptPath(char *path);
BOOL StartTextScript(int no);
void StopTextScript();
void PutTextScript();
int TextScriptProc(void (*caller_return)(MainLoopMeta *meta, int return_value));
