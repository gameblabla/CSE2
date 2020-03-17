#include "ExtraSoundFormats.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "Sound.h"

#include "clownaudio/mixer.h"

typedef struct Song
{
	bool valid;
	unsigned char *file_buffer;
	ClownAudio_Mixer_SoundData *sound;
	ClownAudio_Mixer_Sound instance;
} Song;

typedef struct SFX
{
	unsigned char *file_buffer;
	ClownAudio_Mixer_SoundData *sound;
	ClownAudio_Mixer_Sound instance;
	bool looping;
} SFX;

static ClownAudio_Mixer *mixer;

static Song song;
static Song previous_song;

static SFX *sfx_list[SE_MAX];

static bool playing = true;

static bool LoadFileToMemory(const char *path, unsigned char **buffer, size_t *size)
{
	bool success = false;

	FILE *file = fopen(path, "rb");

	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		*size = ftell(file);
		rewind(file);

		*buffer = (unsigned char*)malloc(*size);

		if (buffer != NULL)
		{
			fread(buffer, 1, *size, file);

			success = true;
		}

		fclose(file);
	}

	return success;
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
		ClownAudio_Mixer_DestroySound(mixer, previous_song.instance);
		ClownAudio_Mixer_UnloadSoundData(previous_song.sound);
		previous_song.valid = false;
	}

	if (song.valid)
	{
		ClownAudio_Mixer_DestroySound(mixer, song.instance);
		ClownAudio_Mixer_UnloadSoundData(song.sound);
		song.valid = false;
	}

	// Free SFX
	for (unsigned int i = 0; i < SE_MAX; ++i)
	{
		if (sfx_list[i])
		{
			ClownAudio_Mixer_DestroySound(mixer, sfx_list[i]->instance);
			ClownAudio_Mixer_UnloadSoundData(sfx_list[i]->sound);
			free(sfx_list[i]);
			sfx_list[i] = NULL;
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
		ClownAudio_Mixer_DestroySound(mixer, previous_song.instance);
		ClownAudio_Mixer_UnloadSoundData(previous_song.sound);
	}

	if (song.valid)
		ClownAudio_Mixer_PauseSound(mixer, song.instance);

	previous_song = song;

	if (path != NULL)
	{
		size_t file_buffer_size;

		if (LoadFileToMemory(path, &song.file_buffer, &file_buffer_size))
		{
			ClownAudio_SoundDataConfig data_config;
			ClownAudio_InitSoundDataConfig(&data_config);
			song.sound = ClownAudio_Mixer_LoadSoundData(song.file_buffer, file_buffer_size, NULL, 0, &data_config);

			ClownAudio_SoundConfig sound_config;
			ClownAudio_InitSoundConfig(&sound_config);
			sound_config.loop = loop;
			song.instance = ClownAudio_Mixer_CreateSound(mixer, song.sound, &sound_config);

			ClownAudio_Mixer_UnpauseSound(mixer, song.instance);

			song.valid = true;
		}
	}
}

void ExtraSound_LoadPreviousMusic(void)
{
	if (song.valid)
	{
		ClownAudio_Mixer_DestroySound(mixer, song.instance);
		ClownAudio_Mixer_UnloadSoundData(song.sound);
	}

	if (previous_song.valid)
	{
		song = previous_song;
		ClownAudio_Mixer_CancelFade(mixer, song.instance);
		ClownAudio_Mixer_UnpauseSound(mixer, song.instance);
	}

	previous_song.valid = false;
}

void ExtraSound_PauseMusic(void)
{
	if (song.valid)
		ClownAudio_Mixer_PauseSound(mixer, song.instance);
}

void ExtraSound_FadeOutMusic(void)
{
	ClownAudio_Mixer_FadeOutSound(mixer, song.instance, 5 * 1000);
}

void ExtraSound_LoadSFX(const char *path, int id)
{
	if (sfx_list[id] != NULL)
	{
		ClownAudio_Mixer_DestroySound(mixer, sfx_list[id]->instance);
		ClownAudio_Mixer_UnloadSoundData(sfx_list[id]->sound);
	}
	else
	{
		sfx_list[id] = (SFX*)malloc(sizeof(SFX));
	}

	if (sfx_list[id] != NULL)
	{
		size_t file_buffer_size;

		if (LoadFileToMemory(path, &sfx_list[id]->file_buffer, &file_buffer_size))
		{
			ClownAudio_SoundDataConfig data_config;
			ClownAudio_InitSoundDataConfig(&data_config);
			data_config.predecode = true;
			sfx_list[id]->sound = ClownAudio_Mixer_LoadSoundData(sfx_list[id]->file_buffer, file_buffer_size, NULL, 0, &data_config);

			ClownAudio_SoundConfig sound_config;
			ClownAudio_InitSoundConfig(&sound_config);
	//		sound_config.do_not_free_when_done = true;
			sfx_list[id]->instance = ClownAudio_Mixer_CreateSound(mixer, sfx_list[id]->sound, &sound_config);

			sfx_list[id]->looping = false;
		}
	}
}

void ExtraSound_PlaySFX(int id, int mode)
{
	if (sfx_list[id] != NULL)
	{
		switch (mode)
		{
			case 0:
				ClownAudio_Mixer_PauseSound(mixer, sfx_list[id]->instance);
				break;

			case 1:
				sfx_list[id]->looping = false;
				ClownAudio_Mixer_DestroySound(mixer, sfx_list[id]->instance);

				ClownAudio_SoundConfig sound_config;
				ClownAudio_InitSoundConfig(&sound_config);
				sfx_list[id]->instance = ClownAudio_Mixer_CreateSound(mixer, sfx_list[id]->sound, &sound_config);
				ClownAudio_Mixer_UnpauseSound(mixer, sfx_list[id]->instance);
				break;

			case -1:
				if (sfx_list[id]->looping == false)
				{
					sfx_list[id]->looping = true;
					ClownAudio_Mixer_DestroySound(mixer, sfx_list[id]->instance);

					ClownAudio_SoundConfig sound_config;
					ClownAudio_InitSoundConfig(&sound_config);
					sound_config.loop = true;
					sfx_list[id]->instance = ClownAudio_Mixer_CreateSound(mixer, sfx_list[id]->sound, &sound_config);
				}

				ClownAudio_Mixer_UnpauseSound(mixer, sfx_list[id]->instance);

				break;
		}
	}
}

void ExtraSound_Mix(float *buffer, unsigned long frames)
{
	if (playing)
		ClownAudio_Mixer_MixSamples(mixer, buffer, frames);
}
