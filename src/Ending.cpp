#include <stdint.h>
#include <string>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "Tags.h"
#include "Generic.h"
#include "Ending.h"
#include "Flags.h"
#include "Draw.h"
#include "TextScr.h"

CREDIT Credit;
STRIP Strip[16];
ILLUSTRATION Illust;

//Update casts
void ActionStripper()
{
	for (int s = 0; s < 16; s++)
	{
		//Move up
		if (Strip[s].flag & 0x80 && Credit.mode)
			Strip[s].y -= 0x100;
		//Get removed when off-screen
		if (Strip[s].y <= -0x2000)
			Strip[s].flag = 0;
	}
}

//Draw casts
void PutStripper()
{
	for (int s = 0; s < 16; s++)
	{
		if (Strip[s].flag & 0x80)
		{
			//Draw text
			RECT rc = {0, 16 * s, 320, 16 * s + 16};
			PutBitmap3(&grcFull, Strip[s].x / 0x200, Strip[s].y / 0x200, &rc, SURFACE_ID_CREDIT_CAST);
			
			//Draw character
			rc.left = 24 * (Strip[s].cast % 13);
			rc.right = rc.left + 24;
			rc.top = 24 * (Strip[s].cast / 13);
			rc.bottom = rc.top + 24;
			PutBitmap3(&grcFull, Strip[s].x / 0x200 - 24, Strip[s].y / 0x200 - 8, &rc, SURFACE_ID_CASTS);
		}
	}
}

//Create a cast object
void SetStripper(int x, int y, char *text, int cast)
{
	for (int s = 0; s < 16; s++)
	{
		if (!(Strip[s].flag & 0x80))
		{
			//Initialize cast property
			Strip[s].flag = 0x80;
			Strip[s].x = x;
			Strip[s].y = y;
			Strip[s].cast = cast;
			strcpy(Strip[s].str, text);
			
			//Draw text
			RECT rc = {0, 16 * s, 320, 16 * s + 16};
			CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
			PutText2(0, 16 * s, text, 0xFFFFFE, SURFACE_ID_CREDIT_CAST);
			break;
		}
	}
}

//Regenerate cast text
void RestoreStripper()
{
	for (int s = 0; s < 16; s++)
	{
		if (Strip[s].flag & 0x80)
		{
			RECT rc = {0, 16 * s, 320, 16 * s + 16};
			CortBox2(&rc, 0, SURFACE_ID_CREDIT_CAST);
			PutText2(0, rc.top, Strip[s].str, 0xFFFFFE, SURFACE_ID_CREDIT_CAST);
		}
	}
}

//Handle the illustration
void ActionIllust()
{
	switch (Illust.act_no)
	{
		case 1: //Move in from the left
			Illust.x += 0x5000;
			if (Illust.x > 0)
				Illust.x = 0;
			break;
			
		case 2: //Move out from the right
			Illust.x -= 0x5000;
			if (Illust.x < -0x14000)
				Illust.x = -0x14000;
			break;
			
		case 0: //Off-screen to the left
			Illust.x = -0x14000;
			break;
	}
}

//Draw illustration
void PutIllust()
{
	RECT rcIllust = {0, 0, 160, 240};
	PutBitmap3(&grcFull, Illust.x / 0x200, 0, &rcIllust, SURFACE_ID_CREDITS_IMAGE);
}

//Load illustration
void ReloadIllust(int a)
{
	char name[16];
	sprintf(name, "CREDIT%02ld", a);
	ReloadBitmap_Resource(name, SURFACE_ID_CREDITS_IMAGE);
}

//Initialize and release credits
void InitCreditScript()
{
	memset(&Credit, 0, sizeof(CREDIT));
	memset(Strip, 0, sizeof(Strip));
}

void ReleaseCreditScript()
{
	if (Credit.pData)
	{
		free(Credit.pData);
		Credit.pData = NULL;
	}
}

//Start playing credits
bool StartCreditScript()
{
	//Clear previously existing credits data
	if (Credit.pData)
	{
		free(Credit.pData);
		Credit.pData = NULL;
	}
	
	//Open file
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, "Credit.tsc");
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
	{
		printf("Couldn't open %s", path);
		return false;
	}
	
	//Allcoate buffer data
	Credit.size = SDL_RWsize(fp);
	
	Credit.pData = (char*)malloc(Credit.size);
	if (!Credit.pData)
		return false;
	
	//Read data
	fp->read(fp, Credit.pData, 1, Credit.size);
	EncryptionBinaryData2((uint8_t*)Credit.pData, Credit.size);
	
	//Reset credits
	Credit.offset = 0;
	Credit.wait = 0;
	Credit.mode = 1;
	Illust.x = -0x14000;
	Illust.act_no = 0;
	
	//Modify cliprect
	grcGame.left = 160;
	
	//Reload casts
	if (!ReloadBitmap_File("casts", SURFACE_ID_CASTS))
	{
		return false;
	}
	
	//Clear casts
	memset(Strip, 0, sizeof(Strip));
	SDL_RWclose(fp);
	return true;
}

//Get number from text (4 digit)
int GetScriptNumber(const char *text)
{
	return 1000 * *text - 48000 + 100 * text[1] - 4800 + 10 * text[2] - 480 + text[3] - 48;
}

//Parse credits
void ActionCredit_Read()
{
	while (Credit.offset < Credit.size)
	{
		uint8_t character = Credit.pData[Credit.offset];
		
		int a, b, len;
		switch (character)
		{
			case '[': //Create cast
				//Get the range for the cast text
				a = ++Credit.offset;
				
				while (Credit.pData[a] != ']')
				{
					if (IsShiftJIS(Credit.pData[a]))
						a += 2;
					else
						a++;
				}
				
				len = a - Credit.offset;
				
				//Copy the text to the cast text
				char text[40];
				memcpy(text, &Credit.pData[Credit.offset], a - Credit.offset);
				text[len] = 0;
				
				//Get cast id
				Credit.offset = a + 1;
				len = GetScriptNumber(&Credit.pData[a + 1]);
				
				//Create cast object
				SetStripper(Credit.start_x, (WINDOW_HEIGHT << 9) + 0x1000, text, len);
				
				//Change offset
				Credit.offset += 4;
				return;
				
			case 'j': //Jump to label
				//Get number
				b = GetScriptNumber(&Credit.pData[++Credit.offset]);
				
				//Change offset
				Credit.offset += 4;
				
				//Jump to specific label
				while (Credit.offset < Credit.size)
				{
					if (Credit.pData[Credit.offset] == 'l')
					{
						//what is this
						a = GetScriptNumber(&Credit.pData[++Credit.offset]);
						Credit.offset += 4;
						if (b == a)
							return;
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
				
				return;
				
			case '~': //Start fading out music
				++Credit.offset;
				//SetOrganyaFadeout();
				return;
				
			case 'f': //Flag jump
				//Read numbers XXXX:YYYY
				a = GetScriptNumber(&Credit.pData[++Credit.offset]);
				Credit.offset += 5;
				b = GetScriptNumber(&Credit.pData[Credit.offset]);
				Credit.offset += 4;
				
				//If flag is set
				if ((uint8_t)GetNPCFlag(a))
				{
					//Jump to label
					while ( Credit.offset < Credit.size )
					{
						if (Credit.pData[Credit.offset] == 'l')
						{
							a = GetScriptNumber(&Credit.pData[++Credit.offset]);
							Credit.offset += 4;
							if ( b == a )
								return;
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
				
			case '+': //Change casts x-position
				Credit.start_x = GetScriptNumber(&Credit.pData[++Credit.offset]) << 9;
				Credit.offset += 4;
				return;
				
			case '-': //Wait for X amount of frames
				Credit.wait = GetScriptNumber(&Credit.pData[++Credit.offset]);
				Credit.offset += 4;
				Credit.mode = 2;
				return;
				
			case '/': //Stop credits
				Credit.mode = 0;
				return;
				
			case '!': //Change music
				a = GetScriptNumber(&Credit.pData[++Credit.offset]);
				Credit.offset += 4;
				//ChangeMusic(a);
				return;
		}
		
		//Progress through file
		++Credit.offset;
	}
}

//Update credits
void ActionCredit()
{
	if (Credit.offset < Credit.size)
	{
		if (Credit.mode == 1)
		{
			ActionCredit_Read();
		}
		else if (Credit.mode == 2 && --Credit.wait <= 0)
		{
			Credit.mode = 1;
		}
	}
}

//Change illustration
void SetCreditIllust(int a)
{
	ReloadIllust(a);
	Illust.act_no = 1;
}

//Slide illustration off-screen
void CutCreditIllust()
{
	Illust.act_no = 2;
}
