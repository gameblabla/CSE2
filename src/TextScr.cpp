#include <stdint.h>
#include <string>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "TextScr.h"
#include "Draw.h"
#include "Tags.h"
#include "ArmsItem.h"
#include "MyChar.h"
#include "Fade.h"
#include "Stage.h"
#include "Frame.h"
#include "MycParam.h"
#include "Flags.h"
#include "Profile.h"
#include "Map.h"
#include "MapName.h"
#include "KeyControl.h"
#include "NpChar.h"
#include "Sound.h"
#include "Organya.h"
#include "Game.h"

#define IS_COMMAND(c1, c2, c3) gTS.data[gTS.p_read + 1] == c1 && gTS.data[gTS.p_read + 2] == c2 && gTS.data[gTS.p_read + 3] == c3

#define TSC_BUFFER_SIZE 0x5000

TEXT_SCRIPT gTS;

int gNumberTextScript[4];
char text[0x100];

RECT gRect_line = {0, 0, 216, 16};

//Initialize and end tsc
bool InitTextScript2()
{
	//Clear flags
	gTS.mode = 0;
	g_GameFlags &= ~0x04;
	
	//Create line surfaces
	for (int i = 0; i < 4; i++)
		MakeSurface_Generic(gRect_line.right, gRect_line.bottom, i + 30);
	
	//Clear text
	memset(text, 0, sizeof(text));
	
	//Allocate script buffer
	gTS.data = (char*)malloc(TSC_BUFFER_SIZE);
	return gTS.data != nullptr;
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
void EncryptionBinaryData2(uint8_t *pData, int size)
{
	int val1;
	
	int half = size / 2;
	if (pData[half])
		val1 = -pData[half];
	else
		val1 = -7;
	
	for (int i = 0; i < size; i++)
	{
		if ( i != half )
			pData[i] += val1;
	}
}

//Load generic .tsc
bool LoadTextScript2(char *name)
{
	//Get path
	char path[260];
	sprintf(path, "%s/%s", gDataPath, name);
	
	//Open file
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Read data
	gTS.size = SDL_RWsize(fp);
	fp->read(fp, gTS.data, 1, gTS.size);
	gTS.data[gTS.size] = 0;
	fp->close(fp);
	
	//Set path
	strcpy(gTS.path, name);
	
	//Decrypt data
	EncryptionBinaryData2((uint8_t*)gTS.data, gTS.size);
	return true;
}

//Load stage .tsc
bool LoadTextScript_Stage(char *name)
{
	//Open Head.tsc
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, "Head.tsc");
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Read Head.tsc
	int head_size = SDL_RWsize(fp);
	fp->read(fp, gTS.data, 1, head_size);
	EncryptionBinaryData2((uint8_t*)gTS.data, head_size);
	gTS.data[head_size] = 0;
	SDL_RWclose(fp);
	
	//Open stage's .tsc
	sprintf(path, "%s/%s", gDataPath, name);
	
	fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	//Read stage's tsc
	int body_size = SDL_RWsize(fp);
	fp->read(fp, &gTS.data[head_size], 1, body_size);
	EncryptionBinaryData2((uint8_t*)&gTS.data[head_size], body_size);
	gTS.data[head_size + body_size] = 0;
	SDL_RWclose(fp);
	
	//Set parameters
	gTS.size = head_size + body_size;
	strcpy(gTS.path, name);
	return true;
}

//Get current path
void GetTextScriptPath(char *path)
{
	strcpy(path, gTS.path);
}

//Get 4 digit number from TSC data
int GetTextScriptNo(int a)
{
	return gTS.data[a + 3] - 48
		+ 10 * gTS.data[a + 2] - 480
		+ 100 * gTS.data[a + 1] - 4800
		+ 1000 * gTS.data[a] - 48000;
}

//Start TSC event
bool StartTextScript(int no)
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
	
	gTS.rcText = {WINDOW_WIDTH / 2 - 108, WINDOW_HEIGHT - 56, WINDOW_WIDTH + 108, WINDOW_HEIGHT - 8};
	
	//Clear text lines
	for (int i = 0; i < 4; i++)
	{
		gTS.ypos_line[i] = 16 * i;
		CortBox2(&gRect_line, 0x000000, i + 30);
		memset(&text[i * 0x40], 0, 0x40);
	}
	
	//Find where event starts
	for (gTS.p_read = 0; ; gTS.p_read++)
	{
		//Check if we are still in the proper range
		if (!gTS.data[gTS.p_read])
			return false;
		
		//Check if we are at an event
		if (gTS.data[gTS.p_read] != '#')
			continue;
		
		//Check if this is our event
		int event_no = GetTextScriptNo(++gTS.p_read);
		
		if (no == event_no)
			break;
		if (no < event_no)
			return false;
	}
	
	//Advance until new-line
	while (gTS.data[gTS.p_read] != '\n')
		++gTS.p_read;
	++gTS.p_read;
	
	return true;
}

bool JumpTextScript(int no)
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
		CortBox2(&gRect_line, 0x000000, i + 30);
		memset(&text[i * 0x40], 0, 0x40);
	}
	
	//Find where event starts
	for (gTS.p_read = 0; ; gTS.p_read++)
	{
		//Check if we are still in the proper range
		if (!gTS.data[gTS.p_read])
			return false;
		
		//Check if we are at an event
		if (gTS.data[gTS.p_read] != '#')
			continue;
		
		//Check if this is our event
		int event_no = GetTextScriptNo(++gTS.p_read);
		
		if (no == event_no)
			break;
		if (no < event_no)
			return false;
	}
	
	//Advance until new-line
	while (gTS.data[gTS.p_read] != '\n')
		++gTS.p_read;
	++gTS.p_read;
	
	return true;
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
		CortBox2(&gRect_line, 0, gTS.line % 4 + 30);
		memset(&text[gTS.line % 4 * 0x40], 0, 0x40);
	}
}

//Type a number into the text buffer
void SetNumberTextScript(int index)
{
	//Get digit table
	int table[3];
	table[0] = 1000;
	table[1] = 100;
	table[2] = 10;
	
	//Get number to print
	int a = gNumberTextScript[index];
	int b;
	
	char str[5];
	bool bZero = false;
	int offset = 0;
	
	for (int i = 0; i < 3; i++)
	{
		if (a / table[i] || bZero)
		{
			//Do whatever the fuck this is
			b = a / table[i];
			str[offset] = b + 0x30;
			bZero = true;
			a -= b * table[i];
			++offset;
		}
	}
	
	//Set last digit of string, and add null terminator
	str[offset] = a + 0x30;
	str[offset + 1] = 0;
	
	//Append number to line
	PutText2(6 * gTS.p_write, 0, str, 0xFFFFFE, gTS.line % 4 + 30);
	strcat(&text[gTS.line % 4 * 0x40], str);
	
	//Play sound and reset blinking cursor
	PlaySoundObject(2, 1);
	gTS.wait_beam = 0;
	
	//Check if should move to next line (prevent a memory overflow, come on guys, this isn't a leftover of pixel trying to make text wrapping)
	gTS.p_write += strlen(str);
	
	if (gTS.p_write > 34)
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
		CortBox2(&gRect_line, 0x000000, i + 30);
		memset(&text[i * 0x40], 0, 0x40);
	}
}

//Draw textbox and whatever else
void PutTextScript()
{
	if (gTS.mode && (gTS.flags & 1))
	{
		//Set textbox position
		if (gTS.flags & 0x20)
		{
			gTS.rcText.top = 32;
			gTS.rcText.bottom = 80;
		}
		else
		{
			gTS.rcText.top = WINDOW_HEIGHT - 56;
			gTS.rcText.bottom = WINDOW_HEIGHT - 8;
		}
		
		//Draw textbox
		if (gTS.flags & 2)
		{
			RECT rcFrame1 = {0, 0, 244, 8};
			RECT rcFrame2 = {0, 8, 244, 16};
			RECT rcFrame3 = {0, 16, 244, 24};
			
			PutBitmap3(&grcFull, gTS.rcText.left - 14, gTS.rcText.top - 10, &rcFrame1, 26);
			int i;
			for (i = 1; i < 7; i++)
				PutBitmap3(&grcFull, gTS.rcText.left - 14, 8 * i + gTS.rcText.top - 10, &rcFrame2, 26);
			PutBitmap3(&grcFull, gTS.rcText.left - 14, 8 * i + gTS.rcText.top - 10, &rcFrame3, 26);
		}
		
		//Draw face picture
		RECT rcFace;
		rcFace.left = 48 * (gTS.face % 6);
		rcFace.top = 48 * (gTS.face / 6);
		rcFace.right = 48 * (gTS.face % 6) + 48;
		rcFace.bottom = 48 * (gTS.face / 6) + 48;
		
		if (gTS.face_x < (gTS.rcText.left << 9))
			gTS.face_x += 0x1000;
		PutBitmap3(&gTS.rcText, gTS.face_x / 0x200, gTS.rcText.top - 3, &rcFace, 27);
		
		//Draw text
		int text_offset;
		if (gTS.face)
			text_offset = 56;
		else
			text_offset = 0;
		
		for (int i = 0; i < 4; i++)
			PutBitmap3(&gTS.rcText, text_offset + gTS.rcText.left, gTS.offsetY + gTS.ypos_line[i] + gTS.rcText.top, &gRect_line, i + 30);
		
		//Draw NOD cursor
		if ((gTS.wait_beam++ % 20 > 12) && gTS.mode == 2)
		{
			RECT rect;
			rect.left = gTS.rcText.left + text_offset + 6 * gTS.p_write;
			rect.top = gTS.rcText.top + gTS.ypos_line[gTS.line % 4] + gTS.offsetY;
			rect.right = rect.left + 5;
			rect.bottom = rect.top + 11;
			CortBox(&rect, 0xFFFFFE);
		}
		
		//Draw GIT
		RECT rcItemBox1 = {0, 0, 72, 16};
		RECT rcItemBox2 = {0, 8, 72, 24};
		RECT rcItemBox3 = {240, 0, 244, 8};
		RECT rcItemBox4 = {240, 8, 244, 16};
		RECT rcItemBox5 = {240, 16, 244, 24};
		
		if (gTS.item)
		{
			PutBitmap3(&grcFull, (WINDOW_WIDTH - 80) / 2, WINDOW_HEIGHT - 112, &rcItemBox1, 26);
			PutBitmap3(&grcFull, (WINDOW_WIDTH - 80) / 2, WINDOW_HEIGHT - 96, &rcItemBox2, 26);
			PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 112, &rcItemBox3, 26);
			PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 104, &rcItemBox4, 26);
			PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 96, &rcItemBox4, 26);
			PutBitmap3(&grcFull, (WINDOW_WIDTH + 64) / 2, WINDOW_HEIGHT - 88, &rcItemBox5, 26);
			
			if (gTS.item_y < WINDOW_HEIGHT - 104)
				++gTS.item_y;
			
			RECT rect;
			if (gTS.item >= 1000)
			{
				rect.left = 32 * ((gTS.item - 1000) % 8);
				rect.right = 32 * ((gTS.item - 1000) % 8) + 32;
				rect.top = 16 * ((gTS.item - 1000) / 8);
				rect.bottom = 16 * ((gTS.item - 1000) / 8) + 16;
				PutBitmap3(&grcFull, (WINDOW_WIDTH - 40) / 2, gTS.item_y, &rect, 8);
			}
			else
			{
				rect.left = 16 * (gTS.item % 16);
				rect.right = 16 * (gTS.item % 16) + 16;
				rect.top = 16 * (gTS.item / 16);
				rect.bottom = 16 * (gTS.item / 16) + 16;
				PutBitmap3(&grcFull, (WINDOW_WIDTH - 24) / 2, gTS.item_y, &rect, 12);
			}
		}
		
		//Draw Yes / No selection
		RECT rect_yesno = {152, 48, 244, 80};
		RECT rect_cur = {112, 88, 128, 104};
		
		if (gTS.mode == 6 )
		{
			int i;
			if (gTS.wait > 1)
				i = WINDOW_HEIGHT - 96;
			else
				i = WINDOW_HEIGHT - 88 - gTS.wait * 4;
			
			PutBitmap3(&grcFull, (WINDOW_WIDTH + 112) / 2, i, &rect_yesno, 26);
			if (gTS.wait == 16)
				PutBitmap3(&grcFull, 41 * gTS.select + (WINDOW_WIDTH + 102) / 2, 154, &rect_cur, 26);
		}
	}
}

//Parse TSC
int TextScriptProc()
{
	RECT rcSymbol = {64, 48, 72, 56};
	
	bool bExit;
	
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
			int w, x, y, z;
			bExit = false;
			
			while (!bExit)
			{
				if (gTS.data[gTS.p_read] != '<')
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
						x;
						for (x = gTS.p_read; ; x++)
						{
							//Break if reaches command, or new-line
							if (gTS.data[x] == '<' || gTS.data[x] == '\r')
								break;
							//Skip if SHIFT-JIS
							if (gTS.data[x] & 0x80)
								++x;
						}
						
						//Get text to copy
						char str[72];
						int length = x - gTS.p_read;
						memcpy(str, &gTS.data[gTS.p_read], length);
						str[length] = 0;
						
						gTS.p_write = x;
						
						//Print text
						PutText2(0, 0, str, 0xFFFFFE, gTS.line % 4 + 30);
						sprintf(&text[gTS.line % 4 * 0x40], str);
						
						//Check if should move to next line (prevent a memory overflow, come on guys, this isn't a leftover of pixel trying to make text wrapping)
						gTS.p_read += length;

						if (gTS.p_write > 34)
							CheckNewLine();
						
						bExit = true;
					}
					else
					{
						//Get text to print
						char c[3];
						c[0] = gTS.data[gTS.p_read];
						
						if (!(c[0] & 0x80))
						{
							c[1] = 0;
						}
						else
						{
							c[1] = gTS.data[gTS.p_read + 1];
							c[2] = 0;
						}
						
						//Print text
						if (c[0] == '=')
						{
							Surface2Surface(6 * gTS.p_write, 2, &rcSymbol, gTS.line % 4 + 30, 26);
						}
						else
						{
							PutText2(6 * gTS.p_write, 0, c, 0xFFFFFE, gTS.line % 4 + 30);
						}
						
						strcat(&text[gTS.line % 4 * 0x40], c);
						PlaySoundObject(2, 1);
						gTS.wait_beam = 0;
						
						//Offset read and write positions
						if (!(c[0] & 0x80))
						{
							gTS.p_read++;
							gTS.p_write++;
						}
						else
						{
							gTS.p_read += 2;
							gTS.p_write += 2;
						}
						
						if (gTS.p_write > 34)
						{
							CheckNewLine();
							gTS.p_write = 0;
							++gTS.line;
							CheckNewLine();
						}
						
						bExit = true;
					}
				}
				else
				{
					if (IS_COMMAND('E','N','D'))
					{
						gTS.mode = 0;
						gMC.cond &= ~1;
						g_GameFlags |= 3;
						gTS.face = 0;
						bExit = true;
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
						EquipItem(z, true);
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('E','Q','-'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						EquipItem(z, false);
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
					else if (IS_COMMAND('T','R','A'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						w = GetTextScriptNo(gTS.p_read + 9);
						x = GetTextScriptNo(gTS.p_read + 14);
						y = GetTextScriptNo(gTS.p_read + 19);
						if (!TransferStage(z, w, x, y))
							return 0;
					}
					else if (IS_COMMAND('M','O','V'))
					{
						x = GetTextScriptNo(gTS.p_read + 4);
						y = GetTextScriptNo(gTS.p_read + 9);
						SetMyCharPosition(x << 13, y << 13);
						gTS.p_read += 13;
					}
					else if (IS_COMMAND('H','M','C'))
					{
						ShowMyChar(false);
						gTS.p_read += 4;
					}
					else if (IS_COMMAND('S','M','C'))
					{
						ShowMyChar(true);
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
						gMC.up = false;
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
						bExit = true;
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
						bExit = true;
					}
					else if (IS_COMMAND('M','S','2'))
					{
						ClearTextLine();
						gTS.flags |= 0x21;
						gTS.flags &= ~0x12;
						if (gTS.flags & 0x40)
							gTS.flags |= 0x10;
						gTS.face = 0;
						gTS.p_read += 4;
						bExit = true;
					}
					else if (IS_COMMAND('M','S','3'))
					{
						ClearTextLine();
						gTS.flags |= 0x23;
						gTS.flags &= ~0x10;
						if (gTS.flags & 0x40)
							gTS.flags |= 0x10;
						gTS.p_read += 4;
						bExit = true;
					}
					else if (IS_COMMAND('W','A','I'))
					{
						gTS.mode = 4;
						gTS.wait_next = GetTextScriptNo(gTS.p_read + 4);
						gTS.p_read += 8;
						bExit = true;
					}
					else if (IS_COMMAND('W','A','S'))
					{
						gTS.mode = 7;
						gTS.p_read += 4;
						bExit = true;
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
						bExit = true;
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
						SetNoise(2, x);
						gTS.p_read += 8;
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
					else if (IS_COMMAND('F','A','I'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						StartFadeIn(z);
						gTS.mode = 5;
						gTS.p_read += 8;
						bExit = true;
					}
					else if (IS_COMMAND('F','A','O'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						StartFadeOut(z);
						gTS.mode = 5;
						gTS.p_read += 8;
						bExit = true;
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
						SetNpChar(w, x << 13, y << 13, 0, 0, z, 0, 0x100);
						gTS.p_read += 23;
					}
					else if (IS_COMMAND('M','N','P'))
					{
						w = GetTextScriptNo(gTS.p_read + 4);
						x = GetTextScriptNo(gTS.p_read + 9);
						y = GetTextScriptNo(gTS.p_read + 14);
						z = GetTextScriptNo(gTS.p_read + 19);
						MoveNpChar(w, x << 13, y << 13, z);
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
						InitializeGame();
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
							InitializeGame();
					}
					else if (IS_COMMAND('F','A','C'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						if (gTS.face != z)
						{
							gTS.face = z;
							gTS.face_x = (gTS.rcText.left - 48) << 9;
						}
						gTS.p_read += 8;
					}
					else if (IS_COMMAND('F','A','C'))
					{
						z = GetTextScriptNo(gTS.p_read + 4);
						if (gTS.face != z)
						{
							gTS.face = z;
							gTS.face_x = (gTS.rcText.left - 48) << 9;
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
					else if (IS_COMMAND('E','S','C'))
					{
						return 2;
					}
					else
					{
						printf("Unimplemented command: <%c%c%c\n", (char)gTS.data[gTS.p_read + 1], (char)gTS.data[gTS.p_read + 2], (char)gTS.data[gTS.p_read + 3]);
						gTS.p_read += 4;
						return 1;
					}
				}
			}
			break;
			
		case 2: //NOD
			if ((gKeyCancel | gKeyOk) & gKeyTrg)
				gTS.mode = 1;
			break;
			
		case 3: //NEW LINE
			for (int i = 0; i < 4; i++)
			{
				gTS.ypos_line[i] -= 4;
				
				if (!gTS.ypos_line[i])
					gTS.mode = 1;
				
				if (gTS.ypos_line[i] == -16)
					gTS.ypos_line[i] = 48;
			}
			break;
			
		case 4: //WAI
			if (gTS.wait_next != 9999)
			{
				if (gTS.wait != 9999)
					++gTS.wait;
				
				if (gTS.wait >= gTS.wait_next)
				{
					gTS.mode = 1;
					gTS.wait_beam = 0;
				}
			}
			break;
			
		case 5: //FAI/FAO
			if (!GetFadeActive())
			{
				gTS.mode = 1;
				gTS.wait_beam = 0;
			}
			break;
		case 7: //WAS
			if (gMC.flag & 8)
			{
				gTS.mode = 1;
				gTS.wait_beam = 0;
			}
			break;
		case 6: //YNJ
			if (gTS.wait >= 16)
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
			else
			{
				gTS.wait++;
			}
			break;
	}
	
	if (gTS.mode)
		g_GameFlags |= 4;
	else
		g_GameFlags &= ~4;
	return 1;
}
