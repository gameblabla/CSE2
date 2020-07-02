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

#ifdef EXTRA_SOUND_FORMATS
#include "../../ExtraSoundFormats.h"
#endif

#include "SoftwareMixer.h"

#define AUDIO_BUFFERS 2	// Double-buffer

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

static void (*organya_callback)(void);
static unsigned int organya_callback_milliseconds;

static OSMutex sound_list_mutex;
static OSMutex organya_mutex;

static AXVoice *voices[2];

static short *stream_buffers[2];
static long *stream_buffer_long;
static size_t buffer_length;

static unsigned long output_frequency;

static void MixSoundsAndUpdateOrganya(long *stream, size_t frames_total)
{
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
	// We use a double-buffer: while the Wii U is busy playing one half of the buffer, we update the other.
	// The buffer is 10ms long in total, and this function runs every 3ms.

	// Just assume both voices are in-sync, and only check the first one
	AXVoiceOffsets offsets;
	AXGetVoiceOffsets(voices[0], &offsets);

	unsigned int current_buffer = offsets.currentOffset / buffer_length;

	static unsigned int last_buffer = 1;

	if (current_buffer != last_buffer)
	{
		// Clear the mixer buffer
		memset(stream_buffer_long, 0, buffer_length * sizeof(long) * 2);

		// Fill mixer buffer
		MixSoundsAndUpdateOrganya(stream_buffer_long, buffer_length);

	#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Mix(stream_buffer_long, buffer_length);
	#endif

		// Deinterlate samples, convert them to S16, and write them to the double-buffers
		short *left_output_buffer = &stream_buffers[0][buffer_length * last_buffer];
		short *right_output_buffer = &stream_buffers[1][buffer_length * last_buffer];

		long *mixer_buffer_pointer = stream_buffer_long;
		short *left_output_buffer_pointer = left_output_buffer;
		short *right_output_buffer_pointer = right_output_buffer;

		for (unsigned int i = 0; i < buffer_length; ++i)
		{
			const long left_sample = *mixer_buffer_pointer++;
			const long right_sample = *mixer_buffer_pointer++;

			// Clamp samples to sane limits, convert to S16, and store in double-buffers
			if (left_sample > 0x7FFF)
				*left_output_buffer_pointer++ = 0x7FFF;
			else if (left_sample < -0x7FFF)
				*left_output_buffer_pointer++ = -0x7FFF;
			else
				*left_output_buffer_pointer++ = (short)left_sample;

			if (right_sample > 0x7FFF)
				*right_output_buffer_pointer++ = 0x7FFF;
			else if (right_sample < -0x7FFF)
				*right_output_buffer_pointer++ = -0x7FFF;
			else
				*right_output_buffer_pointer++ = (short)right_sample;
		}

		// Make sure the sound hardware can see our data
		DCStoreRange(left_output_buffer, buffer_length * sizeof(short));
		DCStoreRange(right_output_buffer, buffer_length * sizeof(short));

		last_buffer = current_buffer;
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

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Init(output_frequency);
#endif

	Mixer_Init(output_frequency);

	buffer_length = output_frequency / 100;	// 10ms buffer

	// Create and initialise two 'voices': each one will stream its own
	// audio - one for the left speaker, and one for the right. 

	// The software-mixer outputs interlaced samples into a buffer of `long`s,
	// so create a buffer for it here.
	stream_buffer_long = (long*)malloc(buffer_length * sizeof(long) * 2);	// `* 2` because it's an interlaced stereo buffer

	if (stream_buffer_long != NULL)
	{
		stream_buffers[0] = (short*)malloc(buffer_length * sizeof(short) * AUDIO_BUFFERS);

		if (stream_buffers[0] != NULL)
		{
			stream_buffers[1] = (short*)malloc(buffer_length * sizeof(short) * AUDIO_BUFFERS);

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
							mix_data[i].bus[0].volume = 0x8000;	// Voice 1 goes on the left speaker - voice 2 goes on the right speaker

							AXSetVoiceDeviceMix(voices[i], AX_DEVICE_TYPE_DRC, 0, mix_data);
							AXSetVoiceDeviceMix(voices[i], AX_DEVICE_TYPE_TV, 0, mix_data);

							AXSetVoiceSrcRatio(voices[i], 1.0f);	// We use the native sample rate
							AXSetVoiceSrcType(voices[i], AX_VOICE_SRC_TYPE_NONE);

							AXVoiceOffsets offs = {
								.dataType = AX_VOICE_FORMAT_LPCM16,
								.loopingEnabled = AX_VOICE_LOOP_ENABLED,
								.loopOffset = 0,
								.endOffset = (buffer_length * AUDIO_BUFFERS) - 1,	// -1 or else you'll get popping!
								.currentOffset = 0,
								.data = stream_buffers[i]
							};
							AXSetVoiceOffsets(voices[i], &offs);

							AXSetVoiceState(voices[i], AX_VOICE_STATE_PLAYING);

							AXVoiceEnd(voices[i]);
						}

						// Register the frame callback.
						// Apparently, this fires every 3ms - we will use
						// it to update the stream buffers when needed.
						AXRegisterAppFrameCallback(FrameCallback);

						return true;
					}

					AXFreeVoice(voices[0]);
				}

				free(stream_buffers[1]);
			}

			free(stream_buffers[0]);
		}

		free(stream_buffer_long);
	}

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

	AXQuit();

	return false;
}

void AudioBackend_Deinit(void)
{
	AXRegisterAppFrameCallback(NULL);

	AXFreeVoice(voices[1]);
	AXFreeVoice(voices[0]);

	free(stream_buffers[1]);
	free(stream_buffers[0]);

	free(stream_buffer_long);

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

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

void AudioBackend_Lock(void)
{
	OSLockMutex(&sound_list_mutex);
}

void AudioBackend_Unlock(void)
{
	OSUnlockMutex(&sound_list_mutex);
}
