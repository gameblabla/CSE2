#include <stdint.h>
#include <string>

#include "CommonDefines.h"
#include "TextScr.h"
#include "Draw.h"
#include "Tags.h"
#include "Game.h"

#define TSC_BUFFER_SIZE 0x5000

TEXT_SCRIPT gTS;

int gNumberTextScript[4];
char text[0x100];

const RECT gRect_line = {0, 0, 216, 16};

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
