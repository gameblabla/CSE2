#include "ExtraSoundFormats.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "Sound.h"

extern "C" {
#include "clownaudio/mixer.h"
}

typedef struct Song
{
	bool valid;
	Mixer_Sound *sound;
	Mixer_SoundInstanceID instance;
} Song;

typedef struct SFX
{
	Mixer_Sound *sound;
	Mixer_SoundInstanceID instance;
	bool looping;
} SFX;

static Song song;
static Song previous_song;

static SFX *sfx_list[SE_MAX];

static bool playing = true;

void ExtraSound_Init(unsigned int sample_rate)
{
	Mixer_Init(sample_rate, 2);
}

void ExtraSound_Deinit(void)
{
	// Free songs
	if (previous_song.valid)
	{
		Mixer_StopSound(previous_song.instance);
		Mixer_UnloadSound(previous_song.sound);
		previous_song.valid = false;
	}

	if (song.valid)
	{
		Mixer_StopSound(song.instance);
		Mixer_UnloadSound(song.sound);
		song.valid = false;
	}

	// Free SFX
	for (unsigned int i = 0; i < SE_MAX; ++i)
	{
		if (sfx_list[i])
		{
			Mixer_StopSound(sfx_list[i]->instance);
			Mixer_UnloadSound(sfx_list[i]->sound);
			free(sfx_list[i]);
			sfx_list[i] = NULL;
		}
	}

	Mixer_Deinit();
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
	if (previous_song.valid)
	{
		Mixer_StopSound(previous_song.instance);
		Mixer_UnloadSound(previous_song.sound);
	}

	if (song.valid)
		Mixer_PauseSound(song.instance);

	previous_song = song;

	if (path)
	{
		song.sound = Mixer_LoadSound(path, false);
		song.instance = Mixer_PlaySound(song.sound, loop);
		Mixer_UnpauseSound(song.instance);
		song.valid = true;
	}
	else
	{
		song.valid = false;
	}
}

void ExtraSound_LoadPreviousMusic(void)
{
	if (song.valid)
	{
		Mixer_StopSound(song.instance);
		Mixer_UnloadSound(song.sound);
	}

	if (previous_song.valid)
	{
		song = previous_song;
		Mixer_CancelFade(song.instance);
		Mixer_UnpauseSound(song.instance);
	}

	previous_song.valid = false;
}

void ExtraSound_PauseMusic(void)
{
	if (song.valid)
		Mixer_PauseSound(song.instance);
}

void ExtraSound_FadeOutMusic(void)
{
	Mixer_FadeOutSound(song.instance, 5 * 1000);
}

void ExtraSound_LoadSFX(const char *path, int id)
{
	if (sfx_list[id])
	{
		Mixer_StopSound(sfx_list[id]->instance);
		Mixer_UnloadSound(sfx_list[id]->sound);
	}
	else
	{
		sfx_list[id] = (SFX*)malloc(sizeof(SFX));
	}

	if (sfx_list[id])
	{
		sfx_list[id]->sound = Mixer_LoadSound(path, true);
		sfx_list[id]->instance = Mixer_PlaySound(sfx_list[id]->sound, false);
		sfx_list[id]->looping = false;
	}
}

void ExtraSound_PlaySFX(int id, int mode)
{
	if (sfx_list[id])
	{
		switch (mode)
		{
			case 0:
				Mixer_PauseSound(sfx_list[id]->instance);
				break;

			case 1:
				sfx_list[id]->looping = false;
				Mixer_StopSound(sfx_list[id]->instance);
				sfx_list[id]->instance = Mixer_PlaySound(sfx_list[id]->sound, false);
				Mixer_UnpauseSound(sfx_list[id]->instance);
				break;

			case -1:
				if (sfx_list[id]->looping == false)
				{
					sfx_list[id]->looping = true;
					Mixer_StopSound(sfx_list[id]->instance);
					sfx_list[id]->instance = Mixer_PlaySound(sfx_list[id]->sound, true);
				}

				Mixer_UnpauseSound(sfx_list[id]->instance);

				break;
		}
	}
}

void ExtraSound_Mix(float *buffer, unsigned long frames)
{
	if (playing)
		Mixer_MixSamples(buffer, frames);
}
