#include "ExtraSoundFormats.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "Sound.h"

#include "clownaudio/mixer.h"

typedef struct SoundSlot
{
	bool valid;
	ClownAudio_SoundData *sound_data;
	ClownAudio_Sound sound;
} SoundSlot;

static ClownAudio_Mixer *mixer;

static SoundSlot song;
static SoundSlot previous_song;

static SoundSlot sfx_list[SE_MAX];

static bool playing = true;

void ExtraSound_Init(unsigned int sample_rate)
{
	mixer = ClownAudio_CreateMixer(sample_rate);
}

void ExtraSound_Deinit(void)
{
	// Free songs
	if (previous_song.valid)
	{
		ClownAudio_DestroySound(mixer, previous_song.sound);
		ClownAudio_UnloadSoundData(previous_song.sound_data);
		previous_song.valid = false;
	}

	if (song.valid)
	{
		ClownAudio_DestroySound(mixer, song.sound);
		ClownAudio_UnloadSoundData(song.sound_data);
		song.valid = false;
	}

	// Free SFX
	for (unsigned int i = 0; i < SE_MAX; ++i)
	{
		if (sfx_list[i].valid)
		{
			ClownAudio_DestroySound(mixer, sfx_list[i].sound);
			ClownAudio_UnloadSoundData(sfx_list[i].sound_data);
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
		ClownAudio_DestroySound(mixer, previous_song.sound);
		ClownAudio_UnloadSoundData(previous_song.sound_data);
	}

	if (song.valid)
		ClownAudio_PauseSound(mixer, song.sound);

	previous_song = song;

	if (intro_file_path != NULL || loop_file_path != NULL)
	{
		ClownAudio_SoundDataConfig data_config;
		ClownAudio_InitSoundDataConfig(&data_config);
		song.sound_data = ClownAudio_LoadSoundDataFromFiles(intro_file_path, loop_file_path, &data_config);

		if (song.sound_data != NULL)
		{
			ClownAudio_SoundConfig sound_config;
			ClownAudio_InitSoundConfig(&sound_config);
			sound_config.loop = loop;
			song.sound = ClownAudio_CreateSound(mixer, song.sound_data, &sound_config);

			if (song.sound != 0)
			{
				ClownAudio_UnpauseSound(mixer, song.sound);

				song.valid = true;

				return;
			}

			ClownAudio_UnloadSoundData(song.sound_data);
		}
	}

	song.valid = false;
}

void ExtraSound_LoadPreviousMusic(void)
{
	if (song.valid)
	{
		ClownAudio_DestroySound(mixer, song.sound);
		ClownAudio_UnloadSoundData(song.sound_data);
		song.valid = false;
	}

	if (previous_song.valid)
	{
		song = previous_song;
		ClownAudio_CancelFade(mixer, song.sound);
		ClownAudio_UnpauseSound(mixer, song.sound);
	}

	previous_song.valid = false;
}

void ExtraSound_PauseMusic(void)
{
	if (song.valid)
		ClownAudio_PauseSound(mixer, song.sound);
}

void ExtraSound_FadeOutMusic(void)
{
	ClownAudio_FadeOutSound(mixer, song.sound, 5 * 1000);
}

void ExtraSound_LoadSFX(const char *path, int id)
{
	if (sfx_list[id].valid)
	{
		ClownAudio_DestroySound(mixer, sfx_list[id].sound);
		ClownAudio_UnloadSoundData(sfx_list[id].sound_data);
	}

	size_t file_buffer_size;

	ClownAudio_SoundDataConfig data_config;
	ClownAudio_InitSoundDataConfig(&data_config);
	data_config.predecode = true;
	sfx_list[id].sound_data = ClownAudio_LoadSoundDataFromFiles(path, NULL, &data_config);

	if (sfx_list[id].sound_data != NULL)
	{
		ClownAudio_SoundConfig sound_config;
		ClownAudio_InitSoundConfig(&sound_config);
		sound_config.do_not_free_when_done = true;
		sfx_list[id].sound = ClownAudio_CreateSound(mixer, sfx_list[id].sound_data, &sound_config);

		if (sfx_list[id].sound != 0)
		{
			sfx_list[id].valid = true;
			return;
		}

		ClownAudio_UnloadSoundData(sfx_list[id].sound_data);
	}

	sfx_list[id].valid = false;
}

void ExtraSound_PlaySFX(int id, int mode)
{
	if (sfx_list[id].valid)
	{
		switch (mode)
		{
			case 0:
				ClownAudio_PauseSound(mixer, sfx_list[id].sound);
				break;

			case 1:
				ClownAudio_PauseSound(mixer, sfx_list[id].sound);
				ClownAudio_RewindSound(mixer, sfx_list[id].sound);
				ClownAudio_SetSoundLoop(mixer, sfx_list[id].sound, false);
				ClownAudio_UnpauseSound(mixer, sfx_list[id].sound);
				break;

			case -1:
				ClownAudio_SetSoundLoop(mixer, sfx_list[id].sound, true);
				ClownAudio_UnpauseSound(mixer, sfx_list[id].sound);
				break;
		}
	}
}

void ExtraSound_SetSFXFrequency(int id, unsigned long frequency)
{
	if (sfx_list[id].valid)
		ClownAudio_SetSoundSampleRate(mixer, sfx_list[id].sound, frequency, frequency);
}

void ExtraSound_Mix(float *buffer, unsigned long frames)
{
	if (playing)
		ClownAudio_MixSamples(mixer, buffer, frames);
}