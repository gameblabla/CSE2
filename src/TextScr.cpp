#include "TextScr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "Boss.h"
#include "BossLife.h"
#include "CommonDefines.h"
#include "Draw.h"
#include "Ending.h"
#include "Fade.h"
#include "Flags.h"
#include "Flash.h"
#include "Frame.h"
#include "Game.h"
#include "Generic.h"
#include "KeyControl.h"
#include "Main.h"
#include "Map.h"
#include "MapName.h"
#include "MiniMap.h"
#include "MyChar.h"
#include "MycParam.h"
#include "NpChar.h"
#include "Organya.h"
#include "Profile.h"
#include "SelStage.h"
#include "Sound.h"
#include "Stage.h"
#include "Tags.h"

#define IS_COMMAND(c1, c2, c3) gTS.data[gTS.p_read + 1] == c1 && gTS.data[gTS.p_read + 2] == c2 && gTS.data[gTS.p_read + 3] == c3

#define TSC_BUFFER_SIZE 0x5000

#define TEXT_LEFT (WINDOW_WIDTH / 2 - 108)

TEXT_SCRIPT gTS;

int gNumberTextScript[4];
char text[0x100];

RECT gRect_line = {0, 0, 216, 16};

#ifdef FIX_BUGS
static unsigned long nod_color;
#endif

//Initialize and end tsc
BOOL InitTextScript2()
{
#ifdef FIX_BUGS
	nod_color = GetCortBoxColor(RGB(0xFF, 0xFF, 0xFE));
#endif

	//Clear flags
	gTS.mode = 0;
	g_GameFlags &= ~0x04;

	//Create line surfaces
	for (int i = 0; i < 4; i++)
		MakeSurface_Generic(gRect_line.right, gRect_line.bottom, (Surface_Ids)(i + SURFACE_ID_TEXT_LINE1), FALSE);

	//Clear text
	memset(text, 0, sizeof(text));

	//Allocate script buffer
	gTS.data = (char*)malloc(TSC_BUFFER_SIZE);

	if (gTS.data == NULL)
		return FALSE;
	else
		return TRUE;
}

void EndTextScript()
{
	//Free TSC buffer
	free(gTS.data);

	//Release buffers
	ReleaseSurface(SURFACE_ID_TEXT_BOX);
	for (int i = 0; i < 4; i++)
		ReleaseSurface(i + 30);
}

//Decrypt .tsc
void EncryptionBinaryData2(unsigned char *pData, int size)
{
	int val1;
	int work;
	int i;

	int half = size / 2;
	if (pData[half] == 0)
		val1 = -7;
	else
		val1 = (pData[half] % 256) * -1;

	for (i = 0; i < size; i++)
	{
		work = pData[i];
		work += val1;

		if (i != half)
			pData[i] = work % 256;
	}
}

//Load generic .tsc
BOOL LoadTextScript2(const char *name)
{
	//Get path
	char path[260];
	sprintf(path, "%s/%s", gDataPath, name);

	gTS.size = GetFileSizeLong(path);
	if (gTS.size == -1)
		return FALSE;

	//Open file
	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	//Read data
	fread(gTS.data, 1, gTS.size, fp);
	gTS.data[gTS.size] = 0;
	fclose(fp);

	//Set path
	strcpy(gTS.path, name);

	//Decrypt data
	EncryptionBinaryData2((unsigned char*)gTS.data, gTS.size);
	return TRUE;
}

//Load stage .tsc
BOOL LoadTextScript_Stage(const char *name)
{
	//Open Head.tsc
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, "Head.tsc");

	long head_size = GetFileSizeLong(path);
	if (head_size == -1)
		return FALSE;

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	//Read Head.tsc
	fread(gTS.data, 1, head_size, fp);
	EncryptionBinaryData2((unsigned char*)gTS.data, head_size);
	gTS.data[head_size] = 0;
	fclose(fp);

	//Open stage's .tsc
	sprintf(path, "%s/%s", gDataPath, name);

	long body_size = GetFileSizeLong(path);
	if (body_size == -1)
		return FALSE;

	fp = fopen(path, "rb");
	if (fp == NULL)
		return FALSE;

	//Read stage's tsc
	fread(&gTS.data[head_size], 1, body_size, fp);
	EncryptionBinaryData2((unsigned char*)&gTS.data[head_size], body_size);
	gTS.data[head_size + body_size] = 0;
	fclose(fp);

	//Set parameters
	gTS.size = head_size + body_size;
	strcpy(gTS.path, name);
	return TRUE;
}

//Get current path
void GetTextScriptPath(char *path)
{
	strcpy(path, gTS.path);
}

//Get 4 digit number from TSC data
int GetTextScriptNo(int a)
{
	int b = 0;
	b += (gTS.data[a++] - '0') * 1000;
	b += (gTS.data[a++] - '0') * 100;
	b += (gTS.data[a++] - '0') * 10;
	b += gTS.data[a] - '0';
	return b;
}

//Start TSC event
BOOL StartTextScript(int no)
{
	//Reset state
	gTS.mode = 1;
	g_GameFlags |= 5;
	gTS.line = 0;
	gTS.p_write = 0;
	gTS.wait = 4;
	gTS.flags = 0;
	gTS.wait_beam = 0;
	gTS.face = 0;
	gTS.item = 0;
	gTS.offsetY = 0;

	gMC.shock = 0;

	gTS.rcText.left = TEXT_LEFT;
	gTS.rcText.top = WINDOW_HEIGHT - 56;
	gTS.rcText.right = WINDOW_WIDTH - TEXT_LEFT;
	gTS.rcText.bottom = gTS.rcText.top + 48;

	/* This is present in the Linux port, but not the Windows version (1.0.0.6, at least)
	//Clear text lines
	for (int i = 0; i < 4; i++)
	{
		gTS.ypos_line[i] = 16 * i;
		CortBox2(&gRect_line, 0x000000, (Surface_Ids)(i + SURFACE_ID_TEXT_LINE1));
		memset(&text[i * 0x40], 0, 0x40);
	}*/

	//Find where event starts
	gTS.p_read = 0;
	while (1)
	{
		//Check if we are still in the proper range
		if (gTS.data[gTS.p_read] == '\0')
			return FALSE;

		//Check if we are at an event
		if (gTS.data[gTS.p_read] == '#')
		{
			//Check if this is our event
			int event_no = GetTextScriptNo(++gTS.p_read);

			if (no == event_no)
				break;
			if (no < event_no)
				return FALSE;
		}

		++gTS.p_read;
	}

	//Advance until new-line
	while (gTS.data[gTS.p_read] != '\n')
		++gTS.p_read;
	++gTS.p_read;

	return TRUE;
}

BOOL JumpTextScript(int no)
{
	//Set state
	gTS.mode = 1;
	g_GameFlags |= 4u;
	gTS.line = 0;
	gTS.p_write = 0;
	gTS.wait = 4;
	gTS.wait_beam = 0;

	//Clear text lines
	for (int i = 0; i < 4; i++)
	{
		gTS.ypos_line[i] = 16 * i;
		CortBox2(&gRect_line, 0x000000, (Surface_Ids)(i + SURFACE_ID_TEXT_LINE1));
		memset(&text[i * 0x40], 0, 0x40);
	}

	//Find where event starts
	gTS.p_read = 0;
	while(1)
	{
		//Check if we are still in the proper range
		if (gTS.data[gTS.p_read] == '\0')
			return FALSE;

		//Check if we are at an event
		if (gTS.data[gTS.p_read] == '#')
		{
			//Check if this is our event
			int event_no = GetTextScriptNo(++gTS.p_read);

			if (no == event_no)
				break;
			if (no < event_no)
				return FALSE;
		}

		++gTS.p_read;
	}

	//Advance until new-line
	while (gTS.data[gTS.p_read] != '\n')
		++gTS.p_read;
	++gTS.p_read;

	return TRUE;
}

//End event
void StopTextScript()
{
	//End TSC and reset flags
	gTS.mode = 0;
	g_GameFlags &= ~4;
	g_GameFlags |= 3;
	gTS.flags = 0;
}

//Prepare a new line
void CheckNewLine()
{
	if (gTS.ypos_line[gTS.line % 4] == 48)
	{
		gTS.mode = 3;
		g_GameFlags |= 4;
		CortBox2(&gRect_line, 0, (Surface_Ids)(gTS.line % 4 + SURFACE_ID_TEXT_LINE1));
		memset(&text[gTS.line % 4 * 0x40], 0, 0x40);
	}
}

//Type a number into the text buffer
void SetNumberTextScript(int index)
{
	int a;
	int b;
	int i;
	BOOL bZero;
	int offset;
	char str[5];

	//Get digit table
	int table[3];
	table[0] = 1000;
	table[1] = 100;
	table[2] = 10;

	//Get number to print
	a = gNumberTextScript[index];

	bZero = FALSE;
	offset = 0;

	//Trim leading zeroes
	for (i = 0; i < 3; i++)
	{
		if (a / table[i] || bZero != FALSE)
		{
			b = a / table[i];
			str[offset] = (char)b + '0';
			bZero = TRUE;
			a -= b * table[i];
			++offset;
		}
	}

	//Set last digit of string, and add null terminator
	str[offset] = (char)a + '0';
	str[offset + 1] = 0;

	//Append number to line
	PutText2(6 * gTS.p_write, 0, str, RGB(0xFF, 0xFF, 0xFE), (Surface_Ids)(gTS.line % 4 + SURFACE_ID_TEXT_LINE1));
	strcat(&text[gTS.line % 4 * 0x40], str);

	//Play sound and reset blinking cursor
	PlaySoundObject(2, 1);
	gTS.wait_beam = 0;

	//Check if should move to next line (prevent a memory overflow, come on guys, this isn't a leftover of pixel trying to make text wrapping)
	gTS.p_write += (unsigned int)strlen(str);

	if (gTS.p_write >= 35)
	{
		gTS.p_write = 0;
		gTS.line++;
		CheckNewLine();
	}
}

//Clear text lines
void ClearTextLine()
{
	gTS.line = 0;
	gTS.p_write = 0;
	gTS.offsetY = 0;

	for (int i = 0; i < 4; i++)
	{
		gTS.ypos_line[i] = 16 * i;
		CortBox2(&gRect_line, 0x000000, (Surface_Ids)(i + SURFACE_ID_TEXT_LINE1));
		memset(&text[i * 0x40], 0, 0x40);
	}
}

//Draw textbox and whatever else
void PutTextScript()
{
	RECT rcFace;
	RECT rcItemBox1;
	RECT rcItemBox2;
	RECT rcItemBox3;
	RECT rcItemBox4;
	RECT rcItemBox5;
	int i;
	RECT rect_yesno;
	RECT rect_cur;
	RECT rect;
	int text_offset;

	if (gTS.mode == 0)
		return;

	if ((gTS.flags & 1) == 0)
		return;

	//Set textbox position
	if (gTS.flags & 0x20)
	{
		gTS.rcText.top = 32;
		gTS.rcText.bottom = gTS.rcText.top + 48;
	}
	else
	{
		gTS.rcText.top = WINDOW_HEIGHT - 56;
		gTS.rcText.bottom = gTS.rcText.top + 48;
	}

	//Draw textbox
	if (gTS.flags & 2)
	{
		RECT rcFrame1 = {0, 0, 244, 8};
		RECT rcFrame2 = {0, 8, 244, 16};
		RECT rcFrame3 = {0, 16, 244, 24};

		PutBitmap3(&grcFull, WINDOW_WIDTH / 2 - 122, gTS.rcText.top - 10, &rcFrame1, SURFACE_ID_TEXT_BOX);
		for (i = 1; i < 7; i++)
			PutBitmap3(&grcFull, WINDOW_WIDTH / 2 - 122, 8 * i + gTS.rcText.top - 10, &rcFrame2, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, WINDOW_WIDTH / 2 - 122, 8 * i + gTS.rcText.top - 10, &rcFrame3, SURFACE_ID_TEXT_BOX);
	}

	//Draw face picture
	rcFace.left = 48 * (gTS.face % 6);
	rcFace.top = 48 * (gTS.face / 6);
	rcFace.right = rcFace.left + 48;
	rcFace.bottom = rcFace.top + 48;

	if (gTS.face_x < (TEXT_LEFT * 0x200))
		gTS.face_x += 0x1000;

	PutBitmap3(&gTS.rcText, gTS.face_x / 0x200, gTS.rcText.top - 3, &rcFace, SURFACE_ID_FACE);

	//Draw text
	if (gTS.face)
		text_offset = 56;
	else
		text_offset = 0;

	for (i = 0; i < 4; i++)
		PutBitmap3(&gTS.rcText, text_offset + TEXT_LEFT, gTS.offsetY + gTS.ypos_line[i] + gTS.rcText.top, &gRect_line, (Surface_Ids)(i + SURFACE_ID_TEXT_LINE1));

	//Draw NOD cursor
	if ((gTS.wait_beam++ % 20 > 12) && gTS.mode == 2)
	{
		rect.left = TEXT_LEFT + 6 * gTS.p_write + text_offset;
		rect.top = gTS.ypos_line[gTS.line % 4] + gTS.rcText.top + gTS.offsetY;
		rect.right = rect.left + 5;
		rect.bottom = rect.top + 11;
#ifdef FIX_BUGS
		CortBox(&rect, nod_color);

		// This is how the Linux port fixed this, but it isn't done
		// the way Pixel would do it (he only calls GetCortBoxColor
		// once, during init functions, so our fix does it that way
		// instead).
		//CortBox(&rect, GetCortBoxColor(RGB(0xFF, 0xFF, 0xFE));
#else
		// This accidentally uses a BGR value directly, without
		// running it though GetCortBoxColor first
		CortBox(&rect, RGB(0xFF, 0xFF, 0xFE));
#endif
	}

	//Draw GIT
	SET_RECT(rcItemBox1, 0, 0, 72, 16)
	SET_RECT(rcItemBox2, 0, 8, 72, 24)
	SET_RECT(rcItemBox3, 240, 0, 244, 8)
	SET_RECT(rcItemBox4, 240, 8, 244, 16)
	SET_RECT(rcItemBox5, 240, 16, 244, 24)

	if (gTS.item)
	{
		PutBitmap3(&grcFull, (WINDOW_WIDTH - 80) / 2, WINDOW_HEIGHT - 112, &rcItemBox1, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, (WINDOW_WIDTH - 80) / 2, WINDOW_HEIGHT - 96, &rcItemBox2, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 112, &rcItemBox3, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 104, &rcItemBox4, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 96, &rcItemBox4, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 88, &rcItemBox5, SURFACE_ID_TEXT_BOX);

		if (gTS.item_y < WINDOW_HEIGHT - 104)
			++gTS.item_y;

		if (gTS.item < 1000)
		{
			rect.left = 16 * (gTS.item % 16);
			rect.right = rect.left + 16;
			rect.top = 16 * (gTS.item / 16);
			rect.bottom = rect.top + 16;
			PutBitmap3(&grcFull, (WINDOW_WIDTH - 24) / 2, gTS.item_y, &rect, SURFACE_ID_ARMS_IMAGE);
		}
		else
		{
			rect.left = 32 * ((gTS.item - 1000) % 8);
			rect.right = rect.left + 32;
			rect.top = 16 * ((gTS.item - 1000) / 8);
			rect.bottom = rect.top + 16;
			PutBitmap3(&grcFull, (WINDOW_WIDTH - 40) / 2, gTS.item_y, &rect, SURFACE_ID_ITEM_IMAGE);
		}
	}

	//Draw Yes / No selection
	SET_RECT(rect_yesno, 152, 48, 244, 80)
	SET_RECT(rect_cur, 112, 88, 128, 104)

	if (gTS.mode == 6)
	{
		if (gTS.wait < 2)
			i = (WINDOW_HEIGHT - 96) + (2 - gTS.wait) * 4;
		else
			i = WINDOW_HEIGHT - 96;

		PutBitmap3(&grcFull, (WINDOW_WIDTH + 112) / 2, i, &rect_yesno, SURFACE_ID_TEXT_BOX);
		if (gTS.wait == 16)
			PutBitmap3(&grcFull, 41 * gTS.select + (WINDOW_WIDTH + 102) / 2, WINDOW_HEIGHT - 86, &rect_cur, SURFACE_ID_TEXT_BOX);
	}
}

static void Return_MiniMapLoop(MainLoopMeta *meta, int return_value)
{
	void (*caller_return)(MainLoopMeta *meta, int return_value) = (void (*)(MainLoopMeta *meta, int return_value))meta->user_data;

	switch (return_value)
	{
		case 0:
			caller_return(meta->next, 0);
			return;

		case 2:
			caller_return(meta->next, 2);
			return;
	}
}

static void Return_StageSelectLoop(MainLoopMeta *meta, int return_value)
{
	StageSelectLoop_Data *data = (StageSelectLoop_Data*)meta->user_data;

	switch (return_value)
	{
		case 0:
			data->caller_return(meta->next, 0);
			return;

		case 1:
			JumpTextScript(data->event);
			return;

		case 2:
			data->caller_return(meta->next, 2);
			return;
	}
}

static void Return_Scene_DownIsland(MainLoopMeta *meta, int return_value)
{
	Scene_DownIsland_Data *data = (Scene_DownIsland_Data*)meta->user_data;

	switch (return_value)
	{
		case 0:
			data->caller_return(meta->next, 0);
			return;

		case 2:
			data->caller_return(meta->next, 2);
			return;
	}
}

//Parse TSC
int TextScriptProc(void (*caller_return)(MainLoopMeta *meta, int return_value))
{
	BOOL bExit;
	char c[3];
	int w, x, y, z;
	int i;
	char str[72];

	RECT rcSymbol = {64, 48, 72, 56};

	switch (gTS.mode)
	{
		case 1: //PARSE
			//Type out (faster if ok or cancel are held)
			++gTS.wait;

			if (!(g_GameFlags & 2) && (gKeyCancel | gKeyOk) & gKey)
				gTS.wait += 4;

			if (gTS.wait < 4)
				break;

			gTS.wait = 0;

			//Parsing time
			bExit = FALSE;

			while (bExit == FALSE)
			{
				if (gTS.data[gTS.p_read] == '<')
				{
					if (IS_COMMAND('E','N','D'))
					{
						gTS.mode = 0;
						gMC.cond &= ~1;
						g_GameFlags |= 3;
						gTS.face = 0;
						bExit = TRUE;
					}
					else if (IS_COMMAND('L','I','+'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						AddLifeMyChar(x);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('M','L','+'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						AddMaxLifeMyChar(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('A','E','+'))
					{
						FullArmsEnergy();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('I','T','+'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						PlaySoundObject(38, 1);
						AddItemData(x);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('I','T','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SubItemData(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('E','Q','+'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						EquipItem(z, TRUE);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('E','Q','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						EquipItem(z, FALSE);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('A','M','+'))
					{
						w = GetTextScriptNo(gTS.p_read + 4);
						x = GetTextScriptNo(gTS.p_read + 9);
						gNumberTextScript[0] = x;
						gNumberTextScript[1] = z;
						PlaySoundObject(38, 1);
						AddArmsData(w, x);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('A','M','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SubArmsData(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('Z','A','M'))
					{
						ZeroArmsEnergy_All();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('T','A','M'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						z = GetTextScriptNo(gTS.p_read + 14);
						TradeArms(x, y, z);
						gTS.p_read += 18;
					}
					else if (IS_COMMAND('P','S','+'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						AddPermitStage(x, y);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('M','P','+'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						SetMapping(x);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('U','N','I'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						ChangeMyUnit(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('S','T','C'))
					{
						SaveTimeCounter();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('T','R','A'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						w = GetTextScriptNo(gTS.p_read + 9);
						x = GetTextScriptNo(gTS.p_read + 14);
						y = GetTextScriptNo(gTS.p_read + 19);
						if (!TransferStage(z, w, x, y))
						{
							#if defined(NONPORTABLE) && defined(WINDOWS)
								#ifdef JAPANESE
								MessageBoxA(ghWnd, "�X�e�[�W�̓ǂݍ��݂Ɏ��s", "�G���[", MB_OK);
								#else
								MessageBoxA(ghWnd, "Failed to load stage", "Error", MB_OK);
								#endif
							#else
								#ifdef JAPANESE
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "�G���[", "�X�e�[�W�̓ǂݍ��݂Ɏ��s", NULL);
								#else
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to load stage", NULL);
								#endif
							#endif

							return 0;
						}
					}
					else if (IS_COMMAND('M','O','V'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						SetMyCharPosition(x * 0x200 * 0x10, y * 0x200 * 0x10);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('H','M','C'))
					{
						ShowMyChar(FALSE);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('S','M','C'))
					{
						ShowMyChar(TRUE);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('F','L','+'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetNPCFlag(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','L','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						CutNPCFlag(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('S','K','+'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetSkipFlag(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('S','K','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						CutSkipFlag(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('K','E','Y'))
					{
						g_GameFlags &= ~2;
						g_GameFlags |= 1;
						gMC.up = FALSE;
						gMC.shock = 0;
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('P','R','I'))
					{
						g_GameFlags &= ~3;
						gMC.shock = 0;
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('F','R','E'))
					{
						g_GameFlags |= 3;
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('N','O','D'))
					{
						gTS.mode = 2;
						gTS.p_read += 4;
						bExit = TRUE;
					}
					else if (IS_COMMAND('C','L','R'))
					{
						ClearTextLine();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('M','S','G'))
					{
						ClearTextLine();
						gTS.flags |= 0x03;
						gTS.flags &= ~0x30;
						if (gTS.flags & 0x40)
							gTS.flags |= 0x10;
						gTS.p_read += 4;
						bExit = TRUE;
					}
					else if (IS_COMMAND('M','S','2'))
					{
						ClearTextLine();
						gTS.flags &= ~0x12;
						gTS.flags |= 0x21;
						if (gTS.flags & 0x40)
							gTS.flags |= 0x10;
						gTS.face = 0;
						gTS.p_read += 4;
						bExit = TRUE;
					}
					else if (IS_COMMAND('M','S','3'))
					{
						ClearTextLine();
						gTS.flags &= ~0x10;
						gTS.flags |= 0x23;
						if (gTS.flags & 0x40)
							gTS.flags |= 0x10;
						gTS.p_read += 4;
						bExit = TRUE;
					}
					else if (IS_COMMAND('W','A','I'))
					{
						gTS.mode = 4;
						gTS.wait_next = GetTextScriptNo(gTS.p_read + 4);
						gTS.p_read += 8;
						bExit = TRUE;
					}
					else if (IS_COMMAND('W','A','S'))
					{
						gTS.mode = 7;
						gTS.p_read += 4;
						bExit = TRUE;
					}
					else if (IS_COMMAND('T','U','R'))
					{
						gTS.p_read += 4;
						gTS.flags |= 0x10;
					}
					else if (IS_COMMAND('S','A','T'))
					{
						gTS.p_read += 4;
						gTS.flags |= 0x40;
					}
					else if (IS_COMMAND('C','A','T'))
					{
						gTS.p_read += 4;
						gTS.flags |= 0x40;
					}
					else if (IS_COMMAND('C','L','O'))
					{
						gTS.flags &= ~0x33;
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('E','V','E'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						JumpTextScript(z);
					}
					else if (IS_COMMAND('Y','N','J'))
					{
						gTS.next_event = GetTextScriptNo(gTS.p_read + 4);
						gTS.p_read += 8;
						gTS.mode = 6;
						PlaySoundObject(5, 1);
						gTS.wait = 0;
						gTS.select = 0;
						bExit = TRUE;
					}
					else if (IS_COMMAND('F','L','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);

						if (GetNPCFlag(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('S','K','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);

						if (GetSkipFlag(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('I','T','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);

						if (CheckItem(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('A','M','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);

						if (CheckArms(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('U','N','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);
						if (GetUnitMyChar() == x)
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('E','C','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);
						if (GetNpCharAlive(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('N','C','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						z = GetTextScriptNo(gTS.p_read + 9);
						if (IsNpCharCode(x))
							JumpTextScript(z);
						else
							gTS.p_read += 13;
					}
					else if (IS_COMMAND('M','P','J'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						if (IsMapping())
							JumpTextScript(x);
						else
							gTS.p_read += 8;
					}
					else if (IS_COMMAND('S','S','S'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						SetNoise(1, x);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('C','S','S'))
					{
						CutNoise();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('S','P','S'))
					{
					#ifdef FIX_BUGS
						SetNoise(2, 0);
					#else
						// x is not initialised. This bug isn't too bad, since that parameter's not used when the first one is set to 2, but still.
						SetNoise(2, x);
					#endif
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('C','P','S'))
					{
						CutNoise();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('Q','U','A'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetQuake(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','L','A'))
					{
						SetFlash(0, 0, 2);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('F','A','I'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						StartFadeIn(z);
						gTS.mode = 5;
						gTS.p_read += 8;
						bExit = TRUE;
					}
					else if (IS_COMMAND('F','A','O'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						StartFadeOut(z);
						gTS.mode = 5;
						gTS.p_read += 8;
						bExit = TRUE;
					}
					else if (IS_COMMAND('M','N','A'))
					{
						StartMapName();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('F','O','M'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetFrameTargetMyChar(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','O','N'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						SetFrameTargetNpChar(x, y);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('F','O','B'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						SetFrameTargetBoss(x, y);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('S','O','U'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						PlaySoundObject(z, 1);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('C','M','U'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						ChangeMusic(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','M','U'))
					{
						SetOrganyaFadeout();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('R','M','U'))
					{
						ReCallMusic();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('M','L','P'))
					{
						gTS.p_read += 4;
						bExit = TRUE;

						EnterMainLoop(MiniMapLoop, Return_MiniMapLoop, (void*)caller_return);
					}
					else if (IS_COMMAND('S','L','P'))
					{
						bExit = TRUE;

						static StageSelectLoop_Data data = {0, caller_return};

						EnterMainLoop(StageSelectLoop, Return_StageSelectLoop, &data);

						g_GameFlags &= ~3;
					}
					else if (IS_COMMAND('D','N','P'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						DeleteNpCharEvent(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('D','N','A'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						DeleteNpCharCode(z, 1);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('B','O','A'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetBossCharActNo(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('C','N','P'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						z = GetTextScriptNo(gTS.p_read + 14);
						ChangeNpCharByEvent(x, y, z);
						gTS.p_read += 18;
					}
					else if (IS_COMMAND('A','N','P'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						z = GetTextScriptNo(gTS.p_read + 14);
						SetNpCharActionNo(x, y, z);
						gTS.p_read += 18;
					}
					else if (IS_COMMAND('I','N','P'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						z = GetTextScriptNo(gTS.p_read + 14);
						ChangeCheckableNpCharByEvent(x, y, z);
						gTS.p_read += 18;
					}
					else if (IS_COMMAND('S','N','P'))
					{
						w = GetTextScriptNo(gTS.p_read + 4);
						x = GetTextScriptNo(gTS.p_read + 9);
						y = GetTextScriptNo(gTS.p_read + 14);
						z = GetTextScriptNo(gTS.p_read + 19);
						SetNpChar(w, x * 0x200 * 0x10, y * 0x200 * 0x10, 0, 0, z, 0, 0x100);
						gTS.p_read += 23;
					}
					else if (IS_COMMAND('M','N','P'))
					{
						w = GetTextScriptNo(gTS.p_read + 4);
						x = GetTextScriptNo(gTS.p_read + 9);
						y = GetTextScriptNo(gTS.p_read + 14);
						z = GetTextScriptNo(gTS.p_read + 19);
						MoveNpChar(w, x * 0x200 * 0x10, y * 0x200 * 0x10, z);
						gTS.p_read += 23;
					}
					else if (IS_COMMAND('S','M','P'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						ShiftMapParts(x, y);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('C','M','P'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						z = GetTextScriptNo(gTS.p_read + 14);
						ChangeMapParts(x, y, z);
						gTS.p_read += 18;
					}
					else if (IS_COMMAND('B','S','L'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);

						if (z)
							StartBossLife(z);
						else
							StartBossLife2();

						gTS.p_read += 8;
					}
					else if (IS_COMMAND('M','Y','D'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetMyCharDirect(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('M','Y','B'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						BackStepMyChar(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('M','M','0'))
					{
						ZeroMyCharXMove();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('I','N','I'))
					{
						InitializeGame(ghWnd);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('S','V','P'))
					{
						SaveProfile(NULL);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('L','D','P'))
					{
						if (!LoadProfile(NULL))
							InitializeGame(ghWnd);
					}
					else if (IS_COMMAND('F','A','C'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						if (gTS.face != (signed char)z)
						{
							gTS.face = (signed char)z;
							gTS.face_x = (WINDOW_WIDTH / 2 - 156) * 0x200;
						}
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','A','C'))	// Duplicate command
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						if (gTS.face != (signed char)z)
						{
							gTS.face = (signed char)z;
							gTS.face_x = (WINDOW_WIDTH / 2 - 156) * 0x200;
						}
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('G','I','T'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						gTS.item = z;
						gTS.item_y = WINDOW_HEIGHT - 112;
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('N','U','M'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetNumberTextScript(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('C','R','E'))
					{
						g_GameFlags |= 8;
						StartCreditScript();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('S','I','L'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						SetCreditIllust(z);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('C','I','L'))
					{
						CutCreditIllust();
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('X','X','1'))
					{
						bExit = TRUE;
						z = GetTextScriptNo(gTS.p_read + 4);

						static Scene_DownIsland_Data data = {ghWnd, z, caller_return};

						EnterMainLoop(Scene_DownIsland, Return_Scene_DownIsland, &data);

						gTS.p_read += 8;
					}
					else if (IS_COMMAND('E','S','C'))
					{
						return 2;
					}
					else
					{
						char str_0[0x40];
						#ifdef JAPANESE
							sprintf(str_0, "�s���̃R�[�h:<%c%c%c", gTS.data[gTS.p_read + 1], gTS.data[gTS.p_read + 2], gTS.data[gTS.p_read + 3]);
							#if defined(NONPORTABLE) && defined(WINDOWS)
								MessageBoxA(NULL, str_0, "�G���[", MB_OK);
							#else
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "�G���[", str_0, NULL);
							#endif
						#else
							sprintf(str_0, "Unknown code:<%c%c%c", gTS.data[gTS.p_read + 1], gTS.data[gTS.p_read + 2], gTS.data[gTS.p_read + 3]);
							#if defined(NONPORTABLE) && defined(WINDOWS)
								MessageBoxA(NULL, str_0, "Error", MB_OK);
							#else
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str_0, NULL);
							#endif
						#endif

						return 0;
					}
				}
				else
				{
					if (gTS.data[gTS.p_read] == '\r')
					{
						//Go to new-line
						gTS.p_read += 2;
						gTS.p_write = 0;

						if (gTS.flags & 1)
						{
							++gTS.line;
							CheckNewLine();
						}
					}
					else if (gTS.flags & 0x10)
					{
						//SAT/CAT/TUR printing
						x = gTS.p_read;
						//Break if reaches command, or new-line
						while (gTS.data[x] != '<' && gTS.data[x] != '\r')
						{
							//Skip if SHIFT-JIS
							if (gTS.data[x] & 0x80)
								++x;

							++x;
						}

						//Get text to copy
						y = x - gTS.p_read;
						memcpy(str, &gTS.data[gTS.p_read], y);
						str[y] = 0;

						gTS.p_write = x;

						//Print text
						PutText2(0, 0, str, RGB(0xFF, 0xFF, 0xFE), (Surface_Ids)(gTS.line % 4 + SURFACE_ID_TEXT_LINE1));
					#ifdef FIX_BUGS
						strcpy(&text[gTS.line % 4 * 0x40], str);
					#else
						sprintf(&text[gTS.line % 4 * 0x40], str);	// No point to using an sprintf here, and it makes Clang mad
					#endif

						//Check if should move to next line (prevent a memory overflow, come on guys, this isn't a leftover of pixel trying to make text wrapping)
						gTS.p_read += y;

						if (gTS.p_write >= 35)
							CheckNewLine();

						bExit = TRUE;
					}
					else
					{
						//Get text to print
						c[0] = gTS.data[gTS.p_read];

						if (c[0] & 0x80)
						{
							c[1] = gTS.data[gTS.p_read + 1];
							c[2] = '\0';
						}
						else
						{
							c[1] = '\0';
						}

						//Print text
						if (c[0] == '=')
						{
							Surface2Surface(6 * gTS.p_write, 2, &rcSymbol, (Surface_Ids)(gTS.line % 4 + SURFACE_ID_TEXT_LINE1), SURFACE_ID_TEXT_BOX);
						}
						else
						{
							PutText2(6 * gTS.p_write, 0, c, RGB(0xFF, 0xFF, 0xFE), (Surface_Ids)(gTS.line % 4 + SURFACE_ID_TEXT_LINE1));
						}

						strcat(&text[gTS.line % 4 * 0x40], c);
						PlaySoundObject(2, 1);
						gTS.wait_beam = 0;

						//Offset read and write positions
						if (c[0] & 0x80)
						{
							gTS.p_read += 2;
							gTS.p_write += 2;
						}
						else
						{
							gTS.p_read++;
							gTS.p_write++;
						}

						if (gTS.p_write >= 35)
						{
							CheckNewLine();
							gTS.p_write = 0;
							++gTS.line;
							CheckNewLine();
						}

						bExit = TRUE;
					}
				}
			}
			break;

		case 2: //NOD
			if ((gKeyCancel | gKeyOk) & gKeyTrg)
				gTS.mode = 1;
			break;

		case 3: //NEW LINE
			for (i = 0; i < 4; i++)
			{
				gTS.ypos_line[i] -= 4;

				if (!gTS.ypos_line[i])
					gTS.mode = 1;

				if (gTS.ypos_line[i] == -16)
					gTS.ypos_line[i] = 48;
			}
			break;

		case 4: //WAI
			if (gTS.wait_next == 9999)
				break;

			if (gTS.wait != 9999)
				++gTS.wait;

			if (gTS.wait < gTS.wait_next)
				break;

			gTS.mode = 1;
			gTS.wait_beam = 0;
			break;

		case 5: //FAI/FAO
			if (GetFadeActive())
				break;

			gTS.mode = 1;
			gTS.wait_beam = 0;
			break;
		case 7: //WAS
			if ((gMC.flag & 8) == 0)
			break;

			gTS.mode = 1;
			gTS.wait_beam = 0;
			break;
		case 6: //YNJ
			if (gTS.wait < 16)
			{
				gTS.wait++;
			}
			else
			{
				//Select option
				if (gKeyTrg & gKeyOk)
				{
					PlaySoundObject(18, 1);

					if (gTS.select == 1)
					{
						JumpTextScript(gTS.next_event);
					}
					else
					{
						gTS.mode = 1;
						gTS.wait_beam = 0;
					}
				}
				//Yes
				else if (gKeyTrg & gKeyLeft)
				{
					gTS.select = 0;
					PlaySoundObject(1, 1);
				}
				//No
				else if (gKeyTrg & gKeyRight)
				{
					gTS.select = 1;
					PlaySoundObject(1, 1);
				}
			}
			break;
	}

	if (gTS.mode == 0)
		g_GameFlags &= ~4;
	else
		g_GameFlags |= 4;
	return 1;
}
