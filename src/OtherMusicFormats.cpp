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
		Mixer_UnpauseSound(song.instance);
	}

	previous_song.valid = false;
}

void OtherMusic_FadeOut(void)
{
	Mixer_FadeOutSound(song.instance, 5 * 1000);
}

void OtherMusic_Mix(float (*buffer)[2], unsigned long frames)
{
	if (playing)
	{
		unsigned long frames_done_total = 0;

		for (unsigned long frames_done; frames_done_total != frames; frames_done_total += frames_done)
		{
			float read_buffer[4096 * 2];

			frames_done = MIN(4096, frames - frames_done_total);

			Mixer_GetSamples(read_buffer, frames_done);

			float *buffer_pointer = buffer[frames_done_total];
			float *read_buffer_pointer = read_buffer;
			for (unsigned long i = 0; i < frames_done * 2; ++i)
				*buffer_pointer++ += *read_buffer_pointer++;

			if (frames_done < frames - frames_done_total)
				break;
		}
	}
}
