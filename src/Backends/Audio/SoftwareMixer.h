#pragma once

#include <stddef.h>

typedef struct Mixer_Sound Mixer_Sound;

void Mixer_Init(unsigned long frequency);
Mixer_Sound* Mixer_CreateSound(unsigned int frequency, size_t frames);
void Mixer_DestroySound(Mixer_Sound *sound);
unsigned char* Mixer_LockSound(Mixer_Sound *sound, size_t *size);
void Mixer_PlaySound(Mixer_Sound *sound, bool looping);
void Mixer_StopSound(Mixer_Sound *sound);
void Mixer_RewindSound(Mixer_Sound *sound);
void Mixer_SetSoundFrequency(Mixer_Sound *sound, unsigned int frequency);
void Mixer_SetSoundVolume(Mixer_Sound *sound, long volume);
void Mixer_SetSoundPan(Mixer_Sound *sound, long pan);
void Mixer_MixSounds(float *stream, unsigned int frames_total);
