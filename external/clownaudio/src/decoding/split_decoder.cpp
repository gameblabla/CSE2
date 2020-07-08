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

typedef struct SplitDecoder
{
	DecoderStage next_stage[2];
	unsigned int channel_count;
	unsigned int current_decoder;
	bool last_decoder;
} SplitDecoder;

void* SplitDecoder_Create(DecoderStage *next_stage_intro, DecoderStage *next_stage_loop, unsigned int channel_count)
{
	if (next_stage_intro != NULL || next_stage_loop != NULL)
	{
		SplitDecoder *split_decoder = (SplitDecoder*)malloc(sizeof(SplitDecoder));

		if (split_decoder != NULL)
		{
			split_decoder->current_decoder = 0;
			split_decoder->last_decoder = false;

			split_decoder->next_stage[0] = *next_stage_intro;
			split_decoder->next_stage[1] = *next_stage_loop;
			split_decoder->channel_count = channel_count;

			return split_decoder;
		}
	}

	return NULL;
}

void SplitDecoder_Destroy(void *split_decoder_void)
{
	SplitDecoder *split_decoder = (SplitDecoder*)split_decoder_void;

	split_decoder->next_stage[0].Destroy(split_decoder->next_stage[0].decoder);
	split_decoder->next_stage[1].Destroy(split_decoder->next_stage[1].decoder);

	free(split_decoder);
}

void SplitDecoder_Rewind(void *split_decoder_void)
{
	SplitDecoder *split_decoder = (SplitDecoder*)split_decoder_void;

	split_decoder->current_decoder = 0;
	split_decoder->last_decoder = false;

	split_decoder->next_stage[0].Rewind(split_decoder->next_stage[0].decoder);
	split_decoder->next_stage[1].Rewind(split_decoder->next_stage[1].decoder);
}

size_t SplitDecoder_GetSamples(void *split_decoder_void, short *buffer, size_t frames_to_do)
{
	SplitDecoder *split_decoder = (SplitDecoder*)split_decoder_void;

	size_t frames_done = 0;

	for (;;)
	{
		frames_done += split_decoder->next_stage[split_decoder->current_decoder].GetSamples(split_decoder->next_stage[split_decoder->current_decoder].decoder, &buffer[frames_done * split_decoder->channel_count], frames_to_do - frames_done);

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

void SplitDecoder_SetLoop(void *split_decoder_void, bool loop)
{
	SplitDecoder *split_decoder = (SplitDecoder*)split_decoder_void;

	split_decoder->next_stage[split_decoder->last_decoder ? split_decoder->current_decoder : 1].SetLoop(split_decoder->next_stage[split_decoder->last_decoder ? split_decoder->current_decoder : 1].decoder, loop);
}
