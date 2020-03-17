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

#include "libxmp-lite.h"

#include <stddef.h>
#include <stdlib.h>

#define BUILDING_STATIC
#include <xmp.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct Decoder_libXMPLite
{
	xmp_context context;
	bool loop;
};

Decoder_libXMPLite* Decoder_libXMPLite_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	xmp_context context = xmp_create_context();

	if (!xmp_load_module_from_memory(context, (void*)data, data_size))
	{
		xmp_start_player(context, SAMPLE_RATE, 0);

		Decoder_libXMPLite *decoder = (Decoder_libXMPLite*)malloc(sizeof(Decoder_libXMPLite));

		if (decoder != NULL)
		{
			decoder->context = context;
			decoder->loop = loop;

			spec->sample_rate = SAMPLE_RATE;
			spec->channel_count = CHANNEL_COUNT;
			spec->format = DECODER_FORMAT_S16;
			spec->is_complex = true;

			return decoder;
		}

		xmp_end_player(context);
		xmp_release_module(context);
	}

	xmp_free_context(context);

	return NULL;
}

void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *decoder)
{
	xmp_end_player(decoder->context);
	xmp_release_module(decoder->context);
	xmp_free_context(decoder->context);
	free(decoder);
}

void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *decoder)
{
	xmp_seek_time(decoder->context, 0);
}

size_t Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *decoder, void *buffer, size_t frames_to_do)
{
	xmp_play_buffer(decoder->context, buffer, frames_to_do * CHANNEL_COUNT * sizeof(short), !decoder->loop);

	return frames_to_do;
}
