// Some of the original source code for this file can be found here:
// https://github.com/shbow/organya/blob/master/source/OrgFile.cpp
// https://github.com/shbow/organya/blob/master/source/OrgPlay.cpp
// https://github.com/shbow/organya/blob/master/source/Sound.cpp
// https://github.com/shbow/organya/blob/master/source/WinTimer.cpp

#include "Organya.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "Resource.h"
#include "Sound.h"

#define PANDUMMY 0xFF
#define VOLDUMMY 0xFF
#define KEYDUMMY 0xFF

#define MAXTRACK 16
#define MAXMELODY 8
#define MAXDRAM 8

SOUNDBUFFER *lpORGANBUFFER[8][8][2] = {NULL};
SOUNDBUFFER **lpDRAMBUFFER = &lpSECONDARYBUFFER[0x96];

MUSICINFO info;

int gTrackVol[MAXTRACK];
int gOrgVolume = 100;
BOOL bFadeout = FALSE;

unsigned int gOrgTimer;
unsigned int gOrgWait = -1;

BOOL OrganyaNoteAlloc(unsigned short alloc)
{
	for (int j = 0; j < MAXTRACK; j++)
	{
		info.tdata[j].wave_no = 0;
		info.tdata[j].note_list = NULL;
		info.tdata[j].note_p = new NOTELIST[alloc];

		if (info.tdata[j].note_p == NULL)
		{
			for (int i = 0; i < MAXTRACK; i++)
			{
				if (info.tdata[i].note_p != NULL)
				{
					delete[] info.tdata[i].note_p;
					info.tdata[j].note_p = NULL;	// Uses j instead of i
				}
			}

			return FALSE;
		}

		for (int i = 0; i < alloc; i++)
		{
			(info.tdata[j].note_p + i)->from = NULL;
			(info.tdata[j].note_p + i)->to = NULL;
			(info.tdata[j].note_p + i)->length = 0;
			(info.tdata[j].note_p + i)->pan = PANDUMMY;
			(info.tdata[j].note_p + i)->volume = VOLDUMMY;
			(info.tdata[j].note_p + i)->y = KEYDUMMY;
		}
	}

	for (int j = 0; j < MAXMELODY; j++)
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);
	// for(int j = 0; j < MAXDRAM; j++)
	// 	InitDramObject(j);

	// this->track = 0;

	return FALSE;
}

void OrganyaReleaseNote()
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		if (info.tdata[i].note_p != NULL)
		{
#ifdef FIX_BUGS
			delete[] info.tdata[i].note_p;
#else
			delete info.tdata[i].note_p;	// Should be delete[]
#endif
			info.tdata[i].note_p = NULL;
		}
	}
}

// Wave playing and loading
typedef struct
{
	short wave_size;
	short oct_par;
	short oct_size;
} OCTWAVE;

OCTWAVE oct_wave[8] =
{
	{ 256,  1,  4 }, // 0 Oct
	{ 256,  2,  8 }, // 1 Oct
	{ 128,  4, 12 }, // 2 Oct
	{ 128,  8, 16 }, // 3 Oct
	{  64, 16, 20 }, // 4 Oct
	{  32, 32, 24 }, // 5 Oct
	{  16, 64, 28 }, // 6 Oct
	{   8,128, 32 }, // 7 Oct
};

BOOL MakeSoundObject8(signed char *wavep, signed char track, signed char pipi)
{
	for (int j = 0; j < 8; j++)
	{
		for (int k = 0; k < 2; k++)
		{
			size_t wave_size = oct_wave[j].wave_size;
			size_t data_size = pipi ? wave_size * oct_wave[j].oct_size : wave_size;

			// Get wave data
			unsigned char *wp = new unsigned char[data_size];
			unsigned char *wp_sub = wp;
			size_t wav_tp = 0;

			for (size_t i = 0; i < data_size; i++)
			{
				unsigned char work = *(wavep + wav_tp);
				work += 0x80;

				*wp_sub = work;

				wav_tp += 0x100 / wave_size;
				if (wav_tp >= 0x100)
					wav_tp -= 0x100;

				wp_sub++;
			}

			// Create sound buffer
			lpORGANBUFFER[track][j][k] = new SOUNDBUFFER(22050, wp, data_size);
			delete[] wp;
		}
	}

	return TRUE;
}

// Playing melody tracks
short freq_tbl[12] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};

void ChangeOrganFrequency(unsigned char key, signed char track, long a)
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			lpORGANBUFFER[track][j][i]->SetFrequency(((oct_wave[j].wave_size * freq_tbl[key]) * oct_wave[j].oct_par) / 8 + (a - 1000));
		}
	}
}

short pan_tbl[13] = {0, 43, 86, 129, 172, 215, 256, 297, 340, 383, 426, 469, 512};
unsigned char old_key[MAXTRACK] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char key_on[MAXTRACK] = {0};
unsigned char key_twin[MAXTRACK] = {0};

void ChangeOrganPan(unsigned char key, unsigned char pan, signed char track)
{
	if (old_key[track] != PANDUMMY)
		lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->SetPan((pan_tbl[pan] - 0x100) * 10);
}

void ChangeOrganVolume(int no, long volume, signed char track)
{
	if (old_key[track] != VOLDUMMY)
		lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->SetVolume((volume - 0xFF) * 8);
}

void PlayOrganObject(unsigned char key, int mode, signed char track, long freq)
{
	if (lpORGANBUFFER[track][key / 12][key_twin[track]] != NULL)
	{
		switch (mode)
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
					if (key_twin[track] == 2)
						key_twin[track] = 0;
					lpORGANBUFFER[track][key / 12][key_twin[track]]->Play(true);
				}
				else
				{
					lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]->Play(false);
					key_twin[track]++;
					if (key_twin[track] == 2)
						key_twin[track] = 0;
					ChangeOrganFrequency(key % 12, track, freq);
					lpORGANBUFFER[track][key / 12][key_twin[track]]->Play(true);
					old_key[track] = key;
				}
				break;
		}
	}
}

// Release tracks
void ReleaseOrganyaObject(signed char track)
{
	for (int i = 0; i < 8; i++)
	{
		if (lpORGANBUFFER[track][i][0] != NULL)
		{
			lpORGANBUFFER[track][i][0]->Release();
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if (lpORGANBUFFER[track][i][1] != NULL)
		{
			lpORGANBUFFER[track][i][1]->Release();
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}

// Handling WAVE100
signed char wave_data[100][0x100];

BOOL InitWaveData100()
{
	const unsigned char *data = FindResource("WAVE100", "WAVE", NULL);

	if (data == NULL)
		return FALSE;

	memcpy(wave_data, data, 100 * 0x100);

	return TRUE;
}

// Create org wave
BOOL MakeOrganyaWave(signed char track, signed char wave_no, signed char pipi)
{
	if (wave_no > 99)
	{
		printf("WARNING: track %d has out-of-range wave_no %d\n", track, wave_no);
		return FALSE;
	}

	ReleaseOrganyaObject(track);
	MakeSoundObject8(wave_data[wave_no], track, pipi);
	return TRUE;
}

// Dram
void ChangeDramFrequency(unsigned char key, signed char track)
{
	lpDRAMBUFFER[track]->SetFrequency(key * 800 + 100);
}

void ChangeDramPan(unsigned char pan, signed char track)
{
	lpDRAMBUFFER[track]->SetPan((pan_tbl[pan] - 0x100) * 10);
}

void ChangeDramVolume(long volume, signed char track)
{
	lpDRAMBUFFER[track]->SetVolume((volume - 0xFF) * 8);
}

void PlayDramObject(unsigned char key, int mode, signed char track)
{
	switch (mode)
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

// Play data
long play_p;
NOTELIST *play_np[MAXTRACK];
long now_leng[MAXMELODY] = {0};

void OrganyaPlayData()
{
	// Handle fading out
	if (bFadeout && gOrgVolume)
		gOrgVolume -= 2;
	if (gOrgVolume < 0)
		gOrgVolume = 0;

	// Play melody
	for (int i = 0; i < MAXMELODY; i++)
	{
		if (play_np[i] != NULL && play_p == play_np[i]->x)
		{
			if (play_np[i]->y != KEYDUMMY)
			{
				PlayOrganObject(play_np[i]->y, -1, i, info.tdata[i].freq);
				now_leng[i] = play_np[i]->length;
			}

			if (play_np[i]->pan != PANDUMMY)
				ChangeOrganPan(play_np[i]->y, play_np[i]->pan, i);
			if (play_np[i]->volume != VOLDUMMY)
				gTrackVol[i] = play_np[i]->volume;

			play_np[i] = play_np[i]->to;
		}

		if (now_leng[i] == 0)
			PlayOrganObject(0, 2, i, info.tdata[i].freq);

		if (now_leng[i] > 0)
			now_leng[i]--;

		if (play_np[i])
			ChangeOrganVolume(play_np[i]->y, gOrgVolume * gTrackVol[i] / 0x7F, i);
	}

	for (int i = MAXMELODY; i < MAXTRACK; i++)
	{
		if (play_np[i] != NULL && play_p == play_np[i]->x)
		{
			if (play_np[i]->y != KEYDUMMY)
				PlayDramObject(play_np[i]->y, 1, i - MAXMELODY);

			if (play_np[i]->pan != PANDUMMY)
				ChangeDramPan(play_np[i]->pan, i - MAXMELODY);
			if (play_np[i]->volume != VOLDUMMY)
				gTrackVol[i] = play_np[i]->volume;

			play_np[i] = play_np[i]->to;
		}

		if (play_np[i])
			ChangeDramVolume(gOrgVolume * gTrackVol[i] / 0x7F, i - MAXMELODY);
	}

	// Looping
	play_p++;
	if (play_p >= info.end_x)
	{
		play_p = info.repeat_x;
		SetPlayPointer(play_p);
	}
}

void SetPlayPointer(long x)
{
	for (int i = 0; i < MAXTRACK; i++)
	{
		play_np[i] = info.tdata[i].note_list;
		while (play_np[i] != NULL && play_np[i]->x < x)
			play_np[i] = play_np[i]->to;
	}

	play_p = x;
}

#define READ_LE16(pointer) pointer[0] | (pointer[1] << 8); pointer += 2;
#define READ_LE32(pointer) pointer[0] | (pointer[1] << 8) | (pointer[2] << 16) | (pointer[3] << 24); pointer += 4;

// Load organya file
void LoadOrganya(const char *name)
{
	// Unload previous things
	OrganyaReleaseNote();
	memset(&info, 0, sizeof(info));
	OrganyaNoteAlloc(0xFFFF);

	// Stop currently playing notes
	memset(play_np, 0, sizeof(play_np));
	memset(old_key, 0xFF, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));
	memset(now_leng, 0, sizeof(now_leng));

	// Open file
	printf("Loading org %s\n", name);
	const unsigned char *p = FindResource(name, "ORG", NULL);

	// Version Check
	unsigned char ver = 0;
	char pass_check[6];

	memcpy(pass_check, p, 6);
	p += 6;

	if (!memcmp(pass_check, "Org-01", 6))
		ver = 1;

	if (!memcmp(pass_check, "Org-02", 6))
		ver = 2;

	// if (!memcmp(pass_check, "Org-03", 6))
	// 	ver = 2;

	if (!ver)
	{
		printf("Failed to open .org, invalid version %s", pass_check);
		return;
	}

	// Set song information
	info.wait = READ_LE16(p);
	info.line = *p++;
	info.dot = *p++;
	info.repeat_x = READ_LE32(p);
	info.end_x = READ_LE32(p);

	for (int i = 0; i < 16; i++)
	{
		info.tdata[i].freq = READ_LE16(p);
		info.tdata[i].wave_no = *p++;
		const signed char pipi = *p++;
		info.tdata[i].pipi = ver == 1 ? 0 : pipi;
		info.tdata[i].note_num = READ_LE16(p);
	}

	// Load notes
	NOTELIST *np;

	for (int j = 0; j < 16; j++)
	{
		// The first note from is NULL
		if (info.tdata[j].note_num == 0)
		{
			info.tdata[j].note_list = NULL;
			continue;
		}

		// Make note list
		np = info.tdata[j].note_p;
		info.tdata[j].note_list = info.tdata[j].note_p;
		np->from = NULL;
		np->to = (np + 1);
		np++;

		for (int i = 1; i < info.tdata[j].note_num; i++)
		{
			np->from = (np - 1);
			np->to = (np + 1);
			np++;
		}

		// The last note to is NULL
		np--;
		np->to = NULL;

		// Set note properties
		np = info.tdata[j].note_p; // X position
		for (int i = 0; i < info.tdata[j].note_num; i++)
		{
			np->x = READ_LE32(p);
			np++;
		}

		np = info.tdata[j].note_p; // Y position
		for (int i = 0; i < info.tdata[j].note_num; i++)
		{
			np->y = *p++;
			np++;
		}

		np = info.tdata[j].note_p; // Length
		for (int i = 0; i < info.tdata[j].note_num; i++)
		{
			np->length = *p++;
			np++;
		}

		np = info.tdata[j].note_p; // Volume
		for (int i = 0; i < info.tdata[j].note_num; i++)
		{
			np->volume = *p++;
			np++;
		}

		np = info.tdata[j].note_p; // Pan
		for (int i = 0; i < info.tdata[j].note_num; i++)
		{
			np->pan = *p++;
			np++;
		}
	}

	// Create waves
	for (int j = 0; j < 8; j++)
		MakeOrganyaWave(j, info.tdata[j].wave_no, info.tdata[j].pipi);

	// Reset position
	SetPlayPointer(0);

	// Set as loaded
	info.loaded = TRUE;
}

void SetOrganyaPosition(unsigned int x)
{
	SetPlayPointer(x);
	gOrgVolume = 100;
	bFadeout = FALSE;
}

unsigned int GetOrganyaPosition()
{
	return play_p;
}

void PlayOrganyaMusic()
{
	// Start timer
	//OrganyaStartTimer(info.wait);
	gOrgTimer = 0;
	gOrgWait = info.wait;
}

BOOL ChangeOrganyaVolume(signed int volume)
{
	if (volume >= 0 && volume <= 100)
	{
		gOrgVolume = volume;
		return TRUE;
	}

	return FALSE;
}

void StopOrganyaMusic()
{
	// Stop timer
	//OrganyaEndTimer();
	gOrgWait = -1;

	// Stop notes
	for (int i = 0; i < MAXMELODY; i++)
		PlayOrganObject(0, 2, i, 0);

	memset(old_key, 255, sizeof(old_key));
	memset(key_on, 0, sizeof(key_on));
	memset(key_twin, 0, sizeof(key_twin));
}

void SetOrganyaFadeout()
{
	bFadeout = TRUE;
}

/* Emscripten doesn't support threads, so we update Organya in the audio callback instead (see Sound.cpp)

// Org timer
SDL_Thread *OrganyaTimer = NULL;
BOOL bEndTimer = FALSE;

int OrganyaPlayTimer(void *ptr)
{
	SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

	// Set time for next step to play
	Uint32 NextTick = SDL_GetTicks() + info.wait;

	while (bEndTimer == FALSE)
	{
		if (info.loaded)
		{
			// Play music
			OrganyaPlayData();

			// Wait until this step is over
			while (NextTick > SDL_GetTicks())
				SDL_Delay(1);

			// Get time for next step to play
			while (NextTick <= SDL_GetTicks())
				NextTick += info.wait;
		}
		else
		{
			// Wait until the org is loaded
			SDL_Delay(1);
		}
	}

	return 0;
}

void OrganyaStartTimer(unsigned int wait)
{
	OrganyaEndTimer();
	bEndTimer = FALSE;
	OrganyaTimer = SDL_CreateThread(OrganyaPlayTimer, "OrganyaPlayTimer", (void*)NULL);
}

void OrganyaEndTimer()
{
	bEndTimer = TRUE; // Tell thread to end
	SDL_WaitThread(OrganyaTimer, NULL); // Wait for thread to end
	OrganyaTimer = NULL;
}
*/
// Start and end organya
void StartOrganya()
{
	// Initialize org stuff
	InitWaveData100();
}

void EndOrganya()
{
	// End timer
	//OrganyaEndTimer();
	gOrgWait = -1;

	// Release everything related to org
	OrganyaReleaseNote();

	for (int i = 0; i < MAXMELODY; i++)
		ReleaseOrganyaObject(i);
}
