#include "Organya.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include "File.h"

#define BOOL BOOL_OGC
#include <aesndlib.h>
#undef BOOL

#include "CommonDefines.h"
#include "Resource.h"
#include "Sound.h"
#include "Tags.h"

#include "WindowsWrapper.h"

#define PANDUMMY 0xFF
#define VOLDUMMY 0xFF
#define KEYDUMMY 0xFF

#define MAXTRACK 16
#define MAXMELODY 8
#define MAXDRAM 8

SOUNDBUFFER* lpORGANBUFFER[8][8][2] = {NULL};
SOUNDBUFFER** lpDRAMBUFFER = &lpSECONDARYBUFFER[0x96];

MUSICINFO info;

int gTrackVol[MAXTRACK];
int gOrgVolume = 100;
bool bFadeout = false;

unsigned int gOrgTimer = 0;
unsigned int gOrgSamplePerStep = 0;

bool OrganyaNoteAlloc(uint16_t alloc)
{
	for(int j = 0; j < MAXTRACK; j++)
	{
		info.tdata[j].wave_no = 0;
		info.tdata[j].note_list = NULL;
		info.tdata[j].note_p = new NOTELIST[alloc];
		
		if(info.tdata[j].note_p == NULL)
		{
			for(int i = 0; i < MAXTRACK; i++)
			{
				if(info.tdata[i].note_p != NULL)
				{
					delete[] info.tdata[i].note_p;
					info.tdata[j].note_p = NULL;	// Uses j instead of i
				}
			}

			return false;
		}

		for(int i = 0; i < alloc; i++)
		{
			(info.tdata[j].note_p + i)->from = NULL;
			(info.tdata[j].note_p + i)->to = NULL;
			(info.tdata[j].note_p + i)->length = 0;
			(info.tdata[j].note_p + i)->pan = PANDUMMY;
			(info.tdata[j].note_p + i)->volume = VOLDUMMY;
			(info.tdata[j].note_p + i)->y = KEYDUMMY;
		}
	}
	
	for(int j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);
	//for(int j = 0; j < MAXDRAM; j++)
	//	InitDramObject(j);

	//this->track = 0;

	return true;
}

void OrganyaReleaseNote()
{
	for(int i = 0; i < MAXTRACK; i++)
	{
		if(info.tdata[i].note_p != NULL)
		{
#ifdef FIX_BUGS
			delete[] info.tdata[i].note_p;
#else
			delete info.tdata[i].note_p;	// should be delete[]
#endif
			info.tdata[i].note_p = NULL;
		}
	}
}

//Wave playing and loading
typedef struct {
	short wave_size;
	short oct_par;
	short oct_size;
} OCTWAVE;

OCTWAVE oct_wave[8] = {
	{ 256,  1,  4 }, //0 Oct
	{ 256,  2,  8 }, //1 Oct
	{ 128,  4, 12 }, //2 Oct
	{ 128,  8, 16 }, //3 Oct
	{  64, 16, 20 }, //4 Oct
	{  32, 32, 24 }, //5 Oct
	{  16, 64, 28 }, //6 Oct
	{   8,128, 32 }, //7 Oct
};

bool MakeSoundObject8(int8_t *wavep, int8_t track, int8_t pipi)
{
	for (int j = 0; j < 8; j++)
	{
		for (int k = 0; k < 2; k++)
		{
			size_t wave_size = oct_wave[j].wave_size;
			size_t data_size = pipi ? wave_size * oct_wave[j].oct_size : wave_size;
			
			//Create sound buffer
			lpORGANBUFFER[track][j][k] = new SOUNDBUFFER(data_size);
			
			//Get wave data
			uint8_t *wp = new uint8_t[data_size];
			uint8_t *wp_sub = wp;
			size_t wav_tp = 0;
			
			for (size_t i = 0; i < data_size; i++)
			{
				uint8_t work = *(wavep+wav_tp);
				work += 0x80;
				
				*wp_sub = work;
				
				wav_tp += 0x100 / wave_size;
				if (wav_tp >= 0x100)
					wav_tp -= 0x100;
				
				wp_sub++;
			}
			
			//Copy wave data to sound buffer
			uint8_t *buf;
			lpORGANBUFFER[track][j][k]->Lock(&buf, NULL);
			memcpy(buf, wp, data_size);
			lpORGANBUFFER[track][j][k]->Unlock();
			lpORGANBUFFER[track][j][k]->SetCurrentPosition(0);
			delete[] wp;
		}
	}
	
	return true;
}

//Playing melody tracks
short freq_tbl[12] = { 262,277,294,311,330,349,370,392,415,440,466,494 };

void ChangeOrganFrequency(uint8_t key, uint8_t track, int32_t a)
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 2; i++) {
			uint32_t tmpDouble = ((oct_wave[j].wave_size * freq_tbl[key]) * oct_wave[j].oct_par) / 8 + (a - 1000);
			lpORGANBUFFER[track][j][i]->SetFrequency(tmpDouble);
		}
	}
}

int16_t pan_tbl[13] = {0, 43, 86, 129, 172, 215, 256, 297, 340, 383, 426, 469, 512}; 
uint8_t old_key[MAXTRACK] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t key_on[MAXTRACK] = {0};
uint8_t key_twin[MAXTRACK] = {0};

void ChangeOrganPan(uint8_t key, uint8_t pan, int8_t track)
{
	if(old_key[track] != PANDUMMY)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetPan((pan_tbl[pan] - 0x100) * 10);
}

void ChangeOrganVolume(int no, int32_t volume, int8_t track)
{
	if(old_key[track] != VOLDUMMY)
		lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->SetVolume((volume - 0xFF) * 8);
}

void PlayOrganObject(uint8_t key, int mode, int8_t track, int32_t freq)
{
	if (lpORGANBUFFER[track][key/12][key_twin[track]] != NULL)
	{
		switch(mode)
		{
			case 0:
				if (old_key[track] != 0xFF)
				{
					lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->Stop();
					lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->SetCurrentPosition(0);
				}
				break;
			
			case 1:
				break;
				
			case 2:
				if (old_key[track] != 0xFF)
				{
					lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->Play(false);
					old_key[track] = 0xFF;
				}
				break;
				
			case -1:
				if (old_key[track] == 0xFF)
				{
					ChangeOrganFrequency(key % 12, track, freq);
					lpORGANBUFFER[track][key / 12][key_twin[track]]->Play(true);
					old_key[track] = key;
					key_on[track] = 1;
				}
				else if (key_on[track] == 1 && old_key[track] == key)
				{
					lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->Play(false);
					key_twin[track]++;
					if(key_twin[track] == 2)
						key_twin[track] = 0; 
					lpORGANBUFFER[track][key / 12][key_twin[track]]->Play(true);
				}
				else
				{
					lpORGANBUFFER[track][old_key[track]/12][key_twin[track]]->Play(false);
					key_twin[track]++;
					if(key_twin[track] == 2)
						key_twin[track] = 0; 
					ChangeOrganFrequency(key % 12, track, freq);
					lpORGANBUFFER[track][key / 12][key_twin[track]]->Play(true);
					old_key[track] = key;
				}
				break;
		}
    }
}

//Release tracks
void ReleaseOrganyaObject(int8_t track)
{
	for(int i = 0; i < 8; i++)
	{
		if(lpORGANBUFFER[track][i][0] != NULL)
		{
			lpORGANBUFFER[track][i][0]->Release();
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if(lpORGANBUFFER[track][i][1] != NULL)
		{
			lpORGANBUFFER[track][i][1]->Release();
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}

//Handling WAVE100
int8_t wave_data[100][0x100];

bool InitWaveData100()
{
	const unsigned char* fp = FindResource("WAVE100", NULL);
	if (fp == NULL)
	{
		printf("Failed to open WAVE100\n");
		return false;
	}

	memcpy(wave_data, fp, 100 * 0x100);
	return true;
}

//Create org wave
bool MakeOrganyaWave(int8_t track, int8_t wave_no, int8_t pipi)
{
	if(wave_no > 99)
	{
		printf("WARNING: track %d has out-of-range wave_no %d\n", track, wave_no);
		return false;
	}
	
	ReleaseOrganyaObject(track);
	MakeSoundObject8(wave_data[wave_no], track, pipi);
	return true;
}

//Dram
void ChangeDramFrequency(uint8_t key, int8_t track)
{
	lpDRAMBUFFER[track]->SetFrequency(key * 800 + 100);
}

void ChangeDramPan(uint8_t pan, int8_t track)
{
	lpDRAMBUFFER[track]->SetPan((pan_tbl[pan] - 0x100) * 10);
}

void ChangeDramVolume(int32_t volume, int8_t track)
{
	lpDRAMBUFFER[track]->SetVolume((volume - 0xFF) * 8);
}

void PlayDramObject(unsigned char key, int mode,char track)
{
	switch(mode)
	{
		case 0:
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			break;
		case 1:
			lpDRAMBUFFER[track]->Stop();
			lpDRAMBUFFER[track]->SetCurrentPosition(0);
			ChangeDramFrequency(key, track);
			lpDRAMBUFFER[track]->Play(false);
			break;
		case 2:
			break;
		case -1:
			break;
	}
}

//Play data
int32_t play_p;
NOTELIST *play_np[MAXTRACK];
int32_t now_leng[MAXMELODY] = {0};

void OrganyaPlayData()
{
	//Handle fading out
	if (bFadeout && gOrgVolume)
		gOrgVolume -= 2;
	if (gOrgVolume < 0)
		gOrgVolume = 0;

	//Play melody
	for(int i = 0; i < MAXMELODY; i++)
	{
		if (play_np[i] != NULL && play_p == play_np[i]->x)
		{
			if(play_np[i]->y != KEYDUMMY)
			{
				PlayOrganObject(play_np[i]->y,-1,i,info.tdata[i].freq);
				now_leng[i] = play_np[i]->length;
			}
			
			if(play_np[i]->pan != PANDUMMY)
				ChangeOrganPan(play_np[i]->y,play_np[i]->pan, i);
			if(play_np[i]->volume != VOLDUMMY)
				gTrackVol[i] = play_np[i]->volume;
			
			play_np[i] = play_np[i]->to;
		}
		
		if (now_leng[i] == 0 )
			PlayOrganObject(0, 2, i, info.tdata[i].freq);
		
		if (now_leng[i] > 0)
			now_leng[i]--;
		
		if (play_np[i])
			ChangeOrganVolume(play_np[i]->y, gOrgVolume * gTrackVol[i] / 0x7F, i);
	}
	
	for(int i = MAXMELODY; i < MAXTRACK; i++)
	{
		if (play_np[i] != NULL && play_p == play_np[i]->x)
		{
			if (play_np[i]->y != KEYDUMMY)
				PlayDramObject(play_np[i]->y,1,i-MAXMELODY);
			
			if(play_np[i]->pan != PANDUMMY)
				ChangeDramPan(play_np[i]->pan,i-MAXMELODY);
			if(play_np[i]->volume != VOLDUMMY)
				gTrackVol[i] = play_np[i]->volume;
			
			play_np[i] = play_np[i]->to;
		}
		
		if (play_np[i])
			ChangeDramVolume(gOrgVolume * gTrackVol[i] / 0x7F, i - MAXMELODY);
	}
	
	//Looping
	play_p++;
	if(play_p >= info.end_x)
	{
		play_p = info.repeat_x;
		SetPlayPointer(play_p);
	}
}

void SetPlayPointer(int32_t x)
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		play_np[i] = info.tdata[i].note_list;
		while (play_np[i] != NULL && play_np[i]->x < x)
			play_np[i] = play_np[i]->to;
	}
	
	play_p = x;
}

//Load organya file
void LoadOrganya(const char *name)
{
	//Unload previous things
	OrganyaReleaseNote();
	memset(&info, 0, sizeof(info));
	OrganyaNoteAlloc(0xFFFF);

	//Stop currently playing notes
	memset(play_np, 0, sizeof(play_np));
	memset(old_key, 0xFF, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));
	memset(now_leng, 0, sizeof(now_leng));

	//Open file
	printf("Loading org %s\n", name);
	
	size_t resSize;
	const unsigned char *res = FindResource(name, &resSize);
	
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s.restemp", gDataPath, name);
	
	FILE *temp = fopen(path, "wb");
	if (!temp)
		return;
	
	fwrite(res, resSize, 1, temp);
	fclose(temp);
	
	FILE *fp = fopen(path, "rb");

	if (!fp)
	{
		printf("Failed to open %s\n", name);
		return;
	}

	//Version Check
	uint8_t ver = 0;
	char pass_check[6];

	fread(&pass_check[0], sizeof(char), 6, fp);

	if (!memcmp(pass_check, "Org-01", 6))ver = 1;
	if (!memcmp(pass_check, "Org-02", 6))ver = 2;
	//if (!memcmp(pass_check, "Org-03", 6))ver = 2;

	if (!ver)
	{
		printf("Failed to open.org, invalid version %s", pass_check);
		return;
	}
	
	//Set song information
	info.wait = File_ReadLE16(fp);
	info.line = File_ReadU8(fp);
	info.dot = File_ReadU8(fp);
	info.repeat_x = File_ReadLE32(fp);
	info.end_x = File_ReadLE32(fp);
	
	for (int i = 0; i < 16; i++) {
		info.tdata[i].freq = File_ReadLE16(fp);
		info.tdata[i].wave_no = File_ReadU8(fp);
		const int8_t pipi = File_ReadU8(fp);
		info.tdata[i].pipi = ver == 1 ? 0 : pipi;
		info.tdata[i].note_num = File_ReadLE16(fp);
	}

	//Load notes
	NOTELIST *np;

	for (int j = 0; j < 16; j++) {
		//The first note from is NULL
		if (info.tdata[j].note_num == 0) {
			info.tdata[j].note_list = NULL;
			continue;
		}

		//Make note list
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;

		for (int i = 1; i < info.tdata[j].note_num; i++) {
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}

		//The last note to is NULL
		np--;
		np->to = NULL;

		//Set note properties
		np = info.tdata[j].note_p; //X position
		for (int i = 0; i < info.tdata[j].note_num; i++) {
			np->x = File_ReadLE32(fp);
			np++;
		}

		np = info.tdata[j].note_p; //Y position
		for (int i = 0; i < info.tdata[j].note_num; i++) {
			np->y = File_ReadU8(fp);
			np++;
		}

		np = info.tdata[j].note_p; //Length
		for (int i = 0; i < info.tdata[j].note_num; i++) {
			np->length = File_ReadU8(fp);
			np++;
		}

		np = info.tdata[j].note_p; //Volume
		for (int i = 0; i < info.tdata[j].note_num; i++) {
			np->volume = File_ReadU8(fp);
			np++;
		}

		np = info.tdata[j].note_p; //Pan
		for (int i = 0; i < info.tdata[j].note_num; i++) {
			np->pan = File_ReadU8(fp);
			np++;
		}
	}

	fclose(fp);
	remove(path);

	//Create waves
	for (int j = 0; j < 8; j++)
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);

	//Reset position
	SetPlayPointer(0);
	
	//Set as loaded
	info.loaded = true;
}

void SetOrganyaPosition(unsigned int x)
{
	SetPlayPointer(x);
	gOrgVolume = 100;
	bFadeout = false;
}

unsigned int GetOrganyaPosition()
{
	return play_p;
}

void PlayOrganyaMusic()
{
	//Start timer
	gOrgTimer = 0;
	gOrgSamplePerStep = info.wait * DSP_DEFAULT_FREQ / 1000;
}

bool ChangeOrganyaVolume(signed int volume)
{
	if (volume >= 0 && volume <= 100)
	{
		gOrgVolume = volume;
		return true;
	}
	
	return false;
}

void StopOrganyaMusic()
{
	//Stop timer
	gOrgTimer = 0;
	gOrgSamplePerStep = -1;
	
	//Stop notes
	for (int i = 0; i < MAXMELODY; i++)
		PlayOrganObject(0, 2, i, 0);
	
	memset(old_key, 255, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));
}

void SetOrganyaFadeout()
{
	bFadeout = true;
}

//Org timer
/*
SDL_Thread *OrganyaTimer = NULL;
bool bEndTimer = false;

int OrganyaPlayTimer(void *ptr)
{
	SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);
	
	//Set time for next step to play
	uint32_t NextTick = SDL_GetTicks() + info.wait;
	
	while (bEndTimer == false)
	{
		if (info.loaded)
		{
			//Play music
			OrganyaPlayData();
			
			//Wait until this step is over
			while (NextTick > SDL_GetTicks())
				SDL_Delay(1);
			
			//Get time for next step to play
			while (NextTick <= SDL_GetTicks())
				NextTick += info.wait;
		}
		else
		{
			//Wait until the org is loaded
			SDL_Delay(1);
		}
	}
	
	return 0;
}

void OrganyaStartTimer(unsigned int wait)
{
	OrganyaEndTimer();
	bEndTimer = false;
	OrganyaTimer = SDL_CreateThread(OrganyaPlayTimer, "OrganyaPlayTimer", (void*)NULL);
}

void OrganyaEndTimer()
{
	bEndTimer = true; //Tell thread to end
	SDL_WaitThread(OrganyaTimer, NULL); //Wait for thread to end
	OrganyaTimer = NULL;
}
*/

//Start and end organya
void StartOrganya()
{
	//Initialize org stuff
	InitWaveData100();
}

void EndOrganya()
{
	//Release everything related to org
	OrganyaReleaseNote();
	
	for (int i = 0; i < MAXMELODY; i++)
		ReleaseOrganyaObject(i);
}
