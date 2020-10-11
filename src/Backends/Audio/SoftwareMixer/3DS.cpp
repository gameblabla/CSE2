#include "Backend.h"

#include <stddef.h>
#include <string.h>

#include <3ds.h>

#include "../../Misc.h"

#define SAMPLE_RATE 32000       // The native sample rate is 32728.4980469
#define FRAMES_PER_BUFFER (SAMPLE_RATE / 30) // 33.333 milliseconds

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void (*parent_callback)(long *stream, size_t frames_total);

static short *stream_buffer;

static ndspWaveBuf dsp_buffers[2];
static bool current_dsp_buffer;

static LightLock mixer_mutex;
static LightLock organya_mutex;

static void FullBuffer(short *stream, size_t frames_total)
{
	size_t frames_done = 0;

	while (frames_done != frames_total)
	{
		long mix_buffer[FRAMES_PER_BUFFER * 2];	// 2 because stereo

		size_t subframes = MIN(FRAMES_PER_BUFFER, frames_total - frames_done);

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

	DSP_FlushDataCache(stream, frames_total * sizeof(short) * 2);
}

static void Callback(void *user_data)
{
	(void)user_data;

	if (dsp_buffers[current_dsp_buffer].status == NDSP_WBUF_DONE)
	{
		FullBuffer(dsp_buffers[current_dsp_buffer].data_pcm16, dsp_buffers[current_dsp_buffer].nsamples);

		ndspChnWaveBufAdd(0, &dsp_buffers[current_dsp_buffer]);

		current_dsp_buffer = !current_dsp_buffer;
	}
}

unsigned long SoftwareMixerBackend_Init(void (*callback)(long *stream, size_t frames_total))
{
	parent_callback = callback;

	current_dsp_buffer = false;

	stream_buffer = (short*)linearAlloc(FRAMES_PER_BUFFER * sizeof(short) * 2 * 2);

	if (stream_buffer != NULL)
	{
		if (R_SUCCEEDED(ndspInit()))
		{
			ndspSetCallback(Callback, NULL);

			ndspSetOutputMode(NDSP_OUTPUT_STEREO);

			ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
			ndspChnSetRate(0, SAMPLE_RATE);
			ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

			float mix[12];
			mix[0] = 1.0f;
			mix[1] = 1.0f;
			mix[2] = 0.0f;
			mix[3] = 0.0f;
			mix[4] = 0.0f;
			mix[5] = 0.0f;
			mix[6] = 0.0f;
			mix[7] = 0.0f;
			mix[8] = 0.0f;
			mix[9] = 0.0f;
			mix[10] = 0.0f;
			mix[11] = 0.0f;
			ndspChnSetMix(0, mix);

			memset(dsp_buffers, 0, sizeof(dsp_buffers));
			dsp_buffers[0].data_vaddr = &stream_buffer[FRAMES_PER_BUFFER * 2 * 0];
			dsp_buffers[0].nsamples = FRAMES_PER_BUFFER;
			dsp_buffers[1].data_vaddr = &stream_buffer[FRAMES_PER_BUFFER * 2 * 1];
			dsp_buffers[1].nsamples = FRAMES_PER_BUFFER;

			LightLock_Init(&mixer_mutex);
			LightLock_Init(&organya_mutex);

			return SAMPLE_RATE;
		}
		else
		{
			Backend_PrintError("ndspInit failed");
		}

		linearFree(stream_buffer);
	}
	else
	{
		Backend_PrintError("linearAlloc failed");
	}

	return 0;
}

void SoftwareMixerBackend_Deinit(void)
{
	ndspExit();

	linearFree(stream_buffer);
}

bool SoftwareMixerBackend_Start(void)
{
	FullBuffer(stream_buffer, FRAMES_PER_BUFFER * 2);

	ndspChnWaveBufAdd(0, &dsp_buffers[0]);
	ndspChnWaveBufAdd(0, &dsp_buffers[1]);

	return true;
}

void SoftwareMixerBackend_LockMixerMutex(void)
{
	LightLock_Lock(&mixer_mutex);
}

void SoftwareMixerBackend_UnlockMixerMutex(void)
{
	LightLock_Unlock(&mixer_mutex);
}

void SoftwareMixerBackend_LockOrganyaMutex(void)
{
	LightLock_Lock(&organya_mutex);
}

void SoftwareMixerBackend_UnlockOrganyaMutex(void)
{
	LightLock_Unlock(&organya_mutex);
}
