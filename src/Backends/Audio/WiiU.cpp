// Released under the MIT licence.
// See LICENCE.txt for details.

// This darned thing doesn't work - sounds just randomly refuse to play,
// particularly the ones used by Organya.

// If anyone could figure out what causes this, that would be great.

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

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

struct AudioBackend_Sound
{
	signed char *samples;
	size_t length;
	AXVoice *voice;
	unsigned int frequency;
	unsigned short volume;
	unsigned short pan_l;
	unsigned short pan_r;

	struct AudioBackend_Sound *next;
};

static void (*organya_callback)(void);
static unsigned int organya_milliseconds;

static unsigned long ticks_per_second;

static OSMutex sound_list_mutex;
static OSMutex organya_mutex;

static AudioBackend_Sound *sound_list_head;

static void CullVoices(void)
{
	// Free any voices that aren't playing anymore
	OSLockMutex(&sound_list_mutex);

	for (AudioBackend_Sound *sound = sound_list_head; sound != NULL; sound = sound->next)
	{
		if (sound->voice != NULL)
		{
			if (!AXIsVoiceRunning(sound->voice))
			{
				AXVoiceBegin(sound->voice);
				AXFreeVoice(sound->voice);
				AXVoiceEnd(sound->voice);
				sound->voice = NULL;
			}
		}
	}

	OSUnlockMutex(&sound_list_mutex);
}

static double MillibelToScale(long volume)
{
	// Volume is in hundredths of a decibel, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
	return pow(10.0, volume / 2000.0);
}

static unsigned long GetTicksMilliseconds(void)
{
	static uint64_t accumulator;

	static unsigned long last_tick;

	unsigned long current_tick = OSGetTick();

	accumulator += current_tick - last_tick;

	last_tick = current_tick;

	return (accumulator * 1000) / ticks_per_second;
}

static int ThreadFunction(int argc, const char *argv[])
{
	for (;;)
	{
		OSTestThreadCancel();

		OSLockMutex(&organya_mutex);

		if (organya_milliseconds == 0)
		{
			OSUnlockMutex(&organya_mutex);

			// Do nothing
			OSSleepTicks(ticks_per_second / 1000);
		}
		else
		{
			OSUnlockMutex(&organya_mutex);

			// Update Organya
			static unsigned long next_ticks;

			for (;;)
			{
				unsigned long ticks = GetTicksMilliseconds();

				if (ticks >= next_ticks)
					break;

				OSSleepTicks(ticks_per_second / 1000);
			}

			OSLockMutex(&organya_mutex);
			next_ticks += organya_milliseconds;
			OSUnlockMutex(&organya_mutex);

			OSLockMutex(&sound_list_mutex);
			organya_callback();
			OSUnlockMutex(&sound_list_mutex);
		}
	}

	return 0;
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

	ticks_per_second = OSGetSystemInfo()->busClockSpeed / 4;

	OSInitMutex(&sound_list_mutex);
	OSInitMutex(&organya_mutex);

	OSRunThread(OSGetDefaultThread(0), ThreadFunction, 0, NULL);

	return true;
}

void AudioBackend_Deinit(void)
{
	OSCancelThread(OSGetDefaultThread(0));

	OSJoinThread(OSGetDefaultThread(0), NULL);

	AXQuit();
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	AudioBackend_Sound *sound = (AudioBackend_Sound*)malloc(sizeof(AudioBackend_Sound));

	if (sound != NULL)
	{
		signed char *samples_copy = (signed char*)malloc(length);

		if (samples_copy != NULL)
		{
			// Convert to signed
			for (size_t i = 0; i < length; ++i)
				samples_copy[i] = samples[i] - 0x80;

			DCStoreRange(samples_copy, length);

			sound->samples = samples_copy;
			sound->length = length;
			sound->voice = NULL;
			sound->frequency = frequency;
			sound->volume = 0x8000;
			sound->pan_l = 0x8000;
			sound->pan_r = 0x8000;

			OSLockMutex(&sound_list_mutex);
			sound->next = sound_list_head;
			sound_list_head = sound;
			OSUnlockMutex(&sound_list_mutex);

			return sound;
		}

		free(sound);
	}

	return NULL;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	// Unhook sound from the linked-list
	for (AudioBackend_Sound **sound_pointer = &sound_list_head; *sound_pointer != NULL; sound_pointer = &(*sound_pointer)->next)
	{
		if (*sound_pointer == sound)
		{
			*sound_pointer = sound->next;
			break;
		}
	}

	OSUnlockMutex(&sound_list_mutex);

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);
		AXFreeVoice(sound->voice);
		AXVoiceEnd(sound->voice);
	}

	free(sound->samples);
	free(sound);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	if (sound == NULL)
		return;

	CullVoices();

	OSLockMutex(&sound_list_mutex);

	if (sound->voice == NULL)
	{
		AXVoice *voice = AXAcquireVoice(31, NULL, NULL);

		if (voice != NULL)
		{
			AXVoiceBegin(voice);

			AXSetVoiceType(voice, 0);

			AXVoiceVeData vol = {.volume = sound->volume};
			AXSetVoiceVe(voice, &vol);

			AXVoiceDeviceMixData mix_data[6];
			memset(mix_data, 0, sizeof(mix_data));
			mix_data[0].bus[0].volume = sound->pan_l;
			mix_data[1].bus[0].volume = sound->pan_r;

			AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, mix_data);
			AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, mix_data);

			float srcratio = (float)sound->frequency / (float)AXGetInputSamplesPerSec();
			AXSetVoiceSrcRatio(voice, srcratio);
			AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

			AXVoiceOffsets offs;
			offs.dataType = AX_VOICE_FORMAT_LPCM8;
			offs.endOffset = sound->length;
			offs.loopingEnabled = AX_VOICE_LOOP_DISABLED;
			offs.loopOffset = 0;
			offs.currentOffset = 0;
			offs.data = sound->samples;
			AXSetVoiceOffsets(voice, &offs);

			AXVoiceEnd(voice);

			sound->voice = voice;
		}
	}

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		AXSetVoiceLoop(sound->voice, looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED);
		AXSetVoiceState(sound->voice, AX_VOICE_STATE_PLAYING);

		AXVoiceEnd(sound->voice);
	}

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		AXSetVoiceState(sound->voice, AX_VOICE_STATE_STOPPED);

		AXVoiceEnd(sound->voice);
	}

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		AXSetVoiceCurrentOffset(sound->voice, 0);

		AXVoiceEnd(sound->voice);
	}

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	sound->frequency = frequency;

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		float srcratio = (float)frequency / (float)AXGetInputSamplesPerSec();
		AXSetVoiceSrcRatio(sound->voice, srcratio);

		AXVoiceEnd(sound->voice);
	}

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	sound->volume = (unsigned short)(0x8000 * MillibelToScale(volume));

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		AXVoiceVeData vol = {.volume = sound->volume};

		AXSetVoiceVe(sound->voice, &vol);

		AXVoiceEnd(sound->voice);
	}

	OSUnlockMutex(&sound_list_mutex);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	OSLockMutex(&sound_list_mutex);

	sound->pan_l = (unsigned short)(0x8000 * MillibelToScale(-pan));
	sound->pan_r = (unsigned short)(0x8000 * MillibelToScale(pan));

	if (sound->voice != NULL)
	{
		AXVoiceBegin(sound->voice);

		AXVoiceDeviceMixData mix_data[6];
		memset(mix_data, 0, sizeof(mix_data));
		mix_data[0].bus[0].volume = sound->pan_l;
		mix_data[1].bus[0].volume = sound->pan_r;

		AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_DRC, 0, mix_data);
		AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_TV, 0, mix_data);

		AXVoiceEnd(sound->voice);
	}

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

	organya_milliseconds = milliseconds;

	OSUnlockMutex(&organya_mutex);
}
