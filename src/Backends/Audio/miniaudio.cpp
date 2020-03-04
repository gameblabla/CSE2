#include "../Audio.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MA_NO_DECODING
#include "miniaudio.h"

#ifdef EXTRA_SOUND_FORMATS
#include "../../ExtraSoundFormats.h"
#endif
#include "../../Organya.h"
#include "../../WindowsWrapper.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

#ifdef __GNUC__
#define ATTR_HOT __attribute__((hot))
#else
#define ATTR_HOT
#endif

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
static ma_device device;
static ma_mutex mutex;
static ma_mutex organya_mutex;

static unsigned long output_frequency;

static unsigned short organya_timer;

static double MillibelToScale(long volume)
{
	// Volume is in hundredths of decibels, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
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

// Most CPU-intensive function in the game (2/3rd CPU time consumption in my experience), so marked with attrHot so the compiler considers it a hot spot (as it is) when optimizing
ATTR_HOT static void MixSounds(float *stream, unsigned int frames_total)
{
	ma_mutex_lock(&mutex);

	for (AudioBackend_Sound *sound = sound_list_head; sound != NULL; sound = sound->next)
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
						sound->playing = FALSE;
						sound->position = 0.0;
						break;
					}
				}
			}
		}
	}

	ma_mutex_unlock(&mutex);

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Mix(stream, frames_total);
#endif
}

static void Callback(ma_device *device, void *output_stream, const void *input_stream, ma_uint32 frames_total)
{
	(void)device;
	(void)input_stream;

	float *stream = (float*)output_stream;

	ma_mutex_lock(&organya_mutex);

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

	ma_mutex_unlock(&organya_mutex);
}

BOOL AudioBackend_Init(void)
{
	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.pDeviceID = NULL;
	config.playback.format = ma_format_f32;
	config.playback.channels = 2;
	config.sampleRate = 44100;
	config.dataCallback = Callback;
	config.pUserData = NULL;

	output_frequency = 44100;

	if (ma_device_init(NULL, &config, &device) == MA_SUCCESS)
	{
		if (ma_mutex_init(device.pContext, &mutex) == MA_SUCCESS)
		{
			if (ma_mutex_init(device.pContext, &organya_mutex) == MA_SUCCESS)
			{
			#ifdef EXTRA_SOUND_FORMATS
				ExtraSound_Init(output_frequency);
			#endif

				if (ma_device_start(&device) == MA_SUCCESS)
					return TRUE;

			#ifdef EXTRA_SOUND_FORMATS
				ExtraSound_Deinit();
			#endif

				ma_mutex_uninit(&organya_mutex);
			}

			ma_mutex_uninit(&mutex);
		}

		ma_device_uninit(&device);
	}

	return FALSE;
}

void AudioBackend_Deinit(void)
{
	ma_device_stop(&device);

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

	ma_mutex_uninit(&organya_mutex);

	ma_mutex_uninit(&mutex);

	ma_device_uninit(&device);
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

	ma_mutex_lock(&mutex);

	sound->next = sound_list_head;
	sound_list_head = sound;

	ma_mutex_unlock(&mutex);

	return sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

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

	ma_mutex_unlock(&mutex);
}

unsigned char* AudioBackend_LockSound(AudioBackend_Sound *sound, size_t *size)
{
	if (sound == NULL)
		return NULL;

	ma_mutex_lock(&mutex);

	if (size != NULL)
		*size = sound->frames;

	return sound->samples;
}

void AudioBackend_UnlockSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_unlock(&mutex);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, BOOL looping)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->playing = TRUE;
	sound->looping = looping;

	sound->samples[sound->frames] = looping ? sound->samples[0] : 0x80;	// For the linear interpolator

	ma_mutex_unlock(&mutex);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->playing = FALSE;
	sound->position = 0.0;

	ma_mutex_unlock(&mutex);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->position = 0.0;

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	SetSoundFrequency(sound, frequency);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	SetSoundVolume(sound, volume);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	SetSoundPan(sound, pan);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetOrganyaTimer(unsigned short timer)
{
	ma_mutex_lock(&organya_mutex);

	organya_timer = timer;

	ma_mutex_unlock(&organya_mutex);
}
