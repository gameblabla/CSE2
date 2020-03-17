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

#include "clownaudio.h"

#include <stddef.h>

#include "config.h"
#include "mixer.h"
#include "playback/playback.h"

static ClownAudio_Mixer *mixer;
static BackendStream *stream;

static void CallbackStream(void *user_data, float *output_buffer, size_t frames_to_do)
{
	(void)user_data;

	for (size_t i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
		output_buffer[i] = 0.0f;

	ClownAudio_Mixer_MixSamples(mixer, output_buffer, frames_to_do);
}

CLOWNAUDIO_EXPORT bool ClownAudio_Init(void)
{
	mixer = ClownAudio_CreateMixer(STREAM_SAMPLE_RATE);

	if (mixer != NULL)
	{
		if (Backend_Init())
		{
			stream = Backend_CreateStream(CallbackStream, NULL);

			if (stream != NULL)
			{
				if (Backend_ResumeStream(stream))
					return true;

				Backend_DestroyStream(stream);
			}

			Backend_Deinit();
		}

		ClownAudio_DestroyMixer(mixer);
	}

	return false;
}

CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void)
{
	Backend_DestroyStream(stream);
	Backend_Deinit();
	ClownAudio_DestroyMixer(mixer);
}

CLOWNAUDIO_EXPORT void ClownAudio_Pause(void)
{
	Backend_PauseStream(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Unpause(void)
{
	Backend_ResumeStream(stream);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	return (ClownAudio_SoundData*)ClownAudio_Mixer_LoadSoundData(file_buffer1, file_size1, file_buffer2, file_size2, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound)
{
	ClownAudio_Mixer_UnloadSoundData((ClownAudio_Mixer_SoundData*)sound);
}

CLOWNAUDIO_EXPORT ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_SoundData *sound, ClownAudio_SoundConfig *config)
{
	return ClownAudio_Mixer_CreateSound(mixer, (ClownAudio_Mixer_SoundData*)sound, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_Sound instance)
{
	ClownAudio_Mixer_DestroySound(mixer, instance);
}

CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_Sound instance)
{
	ClownAudio_Mixer_RewindSound(mixer, instance);
}

CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_Sound instance)
{
	ClownAudio_Mixer_PauseSound(mixer, instance);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_Sound instance)
{
	ClownAudio_Mixer_UnpauseSound(mixer, instance);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_Sound instance, unsigned int duration)
{
	ClownAudio_Mixer_FadeOutSound(mixer, instance, duration);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_Sound instance, unsigned int duration)
{
	ClownAudio_Mixer_FadeInSound(mixer, instance, duration);
}

CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_Sound instance)
{
	ClownAudio_Mixer_CancelFade(mixer, instance);
}

CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_Sound instance)
{
	return ClownAudio_Mixer_GetSoundStatus(mixer, instance);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_Sound instance, float volume_left, float volume_right)
{
	ClownAudio_Mixer_SetSoundVolume(mixer, instance, volume_left, volume_right);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_Sound instance, bool loop)
{
	ClownAudio_Mixer_SetSoundLoop(mixer, instance, loop);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2)
{
	ClownAudio_Mixer_SetSoundSampleRate(mixer, instance, sample_rate1, sample_rate2);
}
