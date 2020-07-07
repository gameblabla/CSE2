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

#include "clownaudio/clownaudio.h"

#include <stddef.h>
#include <string.h>

#include "clownaudio/mixer.h"
#include "clownaudio/playback.h"

static ClownAudio_Stream *stream;
static ClownAudio_Mixer *mixer;

static void StreamCallback(void *user_data, short *output_buffer, size_t frames_to_do)
{
	(void)user_data;

	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_OutputSamples(mixer, output_buffer, frames_to_do);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT bool ClownAudio_Init(void)
{
	if (ClownAudio_InitPlayback())
	{
		unsigned long sample_rate = 48000;	// This default value is a fallback - it will be overwritten if the backend has a preferred rate
		stream = ClownAudio_CreateStream(&sample_rate, StreamCallback);

		if (stream != NULL)
		{
			mixer = ClownAudio_CreateMixer(sample_rate);

			if (mixer != NULL)
			{
				ClownAudio_ResumeStream(stream);

				return true;
			}

			ClownAudio_DestroyStream(stream);
		}

		ClownAudio_DeinitPlayback();
	}

	return false;
}

CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void)
{
	ClownAudio_PauseStream(stream);
	ClownAudio_DestroyMixer(mixer);
	ClownAudio_DestroyStream(stream);
	ClownAudio_DeinitPlayback();
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_LoadSoundDataFromMemory(mixer, file_buffer1, file_size1, file_buffer2, file_size2, config);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_LoadSoundDataFromFiles(mixer, intro_path, loop_path, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound_data)
{
	ClownAudio_Mixer_UnloadSoundData(sound_data);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_CreateSound(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	ClownAudio_Sound *sound = ClownAudio_Mixer_CreateSound(mixer, sound_data, config);

	ClownAudio_LockStream(stream);
	ClownAudio_SoundID sound_id = ClownAudio_Mixer_RegisterSound(mixer, sound);
	ClownAudio_UnlockStream(stream);

	return sound_id;
}

CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_SoundID sound_id)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_DestroySound(mixer, sound_id);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_SoundID sound_id)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_RewindSound(mixer, sound_id);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_SoundID sound_id)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_PauseSound(mixer, sound_id);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_SoundID sound_id)
{	
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_UnpauseSound(mixer, sound_id);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_FadeOutSound(mixer, sound_id, duration);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_FadeInSound(mixer, sound_id, duration);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_SoundID sound_id)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_CancelFade(mixer, sound_id);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_SoundID sound_id)
{
	ClownAudio_LockStream(stream);
	int status = ClownAudio_Mixer_GetSoundStatus(mixer, sound_id);
	ClownAudio_UnlockStream(stream);

	return status;
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_SetSoundVolume(mixer, sound_id, volume_left, volume_right);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_SoundID sound_id, bool loop)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_SetSoundLoop(mixer, sound_id, loop);
	ClownAudio_UnlockStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_SoundID sound_id, unsigned long sample_rate1, unsigned long sample_rate2)
{
	ClownAudio_LockStream(stream);
	ClownAudio_Mixer_SetSoundSampleRate(mixer, sound_id, sample_rate1, sample_rate2);
	ClownAudio_UnlockStream(stream);
}
