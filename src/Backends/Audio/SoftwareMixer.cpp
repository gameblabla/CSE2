#include "SoftwareMixer.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Attributes.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

struct Mixer_Sound
{
	unsigned char *samples;
	size_t frames;
	double position;
	double advance_delta;
	bool playing;
	bool looping;
	unsigned int frequency;
	float volume;
	float pan_l;
	float pan_r;
	float volume_l;
	float volume_r;

	struct Mixer_Sound *next;
};

static Mixer_Sound *sound_list_head;

static unsigned long output_frequency;

static double MillibelToScale(long volume)
{
	// Volume is in hundredths of a decibel, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
	return pow(10.0, volume / 2000.0);
}

void Mixer_Init(unsigned long frequency)
{
	output_frequency = frequency;
}

Mixer_Sound* Mixer_CreateSound(unsigned int frequency, size_t frames)
{
	Mixer_Sound *sound = (Mixer_Sound*)malloc(sizeof(Mixer_Sound));

	if (sound == NULL)
		return NULL;

	sound->samples = (unsigned char*)malloc(frames + 1);

	if (sound->samples == NULL)
	{
		free(sound);
		return NULL;
	}

	sound->frames = frames;
	sound->playing = false;
	sound->position = 0.0;

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

unsigned char* Mixer_LockSound(Mixer_Sound *sound, size_t *size)
{
	if (size != NULL)
		*size = sound->frames;

	return sound->samples;
}

void Mixer_PlaySound(Mixer_Sound *sound, bool looping)
{
	sound->playing = true;
	sound->looping = looping;

	sound->samples[sound->frames] = looping ? sound->samples[0] : 0x80;	// For the linear interpolator
}

void Mixer_StopSound(Mixer_Sound *sound)
{
	sound->playing = false;
	sound->position = 0.0;
}

void Mixer_RewindSound(Mixer_Sound *sound)
{
	sound->position = 0.0;
}

void Mixer_SetSoundFrequency(Mixer_Sound *sound, unsigned int frequency)
{
	sound->frequency = frequency;
	sound->advance_delta = (double)frequency / (double)output_frequency;
}

void Mixer_SetSoundVolume(Mixer_Sound *sound, long volume)
{
	sound->volume = (float)MillibelToScale(volume);

	sound->volume_l = sound->pan_l * sound->volume;
	sound->volume_r = sound->pan_r * sound->volume;
}

void Mixer_SetSoundPan(Mixer_Sound *sound, long pan)
{
	sound->pan_l = (float)MillibelToScale(-pan);
	sound->pan_r = (float)MillibelToScale(pan);

	sound->volume_l = sound->pan_l * sound->volume;
	sound->volume_r = sound->pan_r * sound->volume;
}

// Most CPU-intensive function in the game (2/3rd CPU time consumption in my experience), so marked with attrHot so the compiler considers it a hot spot (as it is) when optimizing
ATTRIBUTE_HOT void Mixer_MixSounds(float *stream, unsigned int frames_total)
{
	for (Mixer_Sound *sound = sound_list_head; sound != NULL; sound = sound->next)
	{
		if (sound->playing)
		{
			float *steam_pointer = stream;

			for (unsigned int frames_done = 0; frames_done < frames_total; ++frames_done)
			{
				// Get two samples, and normalise them to 0-1
				const float sample1 = (sound->samples[(size_t)sound->position] - 128.0f) / 128.0f;
				const float sample2 = (sound->samples[(size_t)sound->position + 1] - 128.0f) / 128.0f;

				// Perform linear interpolation
				const float interpolated_sample = sample1 + ((sample2 - sample1) * fmod((float)sound->position, 1.0f));

				*steam_pointer++ += interpolated_sample * sound->volume_l;
				*steam_pointer++ += interpolated_sample * sound->volume_r;

				sound->position += sound->advance_delta;

				if (sound->position >= sound->frames)
				{
					if (sound->looping)
					{
						sound->position = fmod(sound->position, (double)sound->frames);
					}
					else
					{
						sound->playing = false;
						sound->position = 0.0;
						break;
					}
				}
			}
		}
	}
}
