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
	AXVoice *voice;
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
			for (size_t i = 0; i < length; ++i)
				samples_copy[i] = samples[i] - 0x80;

			DCStoreRange(samples_copy, length);

			AXVoice *voice = AXAcquireVoice(31, NULL, NULL);

			if (voice != NULL)
			{
				AXVoiceOffsets offs;
				AXVoiceVeData vol = {
					.volume = 0x8000,
				};

				AXVoiceBegin(voice);

				AXSetVoiceType(voice, 0);
				AXSetVoiceVe(voice, &vol);

				static AXVoiceDeviceMixData mix_data[1][6];
				mix_data[0][0].bus[0].volume = 0x8000;
				mix_data[0][1].bus[0].volume = 0x8000;

				AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, mix_data[0]);
				AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, mix_data[0]);

				float srcratio = (float)frequency / (float)AXGetInputSamplesPerSec();
				AXSetVoiceSrcRatio(voice, srcratio);
				AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

				offs.dataType = AX_VOICE_FORMAT_LPCM8;
				offs.endOffset = length;
				offs.loopingEnabled = AX_VOICE_LOOP_DISABLED;
				offs.loopOffset = 0;
				offs.currentOffset = 0;
				offs.data = samples_copy;
				AXSetVoiceOffsets(voice, &offs);

				AXVoiceEnd(voice);

				sound->samples = samples_copy;
				sound->voice = voice;

				return sound;
			}

			free(samples_copy);
		}

		free(sound);
	}

	return NULL;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	AXFreeVoice(sound->voice);
	free(sound->samples);
	free(sound);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	AXSetVoiceLoop(sound->voice, looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED);
	AXSetVoiceState(sound->voice, AX_VOICE_STATE_PLAYING);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	AXSetVoiceState(sound->voice, AX_VOICE_STATE_STOPPED);
	AXSetVoiceCurrentOffset(sound->voice, 0);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	AXSetVoiceCurrentOffset(sound->voice, 0);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	float srcratio = (float)frequency / (float)AXGetInputSamplesPerSec();
	AXSetVoiceSrcRatio(sound->voice, srcratio);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	AXVoiceVeData vol = {
		.volume = (unsigned short)(0x8000 * MillibelToScale(volume)),
	};

	AXSetVoiceVe(sound->voice, &vol);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	static AXVoiceDeviceMixData mix_data[1][6];
	mix_data[0][0].bus[0].volume = (unsigned short)(0x8000 * MillibelToScale(-pan));
	mix_data[0][1].bus[0].volume = (unsigned short)(0x8000 * MillibelToScale(pan));

	AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_DRC, 0, mix_data[0]);
	AXSetVoiceDeviceMix(sound->voice, AX_DEVICE_TYPE_TV, 0, mix_data[0]);
}

void AudioBackend_SetOrganyaCallback(void (*callback)(void), unsigned int milliseconds)
{
	(void)callback;
	(void)milliseconds;
}
