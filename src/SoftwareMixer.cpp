#include "SoftwareMixer.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

struct Mixer_Sound
{
	signed char *samples;
	size_t frames;
	size_t position;
	unsigned long sample_offset_remainder;  // 16.16 fixed-point
	unsigned long advance_delta;            // 16.16 fixed-point
	bool playing;
	bool looping;
	short volume;    // 8.8 fixed-point
	short pan_l;     // 8.8 fixed-point
	short pan_r;     // 8.8 fixed-point
	short volume_l;  // 8.8 fixed-point
	short volume_r;  // 8.8 fixed-point

	struct Mixer_Sound *next;
};

static Mixer_Sound *sound_list_head;

static unsigned long output_frequency;

static unsigned short MillibelToScale(long volume)
{
	// Volume is in hundredths of a decibel, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
	return (unsigned short)(pow(10.0, volume / 2000.0) * 256.0f);
}

void Mixer_Init(unsigned long frequency)
{
	output_frequency = frequency;
}

Mixer_Sound* Mixer_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	Mixer_Sound *sound = (Mixer_Sound*)malloc(sizeof(Mixer_Sound));

	if (sound == NULL)
		return NULL;

	sound->samples = (signed char*)malloc(length + 1);

	if (sound->samples == NULL)
	{
		free(sound);
		return NULL;
	}

	for (size_t i = 0; i < length; ++i)
		sound->samples[i] = samples[i] - 0x80;	// Convert from unsigned 8-bit PCM to signed

	sound->frames = length;
	sound->playing = false;
	sound->position = 0;
	sound->sample_offset_remainder = 0;

	Mixer_SetSoundFrequency(sound, frequency);
	Mixer_SetSoundVolume(sound, 0);
	Mixer_SetSoundPan(sound, 0);

	sound->next = sound_list_head;
	sound_list_head = sound;

	return sound;
}

void Mixer_DestroySound(Mixer_Sound *sound)
{
	for (Mixer_Sound **sound_pointer = &sound_list_head; *sound_pointer != NULL; sound_pointer = &(*sound_pointer)->next)
	{
		if (*sound_pointer == sound)
		{
			*sound_pointer = sound->next;
			free(sound->samples);
			free(sound);
			break;
		}
	}
}

void Mixer_PlaySound(Mixer_Sound *sound, bool looping)
{
	sound->playing = true;
	sound->looping = looping;

	sound->samples[sound->frames] = looping ? sound->samples[0] : 0;	// For the linear interpolator
}

void Mixer_StopSound(Mixer_Sound *sound)
{
	sound->playing = false;
}

void Mixer_RewindSound(Mixer_Sound *sound)
{
	sound->position = 0;
	sound->sample_offset_remainder = 0;
}

void Mixer_SetSoundFrequency(Mixer_Sound *sound, unsigned int frequency)
{
	sound->advance_delta = (frequency << 16) / output_frequency;
}

void Mixer_SetSoundVolume(Mixer_Sound *sound, long volume)
{
	sound->volume = MillibelToScale(volume);

	sound->volume_l = (sound->pan_l * sound->volume) >> 8;
	sound->volume_r = (sound->pan_r * sound->volume) >> 8;
}

void Mixer_SetSoundPan(Mixer_Sound *sound, long pan)
{
	sound->pan_l = MillibelToScale(-pan);
	sound->pan_r = MillibelToScale(pan);

	sound->volume_l = (sound->pan_l * sound->volume) >> 8;
	sound->volume_r = (sound->pan_r * sound->volume) >> 8;
}

void Mixer_MixSounds(long *stream, size_t frames_total)
{
	for (Mixer_Sound *sound = sound_list_head; sound != NULL; sound = sound->next)
	{
		if (sound->playing)
		{
			long *stream_pointer = stream;

			for (size_t frames_done = 0; frames_done < frames_total; ++frames_done)
			{
				// Perform linear interpolation
				const unsigned char subsample = sound->sample_offset_remainder >> 8;

				const short interpolated_sample = sound->samples[sound->position] * (0x100 - subsample)
				                                      + sound->samples[sound->position + 1] * subsample;

				// Mix, and apply volume
				*stream_pointer++ += (interpolated_sample * sound->volume_l) >> 8;
				*stream_pointer++ += (interpolated_sample * sound->volume_r) >> 8;

				// Increment sample
				sound->sample_offset_remainder += sound->advance_delta;
				sound->position += sound->sample_offset_remainder >> 16;
				sound->sample_offset_remainder &= 0xFFFF;

				// Stop or loop sample once it's reached its end
				if (sound->position >= sound->frames)
				{
					if (sound->looping)
					{
						sound->position %= sound->frames;
					}
					else
					{
						sound->playing = false;
						sound->position = 0;
						sound->sample_offset_remainder = 0;
						break;
					}
				}
			}
		}
	}
}
