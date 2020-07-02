#include "../Audio.h"

#include <stddef.h>
#include <string.h>

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_API static
#include "../../../external/miniaudio.h"

#ifdef EXTRA_SOUND_FORMATS
#include "../../ExtraSoundFormats.h"
#endif
#include "../Misc.h"

#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static ma_context context;
static ma_device device;
static ma_mutex mutex;
static ma_mutex organya_mutex;

static unsigned long output_frequency;

static void (*organya_callback)(void);
static unsigned int organya_callback_milliseconds;

static void MixSoundsAndUpdateOrganya(long *stream, size_t frames_total)
{
	ma_mutex_lock(&organya_mutex);

	if (organya_callback_milliseconds == 0)
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
				organya_countdown = (organya_callback_milliseconds * output_frequency) / 1000;	// organya_timer is in milliseconds, so convert it to audio frames
				organya_callback();
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

static void Callback(ma_device *device, void *output_stream, const void *input_stream, ma_uint32 frames_total)
{
	(void)device;
	(void)input_stream;

	short *stream = (short*)output_stream;

	size_t frames_done = 0;

	while (frames_done != frames_total)
	{
		long mix_buffer[0x800 * 2];	// 2 because stereo

		size_t subframes = MIN(0x800, frames_total - frames_done);

		memset(mix_buffer, 0, subframes * sizeof(long) * 2);

		MixSoundsAndUpdateOrganya(mix_buffer, subframes);

	#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Mix(mix_buffer, subframes);
	#endif

		for (size_t i = 0; i < subframes * 2; ++i)
		{
			if (mix_buffer[i] > 0x7FFF)
				*stream++ = 0x7FFF;
			else if (mix_buffer[i] < -0x7FFF)
				*stream++ = -0x7FFF;
			else
				*stream++ = mix_buffer[i];
		}

		frames_done += subframes;
	}
}

bool AudioBackend_Init(void)
{
	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.pDeviceID = NULL;
	config.playback.format = ma_format_s16;
	config.playback.channels = 2;
	config.sampleRate = 0;	// Let miniaudio decide what sample rate to use
	config.dataCallback = Callback;
	config.pUserData = NULL;

	ma_result return_value;

	return_value = ma_context_init(NULL, 0, NULL, &context);

	if (return_value == MA_SUCCESS)
	{
		return_value = ma_device_init(&context, &config, &device);

		if (return_value == MA_SUCCESS)
		{
			return_value = ma_mutex_init(&mutex);

			if (return_value == MA_SUCCESS)
			{
				return_value = ma_mutex_init(&organya_mutex);

				if (return_value == MA_SUCCESS)
				{
					return_value = ma_device_start(&device);

					if (return_value == MA_SUCCESS)
					{
						output_frequency = device.sampleRate;

					#ifdef EXTRA_SOUND_FORMATS
						ExtraSound_Init(device.sampleRate);
					#endif

						Mixer_Init(device.sampleRate);

						return true;
					}
					else
					{
						Backend_PrintError("Failed to start playback device: %s", ma_result_description(return_value));
					}

					ma_mutex_uninit(&organya_mutex);
				}
				else
				{
					Backend_PrintError("Failed to create organya mutex: %s", ma_result_description(return_value));
				}

				ma_mutex_uninit(&mutex);
			}
			else
			{
				Backend_PrintError("Failed to create mutex: %s", ma_result_description(return_value));
			}

			ma_device_uninit(&device);
		}
		else
		{
			Backend_PrintError("Failed to initialize playback device: %s", ma_result_description(return_value));
		}

		ma_context_uninit(&context);
	}
	else
	{
		Backend_PrintError("Failed to initialize context: %s", ma_result_description(return_value));
	}


	return false;
}

void AudioBackend_Deinit(void)
{
	ma_result return_value = ma_device_stop(&device);

	if (return_value != MA_SUCCESS)
		Backend_PrintError("Failed to stop playback device: %s", ma_result_description(return_value));

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

	ma_mutex_uninit(&organya_mutex);

	ma_mutex_uninit(&mutex);

	ma_device_uninit(&device);

	ma_context_uninit(&context);
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	ma_mutex_lock(&mutex);

	Mixer_Sound *sound = Mixer_CreateSound(frequency, samples, length);

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

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
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

void AudioBackend_SetOrganyaCallback(void (*callback)(void))
{
	ma_mutex_lock(&organya_mutex);

	organya_callback = callback;

	ma_mutex_unlock(&organya_mutex);
}

void AudioBackend_SetOrganyaTimer(unsigned int milliseconds)
{
	ma_mutex_lock(&organya_mutex);

	organya_callback_milliseconds = milliseconds;

	ma_mutex_unlock(&organya_mutex);
}

void AudioBackend_Lock(void)
{
	ma_mutex_lock(&mutex);
}

void AudioBackend_Unlock(void)
{
	ma_mutex_unlock(&mutex);
}
