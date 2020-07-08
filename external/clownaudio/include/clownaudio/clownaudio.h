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

#include "mixer.h"

#if !defined(CLOWNAUDIO_EXPORT) && !defined(CLOWNAUDIO_NO_EXPORT)
#include "clownaudio_export.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Defined elsewhere, but included here for documentation
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
*/

////////////////////
// Initialisation //
////////////////////

// Initialises clownaudio
CLOWNAUDIO_EXPORT bool ClownAudio_Init(void);

// Deinitialises clownaudio
CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void);


//////////////////////////////////
// Sound-data loading/unloading //
//////////////////////////////////

// Loads data from up to two memory buffers - either buffer pointer can be NULL.
// If two buffers are specified and looping is enabled, the sound will loop at the point where the first buffer ends, and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config);

// Loads data from up to two files - either file path can be NULL.
// If two files are specified and looping is enabled, the sound will loop at the point where the first file ends, and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config);

// Unloads data. All sounds using the specified data must be destroyed manually before this function is called.
CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound_data);


////////////////////////////////
// Sound creation/destruction //
////////////////////////////////

// Creates a sound from sound-data. The sound will be paused by default.
CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_CreateSound(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config);

// Destroys sound.
CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_SoundID sound_id);


/////////////////////////////
// Assorted sound controls //
/////////////////////////////

// Playback

// Rewinds sound to the very beginning.
CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_SoundID sound_id);

// Pauses sound.
CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_SoundID sound_id);

// Unpauses sound.
CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_SoundID sound_id);


// Fading

// Make sound fade-out over the specified duration, measured in milliseconds.
// If the sound is currently fading-in, this function will override it, and cause the sound to fade-out from the volume is was currently at.  
CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_SoundID sound_id, unsigned int duration);

// Make sound fade-in over the specified duration, measured in milliseconds.
// If the sound is currently fading-out, this function will override it, and cause the sound to fade-in from the volume is was currently at.  
CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_SoundID sound_id, unsigned int duration);

// Aborts fading, and instantly restores the sound to full volume.
// If you want to smoothly-undo an in-progress fade, use one of the above functions instead.
CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_SoundID sound_id);


// Miscellaneous

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused.
CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_SoundID sound_id);

// Sets stereo volume - volume is linear, ranging from 0 (silence) to 0x100 (full volume). Exceeding 0x100 will amplify the volume.
CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right);

// Change whether the sound should loop or not. Only certain file formats support this (for example - Ogg Vorbis does, and PxTone doesn't).
CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_SoundID sound_id, bool loop);

// Override the sound's sample-rate. Note - the sound must have been created with `dynamic_sample_rate` enabled in the configuration struct,
// otherwise this function will silently fail.
CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_SoundID sound_id, unsigned long sample_rate1, unsigned long sample_rate2);


#ifdef __cplusplus
}
#endif
