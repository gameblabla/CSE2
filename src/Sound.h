#pragma once

#include <stddef.h>

#include <dsound.h>

#include "WindowsWrapper.h"

#include "PixTone.h"

#define SE_MAX 160	// According to the Organya source code release, this is the real name for this constant

extern LPDIRECTSOUND       lpDS;
extern LPDIRECTSOUNDBUFFER lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound(HWND hwnd);
void EndDirectSound(void);
void PlaySoundObject(int no, int mode);
void ChangeSoundFrequency(int no, unsigned long rate);
void ChangeSoundVolume(int no, long volume);
void ChangeSoundPan(int no, long pan);
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no);
