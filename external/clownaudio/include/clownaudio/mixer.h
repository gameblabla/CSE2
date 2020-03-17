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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClownAudio_Mixer ClownAudio_Mixer;
typedef struct ClownAudio_SoundData ClownAudio_SoundData;
typedef unsigned int ClownAudio_Sound;

typedef struct ClownAudio_SoundDataConfig
{
	bool predecode;
	bool must_predecode;
	bool dynamic_sample_rate;
} ClownAudio_SoundDataConfig;

typedef struct ClownAudio_SoundConfig
{
	bool loop;
	bool do_not_free_when_done;
	bool dynamic_sample_rate;
} ClownAudio_SoundConfig;

CLOWNAUDIO_EXPORT void ClownAudio_InitSoundDataConfig(ClownAudio_SoundDataConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_InitSoundConfig(ClownAudio_SoundConfig *config);

CLOWNAUDIO_EXPORT ClownAudio_Mixer* ClownAudio_CreateMixer(unsigned long sample_rate);
CLOWNAUDIO_EXPORT void ClownAudio_DestroyMixer(ClownAudio_Mixer *mixer);

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config);
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
CLOWNAUDIO_EXPORT ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_Mixer *mixer, ClownAudio_SoundData *sound, ClownAudio_SoundConfig *config);
CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);
CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);
CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance, unsigned int duration);	// Duration is in milliseconds
CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_Mixer *mixer, ClownAudio_Sound instance, unsigned int duration);
CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_Mixer *mixer, ClownAudio_Sound instance);

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_Mixer *mixer, ClownAudio_Sound instance, float volume_left, float volume_right);	// Volume is linear, between 0.0f and 1.0f
CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_Mixer *mixer, ClownAudio_Sound instance, bool loop);
CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_Mixer *mixer, ClownAudio_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);

CLOWNAUDIO_EXPORT void ClownAudio_MixSamples(ClownAudio_Mixer *mixer, float *output_buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
