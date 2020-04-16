#include "../Audio.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <coreinit/cache.h>

#include <sndcore2/core.h>
#include <sndcore2/voice.h>
#include <sndcore2/drcvs.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

struct AudioBackend_Sound
{
	unsigned char *samples;
	size_t length;
	AXVoice *voice;
	unsigned int frequency;
	unsigned short volume;
	unsigned short pan_l;
	unsigned short pan_r;
};

static double MillibelToScale(long volume)
{
	// Volume is in hundredths of a decibel, from 0 to -10000
	volume = CLAMP(volume, -10000, 0);
	return pow(10.0, volume / 2000.0);
}

bool AudioBackend_Init(void)
{
	if (!AXIsInit())
	{
		static AXInitParams initparams =
		{
			.renderer = AX_INIT_RENDERER_48KHZ,
			.pipeline = AX_INIT_PIPELINE_SINGLE,
		};

		AXInitWithParams(&initparams);
	}

	return true;
}

void AudioBackend_Deinit(void)
{
	AXQuit();
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	AudioBackend_Sound *sound = (AudioBackend_Sound*)malloc(sizeof(AudioBackend_Sound));

	if (sound != NULL)
	{
		unsigned char *samples_copy = (unsigned char*)malloc(length);

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

			return sound;
		}

		free(sound);
	}

	return NULL;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	AudioBackend_StopSound(sound);

	free(sound->samples);
	free(sound);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	if (sound->voice == NULL)
	{
		AXVoice *voice = AXAcquireVoice(31, NULL, NULL);

		if (voice != NULL)
		{
			AXVoiceOffsets offs;
			AXVoiceVeData vol = {
				.volume = sound->volume,
			};

			AXVoiceBegin(voice);

			AXSetVoiceType(voice, 0);
			AXSetVoiceVe(voice, &vol);

			static AXVoiceDeviceMixData mix_data[1][6];
			mix_data[0][0].bus[0].volume = sound->pan_l;
			mix_data[0][1].bus[0].volume = sound->pan_r;

			AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, mix_data[0]);
			AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, mix_data[0]);

			float srcratio = (float)sound->frequency / (float)AXGetInputSamplesPerSec();
			AXSetVoiceSrcRatio(voice, srcratio);
			AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

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
		AXSetVoiceLoop(sound->voice, looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED);
		AXSetVoiceState(sound->voice, AX_VOICE_STATE_PLAYING);
	}
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound->voice != NULL)
	{
		AXFreeVoice(sound->voice);
		sound->voice = NULL;
	}
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound->voice != NULL)
		AXSetVoiceCurrentOffset(sound->voice, 0);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	sound->frequency = frequency;

	if (sound->voice != NULL)
	{
		float srcratio = (float)frequency / (float)AXGetInputSamplesPerSec();
		AXSetVoiceSrcRatio(sound->voice, srcratio);
	}
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	sound->volume = (unsigned short)(0x8000 * MillibelToScale(volume));

	if (sound->voice != NULL)
	{
		AXVoiceVeData vol = {
			.volume = sound->volume,
		};

		AXSetVoiceVe(sound->voice, &vol);
	}
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	static AXVoiceDeviceMixData mix_data[1][6];
	mix_data[0][0].bus[0].volume = (unsigned short)(0x8000 * MillibelToScale(-pan));
	mix_data[0][1].bus[0].volume = (unsigned short)(0x8000 * MillibelToScale(pan));

	if (sound->voice != NULL)
	{
		AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_DRC, 0, mix_data[0]);
		AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_TV, 0, mix_data[0]);
	}
}

void AudioBackend_SetOrganyaCallback(void (*callback)(void), unsigned int milliseconds)
{
	(void)callback;
	(void)milliseconds;
}
