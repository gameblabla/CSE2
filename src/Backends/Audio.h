#pragma once

#include <stddef.h>

typedef struct AudioBackend_Sound AudioBackend_Sound;

bool AudioBackend_Init(void);
void AudioBackend_Deinit(void);

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length);
void AudioBackend_DestroySound(AudioBackend_Sound *sound);

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping);
void AudioBackend_StopSound(AudioBackend_Sound *sound);
void AudioBackend_RewindSound(AudioBackend_Sound *sound);

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency);
void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume);
void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan);

void AudioBackend_SetOrganyaCallback(void (*callback)(void));
void AudioBackend_SetOrganyaTimer(unsigned int milliseconds);

void AudioBackend_Lock(void);
void AudioBackend_Unlock(void);
