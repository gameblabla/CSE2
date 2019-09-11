#pragma once

#include <stdbool.h>

typedef struct Mixer_Sound Mixer_Sound;
typedef unsigned int Mixer_SoundInstanceID;

void Mixer_Init(unsigned int sample_rate, unsigned int channel_count);
void Mixer_Deinit(void);
Mixer_Sound* Mixer_LoadSound(const char *file_path, bool predecode);
void Mixer_UnloadSound(Mixer_Sound *sound);
Mixer_SoundInstanceID Mixer_PlaySound(Mixer_Sound *sound, bool loop);
void Mixer_StopSound(Mixer_SoundInstanceID instance);
void Mixer_PauseSound(Mixer_SoundInstanceID instance);
void Mixer_UnpauseSound(Mixer_SoundInstanceID instance);
void Mixer_FadeOutSound(Mixer_SoundInstanceID instance, unsigned int duration);
void Mixer_FadeInSound(Mixer_SoundInstanceID instance, unsigned int duration);
void Mixer_CancelFade(Mixer_SoundInstanceID instance);
void Mixer_SetSoundVolume(Mixer_SoundInstanceID instance, float volume);
void Mixer_MixSamples(float *output_buffer, unsigned long frames_to_do);
