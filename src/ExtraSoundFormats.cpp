#include "ExtraSoundFormats.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "Sound.h"

#include "clownaudio/mixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

typedef struct SoundSlot
{
	bool valid;
	ClownAudio_SoundData *sound_data;
	ClownAudio_SoundID sound_id;
	long volume;
	long pan;
} SoundSlot;

static ClownAudio_Mixer *mixer;

static SoundSlot song;
static SoundSlot previous_song;

static SoundSlot sfx_list[SE_MAX];

static bool playing = true;

static unsigned short MillibelToScale(long volume)
{
	// Volume is in hundredths of a decibel, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
	return (unsigned short)(pow(10.0, volume / 2000.0) * 256.0f);
}

void ExtraSound_Init(unsigned int sample_rate)
{
	mixer = ClownAudio_CreateMixer(sample_rate);
}

void ExtraSound_Deinit(void)
{
	// Free songs
	if (previous_song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_DestroySound(mixer, previous_song.sound_id);
		AudioBackend_Unlock();

		ClownAudio_Mixer_UnloadSoundData(previous_song.sound_data);
		previous_song.valid = false;
	}

	if (song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_DestroySound(mixer, song.sound_id);
		AudioBackend_Unlock();

		ClownAudio_Mixer_UnloadSoundData(song.sound_data);
		song.valid = false;
	}

	// Free SFX
	for (unsigned int i = 0; i < SE_MAX; ++i)
	{
		if (sfx_list[i].valid)
		{
			AudioBackend_Lock();
			ClownAudio_Mixer_DestroySound(mixer, sfx_list[i].sound_id);
			AudioBackend_Unlock();

			ClownAudio_Mixer_UnloadSoundData(sfx_list[i].sound_data);
			sfx_list[i].valid = false;
		}
	}

	ClownAudio_DestroyMixer(mixer);
}

void ExtraSound_Play(void)
{
	playing = true;
}

void ExtraSound_Stop(void)
{
	playing = false;
}

void ExtraSound_LoadMusic(const char *intro_file_path, const char *loop_file_path, bool loop)
{
	if (previous_song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_DestroySound(mixer, previous_song.sound_id);
		AudioBackend_Unlock();

		ClownAudio_Mixer_UnloadSoundData(previous_song.sound_data);
	}

	if (song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_PauseSound(mixer, song.sound_id);
		AudioBackend_Unlock();
	}

	previous_song = song;

	if (intro_file_path != NULL || loop_file_path != NULL)
	{
		ClownAudio_SoundDataConfig data_config;
		ClownAudio_InitSoundDataConfig(&data_config);
		song.sound_data = ClownAudio_Mixer_LoadSoundDataFromFiles(mixer, intro_file_path, loop_file_path, &data_config);

		if (song.sound_data != NULL)
		{
			ClownAudio_SoundConfig sound_config;
			ClownAudio_InitSoundConfig(&sound_config);
			sound_config.loop = loop;
			ClownAudio_Sound *sound = ClownAudio_Mixer_CreateSound(mixer, song.sound_data, &sound_config);

			if (sound != NULL)
			{
				AudioBackend_Lock();
				song.sound_id = ClownAudio_Mixer_RegisterSound(mixer, sound);
				AudioBackend_Unlock();

				song.valid = true;

				return;
			}

			ClownAudio_Mixer_UnloadSoundData(song.sound_data);
		}
	}

	song.valid = false;
}

void ExtraSound_LoadPreviousMusic(void)
{
	if (song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_DestroySound(mixer, song.sound_id);
		AudioBackend_Unlock();

		ClownAudio_Mixer_UnloadSoundData(song.sound_data);
		song.valid = false;
	}

	if (previous_song.valid)
	{
		song = previous_song;

		AudioBackend_Lock();
		ClownAudio_Mixer_CancelFade(mixer, song.sound_id);
		AudioBackend_Unlock();
	}

	previous_song.valid = false;
}

void ExtraSound_PauseMusic(void)
{
	if (song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_PauseSound(mixer, song.sound_id);
		AudioBackend_Unlock();
	}
}

void ExtraSound_UnpauseMusic(void)
{
	if (song.valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_UnpauseSound(mixer, song.sound_id);
		AudioBackend_Unlock();
	}
}

void ExtraSound_FadeOutMusic(void)
{
	AudioBackend_Lock();
	ClownAudio_Mixer_FadeOutSound(mixer, song.sound_id, 5 * 1000);
	AudioBackend_Unlock();
}

void ExtraSound_SetMusicVolume(unsigned short volume)
{
	const unsigned short volume_linear = (volume * volume) >> 8;

	AudioBackend_Lock();
	ClownAudio_Mixer_SetSoundVolume(mixer, song.sound_id, volume_linear, volume_linear);
	AudioBackend_Unlock();
}

void ExtraSound_LoadSFX(const char *path, int id)
{
	if (sfx_list[id].valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_DestroySound(mixer, sfx_list[id].sound_id);
		AudioBackend_Unlock();

		ClownAudio_Mixer_UnloadSoundData(sfx_list[id].sound_data);
	}

	size_t file_buffer_size;

	ClownAudio_SoundDataConfig data_config;
	ClownAudio_InitSoundDataConfig(&data_config);
	data_config.predecode = true;
	data_config.dynamic_sample_rate = true;
	sfx_list[id].sound_data = ClownAudio_Mixer_LoadSoundDataFromFiles(mixer, path, NULL, &data_config);

	if (sfx_list[id].sound_data != NULL)
	{
		ClownAudio_SoundConfig sound_config;
		ClownAudio_InitSoundConfig(&sound_config);
		sound_config.do_not_free_when_done = true;
		sound_config.dynamic_sample_rate = true;
		ClownAudio_Sound *sound = ClownAudio_Mixer_CreateSound(mixer, sfx_list[id].sound_data, &sound_config);

		if (sound != NULL)
		{
			AudioBackend_Lock();
			sfx_list[id].sound_id = ClownAudio_Mixer_RegisterSound(mixer, sound);
			AudioBackend_Unlock();

			sfx_list[id].volume = 0;
			sfx_list[id].pan = 0;
			sfx_list[id].valid = true;
			return;
		}

		ClownAudio_Mixer_UnloadSoundData(sfx_list[id].sound_data);
	}

	sfx_list[id].valid = false;
}

void ExtraSound_PlaySFX(int id, int mode)
{
	if (sfx_list[id].valid)
	{
		AudioBackend_Lock();

		switch (mode)
		{
			case 0:
				ClownAudio_Mixer_PauseSound(mixer, sfx_list[id].sound_id);
				break;

			case 1:
				ClownAudio_Mixer_PauseSound(mixer, sfx_list[id].sound_id);
				ClownAudio_Mixer_RewindSound(mixer, sfx_list[id].sound_id);
				ClownAudio_Mixer_SetSoundLoop(mixer, sfx_list[id].sound_id, false);
				ClownAudio_Mixer_UnpauseSound(mixer, sfx_list[id].sound_id);
				break;

			case -1:
				ClownAudio_Mixer_SetSoundLoop(mixer, sfx_list[id].sound_id, true);
				ClownAudio_Mixer_UnpauseSound(mixer, sfx_list[id].sound_id);
				break;
		}

		AudioBackend_Unlock();
	}
}

void ExtraSound_SetSFXFrequency(int id, unsigned long frequency)
{
	if (sfx_list[id].valid)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_SetSoundSampleRate(mixer, sfx_list[id].sound_id, frequency, frequency);
		AudioBackend_Unlock();
	}
}

void ExtraSound_SetSFXVolume(int id, long volume)
{
	if (sfx_list[id].valid)
	{
		sfx_list[id].volume = volume;

		const unsigned short volume_scale = MillibelToScale(sfx_list[id].volume);
		const unsigned short pan_scale_left = MillibelToScale(-sfx_list[id].pan);
		const unsigned short pan_scale_right = MillibelToScale(sfx_list[id].pan);

		AudioBackend_Lock();
		ClownAudio_Mixer_SetSoundVolume(mixer, sfx_list[id].sound_id, (pan_scale_left * volume_scale) >> 8, (pan_scale_right * volume_scale) >> 8);
		AudioBackend_Unlock();
	}
}

void ExtraSound_SetSFXPan(int id, long pan)
{
	if (sfx_list[id].valid)
	{
		sfx_list[id].pan = pan;

		const unsigned short volume_scale = MillibelToScale(sfx_list[id].volume);
		const unsigned short pan_scale_left = MillibelToScale(-sfx_list[id].pan);
		const unsigned short pan_scale_right = MillibelToScale(sfx_list[id].pan);

		AudioBackend_Lock();
		ClownAudio_Mixer_SetSoundVolume(mixer, sfx_list[id].sound_id, (pan_scale_left * volume_scale) >> 8, (pan_scale_right * volume_scale) >> 8);
		AudioBackend_Unlock();
	}
}

void ExtraSound_Mix(long *buffer, unsigned long frames)
{
	if (playing)
	{
		AudioBackend_Lock();
		ClownAudio_Mixer_MixSamples(mixer, buffer, frames);
		AudioBackend_Unlock();
	}
}
