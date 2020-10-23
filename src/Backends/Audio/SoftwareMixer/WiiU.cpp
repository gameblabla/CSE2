// Released under the MIT licence.
// See LICENCE.txt for details.

#include "Backend.h"

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

#define AUDIO_BUFFERS 2	// Double-buffer

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

static void (*parent_callback)(long *stream, size_t frames_total);

static OSMutex sound_list_mutex;
static OSMutex organya_mutex;

static AXVoice *voices[2];

static short *stream_buffers[2];
static long *stream_buffer_long;
static size_t buffer_length;

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
		parent_callback(stream_buffer_long, buffer_length);

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

unsigned long SoftwareMixerBackend_Init(void (*callback)(long *stream, size_t frames_total))
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

	unsigned long output_frequency = AXGetInputSamplesPerSec();

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

							AXVoiceEnd(voices[i]);
						}

						parent_callback = callback;

						// Register the frame callback.
						// Apparently, this fires every 3ms - we will use
						// it to update the stream buffers when needed.
						AXRegisterAppFrameCallback(FrameCallback);

						return output_frequency;
					}

					AXFreeVoice(voices[0]);
				}

				free(stream_buffers[1]);
			}

			free(stream_buffers[0]);
		}

		free(stream_buffer_long);
	}

	AXQuit();

	return 0;
}

void SoftwareMixerBackend_Deinit(void)
{
	AXRegisterAppFrameCallback(NULL);

	AXFreeVoice(voices[1]);
	AXFreeVoice(voices[0]);

	free(stream_buffers[1]);
	free(stream_buffers[0]);

	free(stream_buffer_long);

	AXQuit();
}

bool SoftwareMixerBackend_Start(void)
{
	AXSetVoiceState(voices[0], AX_VOICE_STATE_PLAYING);
	AXSetVoiceState(voices[1], AX_VOICE_STATE_PLAYING);

	return true;
}

void SoftwareMixerBackend_LockMixerMutex(void)
{
	OSLockMutex(&sound_list_mutex);
}

void SoftwareMixerBackend_UnlockMixerMutex(void)
{
	OSUnlockMutex(&sound_list_mutex);
}

void SoftwareMixerBackend_LockOrganyaMutex(void)
{
	OSLockMutex(&organya_mutex);
}

void SoftwareMixerBackend_UnlockOrganyaMutex(void)
{
	OSUnlockMutex(&organya_mutex);
}
