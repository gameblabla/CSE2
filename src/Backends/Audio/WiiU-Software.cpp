#include "../Audio.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <coreinit/cache.h>
#include <coreinit/mutex.h>
#include <coreinit/thread.h>
#include <sndcore2/core.h>
#include <sndcore2/voice.h>
#include <sndcore2/drcvs.h>

#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

static void (*organya_callback)(void);
static unsigned int organya_callback_milliseconds;

static unsigned long ticks_per_second;

static OSMutex sound_list_mutex;
static OSMutex organya_mutex;

static AXVoice *voices[2];

static short *stream_buffers[2];
static float *stream_buffer_float;
static size_t buffer_length;
static unsigned long output_frequency;

static unsigned long GetTicksMilliseconds(void)
{
	static uint64_t accumulator;

	static unsigned long last_tick;

	unsigned long current_tick = OSGetTick();

	accumulator += current_tick - last_tick;

	last_tick = current_tick;

	return (accumulator * 1000) / ticks_per_second;
}

static void Callback(void *output_stream, size_t frames_total)
{

	float *stream = (float*)output_stream;

	OSLockMutex(&organya_mutex);

	if (organya_callback_milliseconds == 0)
	{
		OSLockMutex(&sound_list_mutex);
		Mixer_MixSounds(stream, frames_total);
		OSUnlockMutex(&sound_list_mutex);
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

			OSLockMutex(&sound_list_mutex);
			Mixer_MixSounds(stream + frames_done * 2, frames_to_do);
			OSUnlockMutex(&sound_list_mutex);

			frames_done += frames_to_do;
			organya_countdown -= frames_to_do;
		}
	}

	OSUnlockMutex(&organya_mutex);
}

static void FrameCallback(void)
{
	static int last_half = 1;

	unsigned int half;

	// Just assume both voices are in-sync

	AXVoiceOffsets offsets;

	AXGetVoiceOffsets(voices[0], &offsets);

	if (offsets.currentOffset > (buffer_length / 2))
	{
		half = 1;
	}
	else
	{
		half = 0;
	}

	if (half != last_half)
	{
		for (unsigned int i = 0; i < buffer_length / 2; ++i)
		{
			stream_buffer_float[i * 2 + 0] = 0.0f;
			stream_buffer_float[i * 2 + 1] = 0.0f;
		}

		Callback(stream_buffer_float, buffer_length / 2);

		for (unsigned int i = 0; i < buffer_length / 2; ++i)
		{
			for (unsigned int j = 0; j < 2; ++j)
			{
				float sample = stream_buffer_float[(i * 2) + j];

				if (sample < -1.0f)
					sample = -1.0f;
				else if (sample > 1.0f)
					sample = 1.0f;

				stream_buffers[j][((buffer_length / 2) * last_half) + i] = sample * 32767.0f;
			}
		}

		DCStoreRange(&stream_buffers[0][(buffer_length / 2) * last_half], buffer_length / 2 * sizeof(short));
		DCStoreRange(&stream_buffers[1][(buffer_length / 2) * last_half], buffer_length / 2 * sizeof(short));

		last_half = half;
	}
}

bool AudioBackend_Init(void)
{
	if (!AXIsInit())
	{
		AXInitParams initparams = {
			.renderer = AX_INIT_RENDERER_48KHZ,
			.pipeline = AX_INIT_PIPELINE_SINGLE,
		};

		AXInitWithParams(&initparams);
	}

	OSInitMutex(&sound_list_mutex);
	OSInitMutex(&organya_mutex);

	output_frequency = AXGetInputSamplesPerSec();

	Mixer_Init(output_frequency);

	buffer_length = output_frequency / 100;	// 10ms buffer

	stream_buffer_float = (float*)malloc(buffer_length * sizeof(float) * 2);

	if (stream_buffer_float != NULL)
	{
		stream_buffers[0] = (short*)malloc(buffer_length * sizeof(short));

		if (stream_buffers[0] != NULL)
		{
			stream_buffers[1] = (short*)malloc(buffer_length * sizeof(short));

			if (stream_buffers[1] != NULL)
			{
				voices[0] = AXAcquireVoice(31, NULL, NULL);

				if (voices[0] != NULL)
				{
					voices[1] = AXAcquireVoice(31, NULL, NULL);

					if (voices[1] != NULL)
					{
						for (unsigned int i = 0; i < 2; ++i)
						{
							AXVoiceBegin(voices[i]);

							AXSetVoiceType(voices[i], 0);

							AXVoiceVeData vol = {.volume = 0x8000};
							AXSetVoiceVe(voices[i], &vol);

							AXVoiceDeviceMixData mix_data[6];
							memset(mix_data, 0, sizeof(mix_data));
							mix_data[0].bus[0].volume = i == 0 ? 0x8000 : 0;
							mix_data[1].bus[0].volume = i == 1 ? 0x8000 : 0;

							AXSetVoiceDeviceMix(voices[i], AX_DEVICE_TYPE_DRC, 0, mix_data);
							AXSetVoiceDeviceMix(voices[i], AX_DEVICE_TYPE_TV, 0, mix_data);

							AXSetVoiceSrcRatio(voices[i], 1.0f);	// We use the native sample rate
							AXSetVoiceSrcType(voices[i], AX_VOICE_SRC_TYPE_NONE);


							AXVoiceOffsets offs;
							offs.dataType = AX_VOICE_FORMAT_LPCM16;
							offs.endOffset = buffer_length;
							offs.loopingEnabled = AX_VOICE_LOOP_ENABLED;
							offs.loopOffset = 0;
							offs.currentOffset = 0;
							offs.data = stream_buffers[i];
							AXSetVoiceOffsets(voices[i], &offs);

							AXSetVoiceState(voices[i], AX_VOICE_STATE_PLAYING);

							AXVoiceEnd(voices[i]);
						}

						AXRegisterAppFrameCallback(FrameCallback);

						return true;
					}

					AXFreeVoice(voices[0]);
				}

				free(stream_buffers[1]);
			}

			free(stream_buffers[0]);
		}

		free(stream_buffer_float);
	}

	return false;
}

void AudioBackend_Deinit(void)
{
	for (unsigned int i = 0; i < 2; ++i)
	{
		AXFreeVoice(voices[i]);
		free(stream_buffers[i]);
	}

	AXQuit();
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	OSLockMutex(&sound_list_mutex);

	Mixer_Sound *sound = Mixer_CreateSound(frequency, samples, length);

	OSUnlockMutex(&sound_list_mutex);

	return (AudioBackend_Sound*)sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_DestroySound((Mixer_Sound*)sound);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_PlaySound((Mixer_Sound*)sound, looping);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_StopSound((Mixer_Sound*)sound);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_RewindSound((Mixer_Sound*)sound);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_SetSoundFrequency((Mixer_Sound*)sound, frequency);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_SetSoundVolume((Mixer_Sound*)sound, volume);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	Mixer_SetSoundPan((Mixer_Sound*)sound, pan);

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetOrganyaCallback(void (*callback)(void))
{
	// As far as thread-safety goes - this is guarded by
	// `organya_milliseconds`, which is guarded by `organya_mutex`.
	organya_callback = callback;
}

void AudioBackend_SetOrganyaTimer(unsigned int milliseconds)
{
	OSLockMutex(&organya_mutex);

	organya_callback_milliseconds = milliseconds;

	OSUnlockMutex(&organya_mutex);
}
