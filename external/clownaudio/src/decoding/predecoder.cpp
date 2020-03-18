/*
 *  (C) 2019-2020 Clownacy
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

#include "predecoder.h"

#include <stddef.h>
#include <stdlib.h>

#define MA_NO_DECODING
#define MA_NO_ENCODING

#ifndef MINIAUDIO_ENABLE_DEVICE_IO
 #define MA_NO_DEVICE_IO
#endif

#include "../miniaudio.h"

#include "decoders/common.h"
#include "decoders/memory_stream.h"

#define CHANNEL_COUNT 2

struct PredecoderData
{
	void *decoded_data;
	size_t decoded_data_size;
	unsigned long sample_rate;
};

struct Predecoder
{
	ROMemoryStream *memory_stream;
	bool loop;
};

static ma_format FormatToMiniaudioFormat(DecoderFormat format)
{
	if (format == DECODER_FORMAT_S16)
		return ma_format_s16;
	else if (format == DECODER_FORMAT_S32)
		return ma_format_s32;
	else //if (format == DECODER_FORMAT_F32)
		return ma_format_f32;
}

PredecoderData* Predecoder_DecodeData(const DecoderSpec *in_spec, const DecoderSpec *out_spec, void *decoder, size_t (*decoder_get_samples_function)(void *decoder, void *buffer, size_t frames_to_do))
{
	PredecoderData *predecoder_data = NULL;

	MemoryStream *memory_stream = MemoryStream_Create(false);

	if (memory_stream != NULL)
	{
		const ma_data_converter_config config = ma_data_converter_config_init(FormatToMiniaudioFormat(in_spec->format), FormatToMiniaudioFormat(out_spec->format), in_spec->channel_count, out_spec->channel_count, in_spec->sample_rate, in_spec->sample_rate);

		ma_data_converter converter;
		if (ma_data_converter_init(&config, &converter) == MA_SUCCESS)
		{
			predecoder_data = (PredecoderData*)malloc(sizeof(PredecoderData));

			if (predecoder_data != NULL)
			{
				unsigned char in_buffer[0x1000];
				unsigned char out_buffer[0x1000];

				const size_t size_of_in_frame = ma_get_bytes_per_sample(FormatToMiniaudioFormat(in_spec->format)) * in_spec->channel_count;
				const size_t size_of_out_frame = ma_get_bytes_per_sample(FormatToMiniaudioFormat(out_spec->format)) * out_spec->channel_count;

				size_t in_buffer_end = 0;
				size_t in_buffer_done = 0;

				for (;;)
				{
					if (in_buffer_done == in_buffer_end)
					{
						in_buffer_done = 0;

						in_buffer_end = decoder_get_samples_function(decoder, in_buffer, 0x1000 / size_of_in_frame);

						if (in_buffer_end == 0)
							break;
					}

					ma_uint64 frames_in = in_buffer_end - in_buffer_done;
					ma_uint64 frames_out = 0x1000 / size_of_out_frame;
					ma_data_converter_process_pcm_frames(&converter, &in_buffer[in_buffer_done * size_of_in_frame], &frames_in, out_buffer, &frames_out);

					MemoryStream_Write(memory_stream, out_buffer, size_of_out_frame, frames_out);

					in_buffer_done += frames_in;
				}

				predecoder_data->decoded_data = MemoryStream_GetBuffer(memory_stream);
				predecoder_data->decoded_data_size = MemoryStream_GetPosition(memory_stream);
				predecoder_data->sample_rate = in_spec->sample_rate;
			}

			ma_data_converter_uninit(&converter);
		}

		MemoryStream_Destroy(memory_stream);
	}

	return predecoder_data;
}

void Predecoder_UnloadData(PredecoderData *data)
{
	free(data->decoded_data);
	free(data);
}

Predecoder* Predecoder_Create(PredecoderData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	Predecoder *predecoder = (Predecoder*)malloc(sizeof(Predecoder));

	if (predecoder != NULL)
	{
		predecoder->memory_stream = ROMemoryStream_Create(data->decoded_data, data->decoded_data_size);

		if (predecoder->memory_stream != NULL)
		{
			predecoder->loop = loop;

			spec->sample_rate = data->sample_rate;
			spec->channel_count = CHANNEL_COUNT;
			spec->format = DECODER_FORMAT_F32;

			return predecoder;
		}

		free(predecoder);
	}

	return NULL;
}

void Predecoder_Destroy(Predecoder *predecoder)
{
	ROMemoryStream_Destroy(predecoder->memory_stream);
}

void Predecoder_Rewind(Predecoder *predecoder)
{
	ROMemoryStream_Rewind(predecoder->memory_stream);
}

size_t Predecoder_GetSamples(Predecoder *predecoder, void *buffer_void, size_t frames_to_do)
{
	float *buffer = (float*)buffer_void;

	size_t frames_done = 0;

	for (;;)
	{
		frames_done += ROMemoryStream_Read(predecoder->memory_stream, &buffer[frames_done * CHANNEL_COUNT], sizeof(float) * CHANNEL_COUNT, frames_to_do - frames_done);

		if (frames_done != frames_to_do && predecoder->loop)
			Predecoder_Rewind(predecoder);
		else
			break;
	}

	return frames_done;
}

void Predecoder_SetLoop(Predecoder *predecoder, bool loop)
{
	predecoder->loop = loop;
}
