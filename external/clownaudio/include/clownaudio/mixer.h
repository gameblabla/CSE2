/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "clownaudio_export.h"

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClownAudio_Mixer ClownAudio_Mixer;
typedef struct ClownAudio_Mixer_SoundData ClownAudio_Mixer_SoundData;
typedef unsigned int ClownAudio_Mixer_Sound;

CLOWNAUDIO_EXPORT ClownAudio_Mixer* ClownAudio_CreateMixer(unsigned long sample_rate);
CLOWNAUDIO_EXPORT void ClownAudio_DestroyMixer(ClownAudio_Mixer *mixer);

CLOWNAUDIO_EXPORT ClownAudio_Mixer_SoundData* ClownAudio_Mixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_UnloadSoundData(ClownAudio_Mixer_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
CLOWNAUDIO_EXPORT ClownAudio_Mixer_Sound ClownAudio_Mixer_CreateSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_SoundData *sound, ClownAudio_SoundConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_DestroySound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_RewindSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_PauseSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_UnpauseSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_FadeOutSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance, unsigned int duration);	// Duration is in milliseconds
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_FadeInSound(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance, unsigned int duration);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_CancelFade(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
CLOWNAUDIO_EXPORT int ClownAudio_Mixer_GetSoundStatus(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundVolume(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance, float volume_left, float volume_right);	// Volume is linear, between 0.0f and 1.0f
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundLoop(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance, bool loop);
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundSampleRate(ClownAudio_Mixer *mixer, ClownAudio_Mixer_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_MixSamples(ClownAudio_Mixer *mixer, float *output_buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
