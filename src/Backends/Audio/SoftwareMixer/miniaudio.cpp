// Released under the MIT licence.
// See LICENCE.txt for details.

#include "Backend.h"

#include <stddef.h>
#include <string.h>

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_API static
#include "../../../../external/miniaudio.h"

#include "../../Misc.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void (*parent_callback)(long *stream, size_t frames_total);

static ma_context context;
static ma_device device;
static ma_mutex mutex;
static ma_mutex organya_mutex;

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

		parent_callback(mix_buffer, subframes);

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

unsigned long SoftwareMixerBackend_Init(void (*callback)(long *stream, size_t frames_total))
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
					parent_callback = callback;

					return device.sampleRate;
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

	return 0;
}

void SoftwareMixerBackend_Deinit(void)
{
	ma_result return_value = ma_device_stop(&device);

	if (return_value != MA_SUCCESS)
		Backend_PrintError("Failed to stop playback device: %s", ma_result_description(return_value));

	ma_mutex_uninit(&organya_mutex);

	ma_mutex_uninit(&mutex);

	ma_device_uninit(&device);

	ma_context_uninit(&context);
}

bool SoftwareMixerBackend_Start(void)
{
	ma_result return_value = ma_device_start(&device);

	if (return_value != MA_SUCCESS)
	{
		Backend_PrintError("Failed to start playback device: %s", ma_result_description(return_value));
		return false;
	}

	return true;
}

void SoftwareMixerBackend_LockMixerMutex(void)
{
	ma_mutex_lock(&mutex);
}

void SoftwareMixerBackend_UnlockMixerMutex(void)
{
	ma_mutex_unlock(&mutex);
}

void SoftwareMixerBackend_LockOrganyaMutex(void)
{
	ma_mutex_lock(&organya_mutex);
}

void SoftwareMixerBackend_UnlockOrganyaMutex(void)
{
	ma_mutex_unlock(&organya_mutex);
}
