#include "OtherMusicFormats.h"

#include <stdbool.h>
#include <stddef.h>

extern "C" {
#include "audio_lib/mixer.h"
}

#define MIN(a, b) (a < b ? a : b)

typedef struct Song
{
	bool valid;
	Mixer_Sound *sound;
	Mixer_SoundInstanceID instance;
} Song;

static Song song;
static Song previous_song;
static bool playing;

void OtherMusic_Init(unsigned int sample_rate)
{
	Mixer_Init(sample_rate, 2);
}

void OtherMusic_Deinit(void)
{
	Mixer_Deinit();
}

void OtherMusic_Play(void)
{
	playing = true;
}

void OtherMusic_Stop(void)
{
	playing = false;
}

void OtherMusic_Load(const char *path, bool loop)
{
	if (previous_song.valid)
	{
		Mixer_StopSound(previous_song.instance);
		Mixer_UnloadSound(previous_song.sound);
	}

	if (song.valid)
	{
		Mixer_PauseSound(song.instance);
		previous_song = song;
	}

	song.sound = Mixer_LoadSound(path, loop, false);
	song.instance = Mixer_PlaySound(song.sound);
	Mixer_UnpauseSound(song.instance);
	song.valid = true;
}

void OtherMusic_LoadPrevious(void)
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

void OtherMusic_FadeOut(void)
{
	Mixer_FadeOutSound(song.instance, 5 * 1000);
}

void OtherMusic_Mix(float *buffer, unsigned long frames)
{
	if (playing)
		Mixer_MixSamples(buffer, frames);
}
