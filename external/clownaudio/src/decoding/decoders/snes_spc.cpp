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

#include "snes_spc.h"

#include <stddef.h>
#include <stdlib.h>

#include "libs/snes_spc-0.9.0/snes_spc/spc.h"

#include "common.h"

#define CHANNEL_COUNT 2

typedef struct Decoder_SNES_SPC
{
	const unsigned char *data;
	size_t data_size;
	SNES_SPC *snes_spc;
	SPC_Filter *filter;
} Decoder_SNES_SPC;

void* Decoder_SNES_SPC_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// Unusable, sadly - looping is up to the music file
	(void)wanted_spec;

	SNES_SPC *snes_spc = spc_new();

	SPC_Filter *filter = spc_filter_new();

	if (!spc_load_spc(snes_spc, data, data_size))
	{
		spc_clear_echo(snes_spc);

		spc_filter_clear(filter);

		Decoder_SNES_SPC *decoder = (Decoder_SNES_SPC*)malloc(sizeof(Decoder_SNES_SPC));

		if (decoder != NULL)
		{
			decoder->data = data;
			decoder->data_size = data_size;
			decoder->snes_spc = snes_spc;
			decoder->filter = filter;

			spec->sample_rate = spc_sample_rate;
			spec->channel_count = CHANNEL_COUNT;
			spec->is_complex = true;

			return decoder;
		}
	}

	spc_filter_delete(filter);
	spc_delete(snes_spc);

	return NULL;
}

void Decoder_SNES_SPC_Destroy(void *decoder_void)
{
	Decoder_SNES_SPC *decoder = (Decoder_SNES_SPC*)decoder_void;

	spc_filter_delete(decoder->filter);
	spc_delete(decoder->snes_spc);
	free(decoder);
}

void Decoder_SNES_SPC_Rewind(void *decoder_void)
{
	Decoder_SNES_SPC *decoder = (Decoder_SNES_SPC*)decoder_void;

	spc_delete(decoder->snes_spc);
	decoder->snes_spc = spc_new();
	spc_load_spc(decoder->snes_spc, decoder->data, decoder->data_size);
}

size_t Decoder_SNES_SPC_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_SNES_SPC *decoder = (Decoder_SNES_SPC*)decoder_void;

	spc_play(decoder->snes_spc, frames_to_do * CHANNEL_COUNT, buffer);

	spc_filter_run(decoder->filter, (spc_sample_t*)buffer, frames_to_do * CHANNEL_COUNT);

	return frames_to_do;
}
