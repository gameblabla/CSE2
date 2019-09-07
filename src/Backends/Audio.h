#pragma once

#include "../WindowsWrapper.h"

typedef struct AudioBackend_Sound AudioBackend_Sound;

BOOL AudioBackend_Init(void);
void AudioBackend_Deinit(void);

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, size_t frames);
void AudioBackend_DestroySound(AudioBackend_Sound *sound);

unsigned char* AudioBackend_LockSound(AudioBackend_Sound *sound, size_t *size);
void AudioBackend_UnlockSound(AudioBackend_Sound *sound);

void AudioBackend_PlaySound(AudioBackend_Sound *sound, BOOL looping);
void AudioBackend_StopSound(AudioBackend_Sound *sound);
void AudioBackend_RewindSound(AudioBackend_Sound *sound);

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency);
void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume);
void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan);
