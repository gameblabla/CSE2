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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Backends/Audio.h"
#include "Organya.h"
#include "PixTone.h"
#include "Tags.h"

#define FREQUENCY 44100

AudioBackend_Sound *lpSECONDARYBUFFER[SE_MAX];

// DirectSoundの開始 (Starting DirectSound)
BOOL InitDirectSound(void)
{
	int i;

	if (!AudioBackend_Init())
		return FALSE;

	for (i = 0; i < SE_MAX; i++)
		lpSECONDARYBUFFER[i] = NULL;

	StartOrganya("Org\\Wave.dat");

	return TRUE;
}

// DirectSoundの終了 (Exit DirectSound)
void EndDirectSound(void)
{
	int i;

	EndOrganya();

	for (i = 0; i < SE_MAX; i++)
		if (lpSECONDARYBUFFER[i] != NULL)
			AudioBackend_DestroySound(lpSECONDARYBUFFER[i]);

	AudioBackend_Deinit();
}
/*
// サウンドの設定 (Sound settings)
BOOL InitSoundObject(LPCSTR resname, int no)
{
	HRSRC hrscr;
	DSBUFFERDESC dsbd;
	DWORD *lpdword;	// リソースのアドレス (Resource address)

	if (lpDS == NULL)
		return TRUE;

	// リソースの検索 (Search for resources)
	if ((hrscr = FindResourceA(NULL, resname, "WAVE")) == NULL)
		return FALSE;

	// リソースのアドレスを取得 (Get resource address)
	lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));

	// 二次バッファの生成 (Create secondary buffer)
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);	// WAVEデータのサイズ (WAVE data size)
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 

	if (lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no], NULL) != DS_OK)
		return FALSE;

	LPVOID lpbuf1, lpbuf2;
	DWORD dwbuf1, dwbuf2;

	// 二次バッファのロック (Secondary buffer lock)
	lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36), &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 

	// 音源データの設定 (Sound source data settings)
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3A, dwbuf1);

	if (dwbuf2 != 0)
		CopyMemory(lpbuf2, (BYTE*)lpdword+0x3A+dwbuf1, dwbuf2);

	// 二次バッファのロック解除 (Unlock secondary buffer)
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

	return TRUE;
}

BOOL LoadSoundObject(LPCSTR file_name, int no)
{
	char path[MAX_PATH];
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	HANDLE hFile;

	sprintf(path, "%s\\%s", gModulePath, file_name);

	if (lpDS == NULL)
		return TRUE;

	hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	file_size = GetFileSize(hFile, NULL);
	CloseHandle(hFile);

	if ((fp = fopen(path, "rb")) == NULL)
		return FALSE;

	for (i = 0; i < 58; i++)
		fread(&check_box[i], sizeof(char), 1, fp);

	if (check_box[0] != 'R')
		return FALSE;
	if (check_box[1] != 'I')
		return FALSE;
	if (check_box[2] != 'F')
		return FALSE;
	if (check_box[3] != 'F')
		return FALSE;

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);	// ファイルのワークスペースを作る (Create a file workspace)
	fseek(fp, 0, SEEK_SET);

	for (i = 0; i < file_size; i++)
		fread((BYTE*)wp+i, sizeof(BYTE), 1, fp);

	fclose(fp);

	// セカンダリバッファの生成 (Create secondary buffer)
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);	// WAVEデータのサイズ (WAVE data size)
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 

	if (lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no], NULL) != DS_OK)
	{
#ifdef FIX_BUGS
		free(wp);	// The updated Organya source code includes this fix
#endif
		return FALSE;	
	}

	LPVOID lpbuf1, lpbuf2;
	DWORD dwbuf1, dwbuf2;

	HRESULT hr;
	hr = lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)wp+0x36), &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);

	if (hr != DS_OK)
	{
#ifdef FIX_BUGS
		free(wp);	// The updated Organya source code includes this fix
#endif
		return FALSE;
	}

	CopyMemory(lpbuf1, (BYTE*)wp+0x3A, dwbuf1);	// +3aはデータの頭 (+ 3a is the head of the data)

	if (dwbuf2 != 0)
		CopyMemory(lpbuf2, (BYTE*)wp+0x3A+dwbuf1, dwbuf2);

	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);

	return TRUE;
}
*/
void PlaySoundObject(int no, int mode)
{
	if (lpSECONDARYBUFFER[no] != NULL)
	{
		switch (mode)
		{
			case 0:	// 停止 (Stop)
				AudioBackend_PauseSound(lpSECONDARYBUFFER[no]);
				break;

			case 1:	// 再生 (Playback)
				AudioBackend_StopSound(lpSECONDARYBUFFER[no]);
				AudioBackend_PlaySound(lpSECONDARYBUFFER[no], FALSE);
				break;

			case -1:// ループ再生 (Loop playback)
				AudioBackend_PlaySound(lpSECONDARYBUFFER[no], TRUE);
				break;
		}
	}
}

void ChangeSoundFrequency(int no, unsigned long rate)	// 100がMIN9999がMAXで2195?がﾉｰﾏﾙ (100 is MIN, 9999 is MAX, and 2195 is normal)
{
	AudioBackend_SetSoundFrequency(lpSECONDARYBUFFER[no], (rate * 10) + 100);
}

void ChangeSoundVolume(int no, long volume)	// 300がMAXで300がﾉｰﾏﾙ (300 is MAX and 300 is normal)
{
	AudioBackend_SetSoundVolume(lpSECONDARYBUFFER[no], (volume - 300) * 8);
}

void ChangeSoundPan(int no, long pan)	// 512がMAXで256がﾉｰﾏﾙ (512 is MAX and 256 is normal)
{
	AudioBackend_SetSoundPan(lpSECONDARYBUFFER[no], (pan - 256) * 10);
}

// TODO - The stack frame for this function is inaccurate
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no)
{
	int i;
	int j;
	const PIXTONEPARAMETER *ptp_pointer;
	int sample_count;
	unsigned char *pcm_buffer;
	unsigned char *mixed_pcm_buffer;

	ptp_pointer = ptp;
	sample_count = 0;

	for (i = 0; i < ptp_num; i++)
	{
		if (ptp_pointer->size > sample_count)
			sample_count = ptp_pointer->size;

		++ptp_pointer;
	}

	lpSECONDARYBUFFER[no] = AudioBackend_CreateSound(22050, ptp->size);

	if (lpSECONDARYBUFFER[no] == NULL)
		return -1;

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
			if (pcm_buffer)
				free(pcm_buffer);

			if (mixed_pcm_buffer)
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

	// Maybe this used to be something to prevent audio popping?
	mixed_pcm_buffer[0] = mixed_pcm_buffer[0];
	mixed_pcm_buffer[sample_count - 1] = mixed_pcm_buffer[sample_count - 1];

	unsigned char *buffer = AudioBackend_LockSound(lpSECONDARYBUFFER[no], NULL);

	memcpy(buffer, mixed_pcm_buffer, ptp->size);

	AudioBackend_UnlockSound(lpSECONDARYBUFFER[no]);

	if (pcm_buffer != NULL)
		free(pcm_buffer);

	if (mixed_pcm_buffer != NULL)
		free(mixed_pcm_buffer);

	return sample_count;
}
