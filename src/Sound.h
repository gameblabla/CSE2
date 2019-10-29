#pragma once

#include <stddef.h>

#include "WindowsWrapper.h"

#include "Backends/Audio.h"
#include "PixTone.h"

#define SE_MAX 160	// According to the Organya source code release, this is the real name for this constant

enum SoundEffectNames
{
	SND_YES_NO_CHANGE_CHOICE = 1,
	SND_MESSAGE_TYPING = 2,
	SND_QUOTE_BUMP_HEAD = 3,
	SND_SWITCH_WEAPON = 4,
	SND_YES_NO_PROMPT = 5,
	// To be continued
	SND_SILLY_EXPLOSION = 25,
	SND_LARGE_OBJECT_HIT_GROUND = 26,
	// To be continued
	SND_ENEMY_SHOOT_PROJETILE = 39,
	// To be continued
	SND_BEHEMOTH_LARGE_HURT = 52,
	// To be continued
	SND_EXPLOSION = 72
	// To be continued
};

extern BOOL audio_backend_initialised;
extern AudioBackend_Sound *lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound(void);
void EndDirectSound(void);
void PlaySoundObject(int no, int mode);
void ChangeSoundFrequency(int no, unsigned long rate);
void ChangeSoundVolume(int no, long volume);
void ChangeSoundPan(int no, long pan);
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no);
