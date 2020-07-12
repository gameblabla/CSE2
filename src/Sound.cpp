// Some of the original source code for this file can be found here:
// https://github.com/shbow/organya/blob/master/source/Sound.cpp

/*
TODO - Code style
Pixel's code was *extremely* Windows-centric, to the point of using
things like ZeroMemory and LPCSTR instead of standard things like
memset and const char*. For now, the decompilation is accurate despite
not using these since they're just macros that evaluate to the portable
equivalents.
*/

#include "Sound.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "WindowsWrapper.h"

#include "Backends/Audio.h"
#ifdef EXTRA_SOUND_FORMATS
#include "ExtraSoundFormats.h"
#endif
#include "Main.h"
#include "Organya.h"
#include "PixTone.h"
#include "Resource.h"

BOOL audio_backend_initialised;
AudioBackend_Sound *lpSECONDARYBUFFER[SE_MAX];

// DirectSoundの開始 (Starting DirectSound)
BOOL InitDirectSound(void)
{
	int i;

	audio_backend_initialised = AudioBackend_Init();

	if (!audio_backend_initialised)
	{
#ifndef FIX_BUGS
		// This makes absolutely no sense here
		StartOrganya("Org/Wave.dat");
#endif
		return FALSE;
	}

	for (i = 0; i < SE_MAX; i++)
		lpSECONDARYBUFFER[i] = NULL;

	StartOrganya("Org/Wave.dat");

	return TRUE;
}

// DirectSoundの終了 (Exit DirectSound)
void EndDirectSound(void)
{
	int i;

	if (!audio_backend_initialised)
		return;

	EndOrganya();

	for (i = 0; i < SE_MAX; i++)
		if (lpSECONDARYBUFFER[i] != NULL)
			AudioBackend_DestroySound(lpSECONDARYBUFFER[i]);

	AudioBackend_Deinit();
}

// Below are two completely unused functions for loading .wav files as sound effects.
// Some say that sounds heard in CS Beta footage don't sound like PixTone...

// There's a bit of a problem with this code: it hardcodes the offsets of various bits
// of data in the WAV header - this makes the code only compatible with very specific
// .wav files. You can check the prototype OrgView EXEs for examples of those.

// サウンドの設定 (Sound settings)
BOOL InitSoundObject(const char *resname, int no)
{
	const unsigned char *resource_pointer;	// リソースのアドレス (Resource address)

	if (!audio_backend_initialised)
		return TRUE;

	// リソースの検索 (Search for resources)
	resource_pointer = FindResource(resname, "WAVE", NULL);

	if (resource_pointer == NULL)
		return FALSE;

	// Get sound properties, and check if it's valid
	unsigned long buffer_size = resource_pointer[0x36] | (resource_pointer[0x37] << 8) | (resource_pointer[0x38] << 16) | (resource_pointer[0x39] << 24);
	unsigned short format = resource_pointer[0x14] | (resource_pointer[0x15] << 8);
	unsigned short channels = resource_pointer[0x16] | (resource_pointer[0x17] << 8);
	unsigned long sample_rate = resource_pointer[0x18] | (resource_pointer[0x19] << 8) | (resource_pointer[0x1A] << 16) | (resource_pointer[0x1B] << 24);
	unsigned short bits_per_sample = resource_pointer[0x22] | (resource_pointer[0x23] << 8);

	if (format != 1)	// 1 is WAVE_FORMAT_PCM
		return FALSE;

	if (channels != 1)	// The mixer only supports mono right now
		return FALSE;

	if (bits_per_sample != 8)	// The mixer only supports unsigned 8-bit samples
		return FALSE;

	// 二次バッファの生成 (Create secondary buffer)
	lpSECONDARYBUFFER[no] = AudioBackend_CreateSound(sample_rate, resource_pointer + 0x3A, buffer_size);

	if (lpSECONDARYBUFFER[no] == NULL)
		return FALSE;

	return TRUE;
}

BOOL LoadSoundObject(const char *file_name, int no)
{
	std::string path;
	//unsigned long i;
	unsigned long file_size = 0;
	char check_box[58];
	FILE *fp;

	path = gModulePath + '/' + file_name;

	if (!audio_backend_initialised)
		return TRUE;

	if ((fp = fopen(path.c_str(), "rb")) == NULL)
		return FALSE;

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);

	// Let's not throttle disk I/O, shall we...
	//for (i = 0; i < 58; i++)
	//	fread(&check_box[i], sizeof(char), 1, fp);	// Holy hell, this is inefficient
	fread(check_box, 1, 58, fp);

#ifdef FIX_BUGS
	// The original code forgets to close 'fp'
	if (check_box[0] != 'R' || check_box[1] != 'I' || check_box[2] != 'F' || check_box[3] != 'F')
	{
		fclose(fp);
		return FALSE;
	}
#else
	if (check_box[0] != 'R')
		return FALSE;
	if (check_box[1] != 'I')
		return FALSE;
	if (check_box[2] != 'F')
		return FALSE;
	if (check_box[3] != 'F')
		return FALSE;
#endif

	unsigned char *wp;
	wp = (unsigned char*)malloc(file_size);	// ファイルのワークスペースを作る (Create a file workspace)

#ifdef FIX_BUGS
	if (wp == NULL)
	{
		fclose(fp);
		return FALSE;
	}
#endif

	fseek(fp, 0, SEEK_SET);

	// Bloody hell, Pixel, come on...
	//for (i = 0; i < file_size; i++)
	//	fread((BYTE*)wp+i, sizeof(char), 1, fp);	// Pixel, stahp
	fread(wp, 1, file_size, fp);

	fclose(fp);

	// Get sound properties, and check if it's valid
	unsigned long buffer_size = wp[0x36] | (wp[0x37] << 8) | (wp[0x38] << 16) | (wp[0x39] << 24);
	unsigned short format = wp[0x14] | (wp[0x15] << 8);
	unsigned short channels = wp[0x16] | (wp[0x17] << 8);
	unsigned long sample_rate = wp[0x18] | (wp[0x19] << 8) | (wp[0x1A] << 16) | (wp[0x1B] << 24);
	unsigned short bits_per_sample = wp[0x22] | (wp[0x23] << 8);

	if (format != 1)	// 1 is WAVE_FORMAT_PCM
	{
		free(wp);
		return FALSE;
	}

	if (channels != 1)	// The mixer only supports mono right now
	{
		free(wp);
		return FALSE;
	}

	if (bits_per_sample != 8)	// The mixer only supports 8-bit unsigned samples
	{
		free(wp);
		return FALSE;
	}

	// セカンダリバッファの生成 (Create secondary buffer)
	lpSECONDARYBUFFER[no] = AudioBackend_CreateSound(sample_rate, wp + 0x3A, buffer_size);

	if (lpSECONDARYBUFFER[no] == NULL)
	{
#ifdef FIX_BUGS
		free(wp);	// The updated Organya source code includes this fix
#endif
		return FALSE;	
	}
	
	free(wp);

	return TRUE;
}

void PlaySoundObject(int no, SoundMode mode)
{
	if (!audio_backend_initialised)
		return;

	if (lpSECONDARYBUFFER[no] != NULL)
	{
		switch (mode)
		{
			case SOUND_MODE_STOP:	// 停止 (Stop)
				AudioBackend_StopSound(lpSECONDARYBUFFER[no]);
				break;

			case SOUND_MODE_PLAY:	// 再生 (Playback)
				AudioBackend_StopSound(lpSECONDARYBUFFER[no]);
				AudioBackend_RewindSound(lpSECONDARYBUFFER[no]);
				AudioBackend_PlaySound(lpSECONDARYBUFFER[no], FALSE);
				break;

			case SOUND_MODE_PLAY_LOOP:// ループ再生 (Loop playback)
				AudioBackend_PlaySound(lpSECONDARYBUFFER[no], TRUE);
				break;
		}
	}
#ifdef EXTRA_SOUND_FORMATS
	else
	{
		ExtraSound_PlaySFX(no, mode);
	}
#endif
}

void ChangeSoundFrequency(int no, unsigned long rate)	// 100がMIN9999がMAXで2195?がﾉｰﾏﾙ (100 is MIN, 9999 is MAX, and 2195 is normal)
{
	if (!audio_backend_initialised)
		return;

	if (lpSECONDARYBUFFER[no] != NULL)
		AudioBackend_SetSoundFrequency(lpSECONDARYBUFFER[no], (rate * 10) + 100);
#ifdef EXTRA_SOUND_FORMATS
	else
		ExtraSound_SetSFXFrequency(no, (rate * 10) + 100);
#endif
}

void ChangeSoundVolume(int no, long volume)	// 300がMAXで300がﾉｰﾏﾙ (300 is MAX and 300 is normal)
{
	if (!audio_backend_initialised)
		return;

	if (lpSECONDARYBUFFER[no] != NULL)
		AudioBackend_SetSoundVolume(lpSECONDARYBUFFER[no], (volume - 300) * 8);
#ifdef EXTRA_SOUND_FORMATS
	else
		ExtraSound_SetSFXVolume(no, (volume - 300) * 8);
#endif
}

void ChangeSoundPan(int no, long pan)	// 512がMAXで256がﾉｰﾏﾙ (512 is MAX and 256 is normal)
{
	if (!audio_backend_initialised)
		return;

	if (lpSECONDARYBUFFER[no] != NULL)
		AudioBackend_SetSoundPan(lpSECONDARYBUFFER[no], (pan - 256) * 10);
#ifdef EXTRA_SOUND_FORMATS
	else
		ExtraSound_SetSFXPan(no, (pan - 256) * 10);
#endif
}

// TODO - The stack frame for this function is inaccurate
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no)
{
	int sample_count;
	int i, j;
	const PIXTONEPARAMETER *ptp_pointer;
	unsigned char *pcm_buffer;
	unsigned char *mixed_pcm_buffer;

	if (!audio_backend_initialised)
		return 0;

	ptp_pointer = ptp;
	sample_count = 0;

	for (i = 0; i < ptp_num; i++)
	{
		if (ptp_pointer->size > sample_count)
			sample_count = ptp_pointer->size;

		++ptp_pointer;
	}

	pcm_buffer = mixed_pcm_buffer = NULL;

	pcm_buffer = (unsigned char*)malloc(sample_count);
	mixed_pcm_buffer = (unsigned char*)malloc(sample_count);

	if (pcm_buffer == NULL || mixed_pcm_buffer == NULL)
	{
		if (pcm_buffer != NULL)
			free(pcm_buffer);

		if (mixed_pcm_buffer != NULL)
			free(mixed_pcm_buffer);

		return -1;
	}

	memset(pcm_buffer, 0x80, sample_count);
	memset(mixed_pcm_buffer, 0x80, sample_count);

	ptp_pointer = ptp;

	for (i = 0; i < ptp_num; i++)
	{
		if (!MakePixelWaveData(ptp_pointer, pcm_buffer))
		{
			if (pcm_buffer != NULL) // This is always true
				free(pcm_buffer);

			if (mixed_pcm_buffer != NULL) // This is always true
				free(mixed_pcm_buffer);

			return -1;
		}

		for (j = 0; j < ptp_pointer->size; j++)
		{
			if (pcm_buffer[j] + mixed_pcm_buffer[j] - 0x100 < -0x7F)
				mixed_pcm_buffer[j] = 0;
			else if (pcm_buffer[j] + mixed_pcm_buffer[j] - 0x100 > 0x7F)
				mixed_pcm_buffer[j] = 0xFF;
			else
				mixed_pcm_buffer[j] = mixed_pcm_buffer[j] + pcm_buffer[j] - 0x80;
		}

		++ptp_pointer;
	}

	// This is self-assignment, so redundant. Maybe this used to be something to prevent audio popping ?
	mixed_pcm_buffer[0] = mixed_pcm_buffer[0];
	mixed_pcm_buffer[sample_count - 1] = mixed_pcm_buffer[sample_count - 1];

	lpSECONDARYBUFFER[no] = AudioBackend_CreateSound(22050, mixed_pcm_buffer, sample_count);

	if (pcm_buffer != NULL)
		free(pcm_buffer);

	if (mixed_pcm_buffer != NULL)
		free(mixed_pcm_buffer);

	if (lpSECONDARYBUFFER[no] == NULL)
		return -1;

	return sample_count;
}
