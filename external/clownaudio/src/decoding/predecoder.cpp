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

#include "decoders/common.h"
#include "decoders/memory_stream.h"

#include "resampled_decoder.h"

#define CHANNEL_COUNT 2

typedef struct Predecoder
{
	ROMemoryStream *memory_stream;
	bool loop;
} Predecoder;

struct PredecoderData
{
	void *decoded_data;
	size_t decoded_data_size;
	unsigned long sample_rate;
};

PredecoderData* Predecoder_DecodeData(const DecoderSpec *in_spec, const DecoderSpec *out_spec, DecoderStage *stage)
{
	MemoryStream *memory_stream = MemoryStream_Create(false);

	if (memory_stream != NULL)
	{
		PredecoderData *predecoder_data = (PredecoderData*)malloc(sizeof(PredecoderData));

		if (predecoder_data != NULL)
		{
			void *resampled_decoder = ResampledDecoder_Create(stage, false, out_spec, in_spec);

			if (resampled_decoder != NULL)
			{
				size_t size_of_frame = sizeof(short) * out_spec->channel_count;

				for (;;)
				{
					short buffer[0x1000];

					size_t frames_done = ResampledDecoder_GetSamples(resampled_decoder, buffer, sizeof(buffer) / size_of_frame);

					if (frames_done == 0)
						break;

					MemoryStream_Write(memory_stream, buffer, size_of_frame, frames_done);
				}

				predecoder_data->decoded_data = MemoryStream_GetBuffer(memory_stream);
				predecoder_data->decoded_data_size = MemoryStream_GetPosition(memory_stream);
				predecoder_data->sample_rate = out_spec->sample_rate == 0 ? in_spec->sample_rate : out_spec->sample_rate;

				MemoryStream_Destroy(memory_stream);
				ResampledDecoder_Destroy(resampled_decoder);

				return predecoder_data;
			}

			free(predecoder_data);
		}

		MemoryStream_Destroy(memory_stream);
	}

	return NULL;
}

void Predecoder_UnloadData(PredecoderData *data)
{
	free(data->decoded_data);
	free(data);
}

void* Predecoder_Create(PredecoderData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
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

			return predecoder;
		}

		free(predecoder);
	}

	return NULL;
}

void Predecoder_Destroy(void *predecoder_void)
{
	Predecoder *predecoder = (Predecoder*)predecoder_void;

	ROMemoryStream_Destroy(predecoder->memory_stream);

	free(predecoder);
}

void Predecoder_Rewind(void *predecoder_void)
{
	Predecoder *predecoder = (Predecoder*)predecoder_void;

	ROMemoryStream_Rewind(predecoder->memory_stream);
}

size_t Predecoder_GetSamples(void *predecoder_void, short *buffer, size_t frames_to_do)
{
	Predecoder *predecoder = (Predecoder*)predecoder_void;

	size_t frames_done = 0;

	for (;;)
	{
		frames_done += ROMemoryStream_Read(predecoder->memory_stream, &buffer[frames_done * CHANNEL_COUNT], sizeof(short) * CHANNEL_COUNT, frames_to_do - frames_done);

		if (frames_done != frames_to_do && predecoder->loop)
			Predecoder_Rewind(predecoder);
		else
			break;
	}

	return frames_done;
}

void Predecoder_SetLoop(void *predecoder_void, bool loop)
{
	Predecoder *predecoder = (Predecoder*)predecoder_void;

	predecoder->loop = loop;
}
