#include "../Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_API static
#include "../../../external/miniaudio.h"

#include "../../Organya.h"
#include "../../WindowsWrapper.h"

#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static ma_device device;
static ma_mutex mutex;
static ma_mutex organya_mutex;

static unsigned long output_frequency;

static unsigned short organya_timer;

static void Callback(ma_device *device, void *output_stream, const void *input_stream, ma_uint32 frames_total)
{
	(void)device;
	(void)input_stream;

	float *stream = (float*)output_stream;

	ma_mutex_lock(&organya_mutex);

	if (organya_timer == 0)
	{
		ma_mutex_lock(&mutex);
		Mixer_MixSounds(stream, frames_total);
		ma_mutex_unlock(&mutex);
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

			ma_mutex_lock(&mutex);
			Mixer_MixSounds(stream + frames_done * 2, frames_to_do);
			ma_mutex_unlock(&mutex);

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
	config.sampleRate = 0;
	config.dataCallback = Callback;
	config.pUserData = NULL;

	if (ma_device_init(NULL, &config, &device) == MA_SUCCESS)
	{
		if (ma_mutex_init(device.pContext, &mutex) == MA_SUCCESS)
		{
			if (ma_mutex_init(device.pContext, &organya_mutex) == MA_SUCCESS)
			{
				if (ma_device_start(&device) == MA_SUCCESS)
				{
					output_frequency = device.sampleRate;

					Mixer_Init(device.sampleRate);

					return TRUE;
				}

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

	ma_mutex_uninit(&organya_mutex);

	ma_mutex_uninit(&mutex);

	ma_device_uninit(&device);
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, size_t frames)
{
	ma_mutex_lock(&mutex);

	Mixer_Sound *sound = Mixer_CreateSound(frequency, frames);

	ma_mutex_unlock(&mutex);

	return (AudioBackend_Sound*)sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_DestroySound((Mixer_Sound*)sound);

	ma_mutex_unlock(&mutex);
}

unsigned char* AudioBackend_LockSound(AudioBackend_Sound *sound, size_t *size)
{
	if (sound == NULL)
		return NULL;

	ma_mutex_lock(&mutex);

	return Mixer_LockSound((Mixer_Sound*)sound, size);
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

	Mixer_PlaySound((Mixer_Sound*)sound, looping);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_StopSound((Mixer_Sound*)sound);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_RewindSound((Mixer_Sound*)sound);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_SetSoundFrequency((Mixer_Sound*)sound, frequency);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_SetSoundVolume((Mixer_Sound*)sound, volume);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	Mixer_SetSoundPan((Mixer_Sound*)sound, pan);

	ma_mutex_unlock(&mutex);
}

void AudioBackend_SetOrganyaTimer(unsigned short timer)
{
	ma_mutex_lock(&organya_mutex);

	organya_timer = timer;

	ma_mutex_unlock(&organya_mutex);
}
