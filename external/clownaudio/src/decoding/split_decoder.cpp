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

#include "split_decoder.h"

#include <stddef.h>
#include <stdlib.h>

#include "decoders/common.h"

#include "resampled_decoder.h"

#define CHANNEL_COUNT 2

struct SplitDecoderData
{
	ResampledDecoderData *resampled_decoder_data[2];
};

struct SplitDecoder
{
	ResampledDecoder *resampled_decoder[2];
	unsigned int current_decoder;
	bool last_decoder;
};

SplitDecoderData* SplitDecoder_LoadData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode)
{
	SplitDecoderData *data = (SplitDecoderData*)malloc(sizeof(SplitDecoderData));

	if (data != NULL)
	{
		data->resampled_decoder_data[0] = file_buffer1 != NULL ? ResampledDecoder_LoadData(file_buffer1, file_size1, predecode) : NULL;
		data->resampled_decoder_data[1] = file_buffer2 != NULL ? ResampledDecoder_LoadData(file_buffer2, file_size2, predecode) : NULL;

		if (data->resampled_decoder_data[0] != NULL || data->resampled_decoder_data[1] != NULL)
			return data;

		free(data);
	}

	return NULL;
}

void SplitDecoder_UnloadData(SplitDecoderData *data)
{
	if (data->resampled_decoder_data[0] != NULL)
		ResampledDecoder_UnloadData(data->resampled_decoder_data[0]);

	if (data->resampled_decoder_data[1] != NULL)
		ResampledDecoder_UnloadData(data->resampled_decoder_data[1]);

	free(data);
}

SplitDecoder* SplitDecoder_Create(SplitDecoderData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	SplitDecoder *split_decoder = (SplitDecoder*)malloc(sizeof(SplitDecoder));

	if (split_decoder != NULL)
	{
		if (data->resampled_decoder_data[0] != NULL && data->resampled_decoder_data[1] != NULL)
		{
			split_decoder->resampled_decoder[0] = ResampledDecoder_Create(data->resampled_decoder_data[0], false, wanted_spec, spec);
			split_decoder->resampled_decoder[1] = ResampledDecoder_Create(data->resampled_decoder_data[1], loop, wanted_spec, spec);

			if (split_decoder->resampled_decoder[0] != NULL && split_decoder->resampled_decoder[1] != NULL)
			{
				split_decoder->current_decoder = 0;
				split_decoder->last_decoder = false;
				return split_decoder;
			}
		}
		else if (data->resampled_decoder_data[0] != NULL)
		{
			split_decoder->resampled_decoder[0] = ResampledDecoder_Create(data->resampled_decoder_data[0], loop, wanted_spec, spec);
			split_decoder->resampled_decoder[1] = NULL;

			if (split_decoder->resampled_decoder[0] != NULL)
			{
				split_decoder->current_decoder = 0;
				split_decoder->last_decoder = true;
				return split_decoder;
			}
		}
		else if (data->resampled_decoder_data[1] != NULL)
		{
			split_decoder->resampled_decoder[0] = NULL;
			split_decoder->resampled_decoder[1] = ResampledDecoder_Create(data->resampled_decoder_data[1], loop, wanted_spec, spec);

			if (split_decoder->resampled_decoder[1] != NULL)
			{
				split_decoder->current_decoder = 1;
				split_decoder->last_decoder = true;
				return split_decoder;
			}
		}

		free(split_decoder);
	}

	return NULL;
}

void SplitDecoder_Destroy(SplitDecoder *split_decoder)
{
	if (split_decoder->resampled_decoder[0] != NULL)
		ResampledDecoder_Destroy(split_decoder->resampled_decoder[0]);

	if (split_decoder->resampled_decoder[1] != NULL)
		ResampledDecoder_Destroy(split_decoder->resampled_decoder[1]);

	free(split_decoder);
}

void SplitDecoder_Rewind(SplitDecoder *split_decoder)
{
	if (split_decoder->resampled_decoder[0] != NULL)
		ResampledDecoder_Rewind(split_decoder->resampled_decoder[0]);

	if (split_decoder->resampled_decoder[1] != NULL)
		ResampledDecoder_Rewind(split_decoder->resampled_decoder[1]);
}

size_t SplitDecoder_GetSamples(SplitDecoder *split_decoder, void *buffer_void, size_t frames_to_do)
{
	float *buffer = (float*)buffer_void;

	size_t frames_done = 0;

	for (;;)
	{
		frames_done += ResampledDecoder_GetSamples(split_decoder->resampled_decoder[split_decoder->current_decoder], &buffer[frames_done * CHANNEL_COUNT], frames_to_do - frames_done);

		if (frames_done != frames_to_do && !split_decoder->last_decoder)
		{
			split_decoder->current_decoder = 1;
			split_decoder->last_decoder = true;
		}
		else
		{
			break;
		}
	}

	return frames_done;
}

void SplitDecoder_SetLoop(SplitDecoder *split_decoder, bool loop)
{
	ResampledDecoder_SetLoop(split_decoder->resampled_decoder[split_decoder->last_decoder ? split_decoder->current_decoder : 1], loop);
}

void SplitDecoder_SetSampleRate(SplitDecoder *split_decoder, unsigned long sample_rate1, unsigned long sample_rate2)
{
	if (split_decoder->resampled_decoder[0] != NULL)
		ResampledDecoder_SetSampleRate(split_decoder->resampled_decoder[0], sample_rate1);

	if (split_decoder->resampled_decoder[1] != NULL)
		ResampledDecoder_SetSampleRate(split_decoder->resampled_decoder[1], sample_rate2);
}
