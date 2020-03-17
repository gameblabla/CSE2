#include "ExtraSoundFormats.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "Sound.h"

#include "clownaudio/mixer.h"

typedef struct SoundSlot
{
	bool valid;
	ClownAudio_SoundData *sound;
	ClownAudio_Sound instance;
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
		ClownAudio_DestroySound(mixer, previous_song.instance);
		ClownAudio_UnloadSoundData(previous_song.sound);
		previous_song.valid = false;
	}

	if (song.valid)
	{
		ClownAudio_DestroySound(mixer, song.instance);
		ClownAudio_UnloadSoundData(song.sound);
		song.valid = false;
	}

	// Free SFX
	for (unsigned int i = 0; i < SE_MAX; ++i)
	{
		if (sfx_list[i].valid)
		{
			ClownAudio_DestroySound(mixer, sfx_list[i].instance);
			ClownAudio_UnloadSoundData(sfx_list[i].sound);
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

void ExtraSound_LoadMusic(const char *path, bool loop)
{
	song.valid = false;

	if (previous_song.valid)
	{
		ClownAudio_DestroySound(mixer, previous_song.instance);
		ClownAudio_UnloadSoundData(previous_song.sound);
	}

	if (song.valid)
		ClownAudio_PauseSound(mixer, song.instance);

	previous_song = song;

	if (path != NULL)
	{
		size_t file_buffer_size;

		ClownAudio_SoundDataConfig data_config;
		ClownAudio_InitSoundDataConfig(&data_config);
		song.sound = ClownAudio_LoadSoundDataFromFiles(path, NULL, &data_config);

		if (song.sound != NULL)
		{
			ClownAudio_SoundConfig sound_config;
			ClownAudio_InitSoundConfig(&sound_config);
			sound_config.loop = loop;
			song.instance = ClownAudio_CreateSound(mixer, song.sound, &sound_config);

			if (song.instance)
			{
				ClownAudio_UnpauseSound(mixer, song.instance);

				song.valid = true;

				return;
			}

			ClownAudio_UnloadSoundData(song.sound);
		}
	}
}

void ExtraSound_LoadPreviousMusic(void)
{
	if (song.valid)
	{
		ClownAudio_DestroySound(mixer, song.instance);
		ClownAudio_UnloadSoundData(song.sound);
	}

	if (previous_song.valid)
	{
		song = previous_song;
		ClownAudio_CancelFade(mixer, song.instance);
		ClownAudio_UnpauseSound(mixer, song.instance);
	}

	previous_song.valid = false;
}

void ExtraSound_PauseMusic(void)
{
	if (song.valid)
		ClownAudio_PauseSound(mixer, song.instance);
}

void ExtraSound_FadeOutMusic(void)
{
	ClownAudio_FadeOutSound(mixer, song.instance, 5 * 1000);
}

void ExtraSound_LoadSFX(const char *path, int id)
{
	if (sfx_list[id].valid)
	{
		ClownAudio_DestroySound(mixer, sfx_list[id].instance);
		ClownAudio_UnloadSoundData(sfx_list[id].sound);
	}

	size_t file_buffer_size;

	ClownAudio_SoundDataConfig data_config;
	ClownAudio_InitSoundDataConfig(&data_config);
	data_config.predecode = true;
	sfx_list[id].sound = ClownAudio_LoadSoundDataFromFiles(path, NULL, &data_config);

	if (sfx_list[id].sound != NULL)
	{
		ClownAudio_SoundConfig sound_config;
		ClownAudio_InitSoundConfig(&sound_config);
		sound_config.do_not_free_when_done = true;
		sfx_list[id].instance = ClownAudio_CreateSound(mixer, sfx_list[id].sound, &sound_config);

		if (sfx_list[id].instance != 0)
		{
			sfx_list[id].valid = true;
			return;
		}

		ClownAudio_UnloadSoundData(sfx_list[id].sound);
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
				ClownAudio_PauseSound(mixer, sfx_list[id].instance);
				break;

			case 1:
				ClownAudio_PauseSound(mixer, sfx_list[id].instance);
				ClownAudio_RewindSound(mixer, sfx_list[id].instance);
				ClownAudio_SetSoundLoop(mixer, sfx_list[id].instance, false);
				ClownAudio_UnpauseSound(mixer, sfx_list[id].instance);
				break;

			case -1:
				ClownAudio_SetSoundLoop(mixer, sfx_list[id].instance, true);
				ClownAudio_UnpauseSound(mixer, sfx_list[id].instance);
				break;
		}
	}
}

void ExtraSound_Mix(float *buffer, unsigned long frames)
{
	if (playing)
		ClownAudio_MixSamples(mixer, buffer, frames);
}
