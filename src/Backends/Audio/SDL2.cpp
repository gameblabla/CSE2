#include "../Audio.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#ifdef EXTRA_SOUND_FORMATS
#include "../../ExtraSoundFormats.h"
#endif
#include "../../Organya.h"
#include "../../WindowsWrapper.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define clamp(x, y, z) MIN(MAX((x), (y)), (z))

struct AudioBackend_Sound
{
	unsigned char *samples;
	size_t frames;
	double position;
	double advance_delta;
	BOOL playing;
	BOOL looping;
	unsigned int frequency;
	float volume;
	float pan_l;
	float pan_r;
	float volume_l;
	float volume_r;

	struct AudioBackend_Sound *next;
};

static AudioBackend_Sound *sound_list_head;
static SDL_AudioDeviceID device_id;

static unsigned long output_frequency;

static double MillibelToScale(long volume)
{
	// Volume is in hundredths of decibels, from 0 to -10000
	volume = clamp(volume, -10000, 0);
	return pow(10.0, volume / 2000.0);
}

static void SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	sound->frequency = frequency;
	sound->advance_delta = (double)frequency / (double)output_frequency;
}

static void SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	sound->volume = (float)MillibelToScale(volume);

	sound->volume_l = sound->pan_l * sound->volume;
	sound->volume_r = sound->pan_r * sound->volume;
}

static void SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	sound->pan_l = (float)MillibelToScale(-pan);
	sound->pan_r = (float)MillibelToScale(pan);

	sound->volume_l = sound->pan_l * sound->volume;
	sound->volume_r = sound->pan_r * sound->volume;
}

static void MixSounds(float *stream, unsigned int frames_total)
{
	for (AudioBackend_Sound *sound = sound_list_head; sound != NULL; sound = sound->next)
	{
		if (sound->playing)
		{
			float *steam_pointer = stream;

			unsigned int frames_done = 0;

			while (frames_done != frames_total)
			{
				const unsigned int frames_to_do = MIN((unsigned int)ceil((sound->frames - sound->position) / sound->advance_delta), frames_total - frames_done);

				for (unsigned int i = 0; i < frames_to_do; ++i)
				{
					// Get two samples, and normalise them to 0-1
					const float sample1 = (sound->samples[(size_t)sound->position] - 128.0f) / 128.0f;
					const float sample2 = (sound->samples[(size_t)sound->position + 1] - 128.0f) / 128.0f;

					// Perform linear interpolation
					const float interpolated_sample = sample1 + ((sample2 - sample1) * (float)fmod(sound->position, 1.0));

					*steam_pointer++ += interpolated_sample * sound->volume_l;
					*steam_pointer++ += interpolated_sample * sound->volume_r;

					sound->position += sound->advance_delta;
				}

				if ((size_t)sound->position >= sound->frames)
				{
					if (sound->looping)
					{
						sound->position = fmod(sound->position, (double)sound->frames);
					}
					else
					{
						sound->playing = FALSE;
						sound->position = 0.0;
						break;
					}
				}

				frames_done += frames_to_do;
			}
		}
	}

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Mix(stream, frames_total);
#endif
}

static void Callback(void *user_data, Uint8 *stream_uint8, int len)
{
	(void)user_data;

	float *stream = (float*)stream_uint8;
	unsigned int frames_total = len / sizeof(float) / 2;

	for (unsigned int i = 0; i < frames_total * 2; ++i)
		stream[i] = 0.0f;

	if (organya_timer == 0)
	{
		MixSounds(stream, frames_total);
	}
	else
	{
		// Synchronise audio generation with Organya.
		// In the original game, Organya ran asynchronously in a separate thread,
		// firing off commands to DirectSound in realtime. To match that, we'd
		// need a very low-latency buffer, otherwise we'd get mistimed instruments.
		// Instead, we can just do this.
		unsigned int frames_done = 0;

		while (frames_done != frames_total)
		{
			static unsigned long organya_countdown;

			if (organya_countdown == 0)
			{
				organya_countdown = (organya_timer * output_frequency) / 1000;	// organya_timer is in milliseconds, so convert it to audio frames
				UpdateOrganya();
			}

			const unsigned int frames_to_do = MIN(organya_countdown, frames_total - frames_done);

			MixSounds(stream + frames_done * 2, frames_to_do);

			frames_done += frames_to_do;
			organya_countdown -= frames_to_do;
		}
	}
}

BOOL AudioBackend_Init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		return FALSE;

	SDL_AudioSpec specification;
	specification.freq = 44100;
	specification.format = AUDIO_F32;
	specification.channels = 2;
	specification.samples = 0x400;	// Roughly 10 milliseconds for 44100Hz
	specification.callback = Callback;
	specification.userdata = NULL;

	SDL_AudioSpec obtained_specification;
	device_id = SDL_OpenAudioDevice(NULL, 0, &specification, &obtained_specification, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	output_frequency = obtained_specification.freq;

	if (device_id == 0)
		return FALSE;

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Init(output_frequency);
#endif

	SDL_PauseAudioDevice(device_id, 0);

	return TRUE;
}

void AudioBackend_Deinit(void)
{
#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

	SDL_CloseAudioDevice(device_id);

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, size_t frames)
{
	AudioBackend_Sound *sound = (AudioBackend_Sound*)malloc(sizeof(AudioBackend_Sound));

	if (sound == NULL)
		return NULL;

	sound->samples = (unsigned char*)malloc(frames + 1);

	if (sound->samples == NULL)
	{
		free(sound);
		return NULL;
	}

	sound->frames = frames;
	sound->playing = FALSE;
	sound->position = 0.0;

	SetSoundFrequency(sound, frequency);
	SetSoundVolume(sound, 0);
	SetSoundPan(sound, 0);

	SDL_LockAudioDevice(device_id);

	sound->next = sound_list_head;
	sound_list_head = sound;

	SDL_UnlockAudioDevice(device_id);

	return sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	for (AudioBackend_Sound **sound_pointer = &sound_list_head; *sound_pointer != NULL; sound_pointer = &(*sound_pointer)->next)
	{
		if (*sound_pointer == sound)
		{
			*sound_pointer = sound->next;
			free(sound->samples);
			free(sound);
			break;
		}
	}

	SDL_UnlockAudioDevice(device_id);
}

unsigned char* AudioBackend_LockSound(AudioBackend_Sound *sound, size_t *size)
{
	if (sound == NULL)
		return NULL;

	SDL_LockAudioDevice(device_id);

	if (size != NULL)
		*size = sound->frames;

	return sound->samples;
}

void AudioBackend_UnlockSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_UnlockAudioDevice(device_id);	
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, BOOL looping)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	sound->playing = TRUE;
	sound->looping = looping;

	sound->samples[sound->frames] = looping ? sound->samples[0] : 0x80;	// For the linear interpolator

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	sound->playing = FALSE;
	sound->position = 0.0;

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	sound->position = 0.0;

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	SetSoundFrequency(sound, frequency);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	SetSoundVolume(sound, volume);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	SetSoundPan(sound, pan);

	SDL_UnlockAudioDevice(device_id);
}
