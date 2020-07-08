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

#if !defined(CLOWNAUDIO_EXPORT) && !defined(CLOWNAUDIO_NO_EXPORT)
#include "clownaudio_export.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct ClownAudio_Mixer ClownAudio_Mixer;
typedef struct ClownAudio_Sound ClownAudio_Sound;
typedef struct ClownAudio_SoundData ClownAudio_SoundData;
typedef unsigned int ClownAudio_SoundID;

typedef struct ClownAudio_SoundDataConfig
{
	bool predecode;             // If true, the sound *may* be predecoded if possible. If not, the sound will still be loaded, albeit not predecoded.
	bool must_predecode;        // If true, the sound *must* be predecoded if possible. If not, the function will fail.
	bool dynamic_sample_rate;   // If sound is predecoded, then this needs to be true for `ClownAudio_SetSoundSampleRate` to work
} ClownAudio_SoundDataConfig;

typedef struct ClownAudio_SoundConfig
{
	bool loop;                  // If true, the sound will loop indefinitely
	bool do_not_free_when_done; // If true, the sound will not be automatically destroyed once it finishes playing
	bool dynamic_sample_rate;   // If sound is not predecoded, then this needs to be true for `ClownAudio_SetSoundSampleRate` to work
} ClownAudio_SoundConfig;


//////////////////////////////////
// Configuration initialisation //
//////////////////////////////////

// Initialises a `ClownAudio_SoundDataConfig` struct with sane default values
CLOWNAUDIO_EXPORT void ClownAudio_InitSoundDataConfig(ClownAudio_SoundDataConfig *config);

// Initialises a `ClownAudio_SoundConfig` struct with sane default values
CLOWNAUDIO_EXPORT void ClownAudio_InitSoundConfig(ClownAudio_SoundConfig *config);


////////////////////////////////
// Mixer creation/destruction //
////////////////////////////////

// Creates a mixer. Will return NULL if it fails.
CLOWNAUDIO_EXPORT ClownAudio_Mixer* ClownAudio_CreateMixer(unsigned long sample_rate);

// Destroys a mixer. All sounds playing through the specified mixer must be destroyed manually before this function is called.
CLOWNAUDIO_EXPORT void ClownAudio_DestroyMixer(ClownAudio_Mixer *mixer);


//////////////////////////////////
// Sound-data loading/unloading //
//////////////////////////////////

// Loads data from up to two memory buffers - either buffer pointer can be NULL.
// If two buffers are specified and looping is enabled, the sound will loop at the point where the first buffer ends, and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_Mixer_LoadSoundDataFromMemory(ClownAudio_Mixer *mixer, const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config);

// Loads data from up to two files - either file path can be NULL.
// If two files are specified and looping is enabled, the sound will loop at the point where the first file ends, and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_Mixer_LoadSoundDataFromFiles(ClownAudio_Mixer *mixer, const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config);

// Unloads data. All sounds using the specified data must be destroyed manually before this function is called.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_UnloadSoundData(ClownAudio_SoundData *sound_data);


////////////////////////////////
// Sound creation/destruction //
////////////////////////////////

// Creates a sound from sound-data. The sound will be paused by default.
CLOWNAUDIO_EXPORT ClownAudio_Sound* ClownAudio_Mixer_CreateSound(ClownAudio_Mixer *mixer, ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config);

// Used to create a sound ID from a sound. Must be done only once.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_Mixer_RegisterSound(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound);

// Destroys sound.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_DestroySound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);


/////////////////////////////
// Assorted sound controls //
/////////////////////////////

// Playback

// Rewinds sound to the very beginning.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_RewindSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);

// Pauses sound.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_PauseSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);

// Unpauses sound.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_UnpauseSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);


// Fading

// Make sound fade-out over the specified duration, measured in milliseconds.
// If the sound is currently fading-in, this function will override it, and cause the sound to fade-out from the volume is was currently at.  
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_FadeOutSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned int duration);

// Make sound fade-in over the specified duration, measured in milliseconds.
// If the sound is currently fading-out, this function will override it, and cause the sound to fade-in from the volume is was currently at.  
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_FadeInSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned int duration);

// Aborts fading, and instantly restores the sound to full volume.
// If you want to smoothly-undo an in-progress fade, use one of the above functions instead.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_CancelFade(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);


// Miscellaneous

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT int ClownAudio_Mixer_GetSoundStatus(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id);

// Sets stereo volume - volume is linear, ranging from 0 (silence) to 0x100 (full volume). Exceeding 0x100 will amplify the volume.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundVolume(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right);

// Change whether the sound should loop or not. Only certain file formats support this (for example - Ogg Vorbis does, and PxTone doesn't).
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundLoop(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, bool loop);

// Override the sound's sample-rate. Note - the sound must have been created with `dynamic_sample_rate` enabled in the configuration struct,
// otherwise this function will silently fail.
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SetSoundSampleRate(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned long sample_rate1, unsigned long sample_rate2);


////////////
// Output //
////////////

// Mix interlaced (L,R ordering) S16 PCM samples into specified S32 buffer (not clamped).
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_MixSamples(ClownAudio_Mixer *mixer, long *output_buffer, size_t frames_to_do);

// Output interlaced (L,R ordering) S16 PCM samples into specified S16 buffer (clamped).
// Must be guarded with mutex.
CLOWNAUDIO_EXPORT void ClownAudio_Mixer_OutputSamples(ClownAudio_Mixer *mixer, short *output_buffer, size_t frames_to_do);


#ifdef __cplusplus
}
#endif
