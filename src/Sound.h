#pragma once

#include <stddef.h>

#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x500
#endif
#include <dsound.h>

#include "WindowsWrapper.h"

#include "PixTone.h"

#define SE_MAX 160	// According to the Organya source code release, this is the real name for this constant

enum SoundEffectNames
{
	// To be continued
	SND_SILLY_EXPLOSION = 25,
	SND_LARGE_OBJECT_HIT_GROUND = 26,
	// To be continued
	SND_ENEMY_SHOOT_PROJETILE = 39,
	// To be continued
	SND_BEHEMOTH_LARGE_HURT = 52,
	// To be continued
	SND_EXPLOSION = 72,
	// To be continued
};

extern LPDIRECTSOUND lpDS;
extern LPDIRECTSOUNDBUFFER lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound(HWND hwnd);
void EndDirectSound(void);
void PlaySoundObject(int no, int mode);
void ChangeSoundFrequency(int no, unsigned long rate);
void ChangeSoundVolume(int no, long volume);
void ChangeSoundPan(int no, long pan);
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no);
