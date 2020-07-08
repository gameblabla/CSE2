/*
 *  (C) 2019 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "resampled_decoder.h"

#include <stddef.h>
#include <stdlib.h>

#define MA_NO_DECODING
#define MA_NO_ENCODING

#ifndef MINIAUDIO_ENABLE_DEVICE_IO
 #define MA_NO_DEVICE_IO
#endif

#include "../miniaudio.h"

#include "decoders/common.h"

#define RESAMPLE_BUFFER_SIZE 0x1000

typedef struct ResampledDecoder
{
	DecoderStage next_stage;
	ma_data_converter converter;
	unsigned long sample_rate;
	size_t in_channel_count;
	size_t out_channel_count;
	short buffer[RESAMPLE_BUFFER_SIZE];
	size_t buffer_end;
	size_t buffer_done;
} ResampledDecoder;

void* ResampledDecoder_Create(DecoderStage *next_stage, bool dynamic_sample_rate, const DecoderSpec *wanted_spec, const DecoderSpec *child_spec)
{
//	DecoderSpec child_spec;
//	void *decoder = DecoderSelector_Create(data, loop, wanted_spec, &child_spec);

//	if (decoder != NULL)
	{
		ResampledDecoder *resampled_decoder = (ResampledDecoder*)malloc(sizeof(ResampledDecoder));

		if (resampled_decoder != NULL)
		{
			resampled_decoder->next_stage = *next_stage;

			ma_data_converter_config config = ma_data_converter_config_init(ma_format_s16, ma_format_s16, child_spec->channel_count, wanted_spec->channel_count, child_spec->sample_rate, wanted_spec->sample_rate == 0 ? child_spec->sample_rate : wanted_spec->sample_rate);

			if (dynamic_sample_rate)
				config.resampling.allowDynamicSampleRate = MA_TRUE;

			if (ma_data_converter_init(&config, &resampled_decoder->converter) == MA_SUCCESS)
			{
				resampled_decoder->in_channel_count = child_spec->channel_count;
				resampled_decoder->out_channel_count = wanted_spec->channel_count;
				resampled_decoder->buffer_end = 0;
				resampled_decoder->buffer_done = 0;
				resampled_decoder->sample_rate = wanted_spec->sample_rate;

				return resampled_decoder;
			}

			free(resampled_decoder);
		}

		next_stage->Destroy(next_stage->decoder);
	}

	return NULL;
}

void ResampledDecoder_Destroy(void *resampled_decoder_void)
{
	ResampledDecoder *resampled_decoder = (ResampledDecoder*)resampled_decoder_void;

	ma_data_converter_uninit(&resampled_decoder->converter);
	resampled_decoder->next_stage.Destroy(resampled_decoder->next_stage.decoder);
	free(resampled_decoder);
}

void ResampledDecoder_Rewind(void *resampled_decoder_void)
{
	ResampledDecoder *resampled_decoder = (ResampledDecoder*)resampled_decoder_void;

	resampled_decoder->next_stage.Rewind(resampled_decoder->next_stage.decoder);
}

size_t ResampledDecoder_GetSamples(void *resampled_decoder_void, short *buffer, size_t frames_to_do)
{
	ResampledDecoder *resampled_decoder = (ResampledDecoder*)resampled_decoder_void;

	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		if (resampled_decoder->buffer_done == resampled_decoder->buffer_end)
		{
			resampled_decoder->buffer_done = 0;

			resampled_decoder->buffer_end = resampled_decoder->next_stage.GetSamples(resampled_decoder->next_stage.decoder, resampled_decoder->buffer, RESAMPLE_BUFFER_SIZE / resampled_decoder->in_channel_count);

			if (resampled_decoder->buffer_end == 0)
				return frames_done;	// Sample end
		}

		ma_uint64 frames_in = resampled_decoder->buffer_end - resampled_decoder->buffer_done;
		ma_uint64 frames_out = frames_to_do - frames_done;
		ma_data_converter_process_pcm_frames(&resampled_decoder->converter, &resampled_decoder->buffer[resampled_decoder->buffer_done * resampled_decoder->in_channel_count], &frames_in, &buffer[frames_done * resampled_decoder->out_channel_count], &frames_out);

		resampled_decoder->buffer_done += frames_in;
		frames_done += frames_out;
	}

	return frames_to_do;
}

void ResampledDecoder_SetLoop(void *resampled_decoder_void, bool loop)
{
	ResampledDecoder *resampled_decoder = (ResampledDecoder*)resampled_decoder_void;

	resampled_decoder->next_stage.SetLoop(resampled_decoder->next_stage.decoder, loop);
}

void ResampledDecoder_SetSampleRate(void *resampled_decoder_void, unsigned long sample_rate)
{
	ResampledDecoder *resampled_decoder = (ResampledDecoder*)resampled_decoder_void;

	ma_data_converter_set_rate(&resampled_decoder->converter, sample_rate, resampled_decoder->sample_rate);
}
