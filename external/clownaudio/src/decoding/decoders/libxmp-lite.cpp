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
#include <string.h>

#define BUILDING_STATIC
#include <xmp.h>

#include "common.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct Decoder_libXMPLite
{
	xmp_context context;
	bool loop;
	xmp_frame_info frame_info;
	size_t buffer_done;
	size_t buffer_size;
} Decoder_libXMPLite;

void* Decoder_libXMPLite_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	xmp_context context = xmp_create_context();

	if (!xmp_load_module_from_memory(context, (void*)data, data_size))
	{
		unsigned long sample_rate = wanted_spec->sample_rate == 0 ? SAMPLE_RATE : wanted_spec->sample_rate;

		xmp_start_player(context, sample_rate, 0);

		Decoder_libXMPLite *decoder = (Decoder_libXMPLite*)malloc(sizeof(Decoder_libXMPLite));

		if (decoder != NULL)
		{
			decoder->context = context;
			decoder->loop = loop;
			decoder->buffer_done = 0;
			decoder->buffer_size = 0;

			spec->sample_rate = sample_rate;
			spec->channel_count = CHANNEL_COUNT;
			spec->is_complex = true;

			return decoder;
		}

		xmp_end_player(context);
		xmp_release_module(context);
	}

	xmp_free_context(context);

	return NULL;
}

void Decoder_libXMPLite_Destroy(void *decoder_void)
{
	Decoder_libXMPLite *decoder = (Decoder_libXMPLite*)decoder_void;

	xmp_end_player(decoder->context);
	xmp_release_module(decoder->context);
	xmp_free_context(decoder->context);
	free(decoder);
}

void Decoder_libXMPLite_Rewind(void *decoder_void)
{
	Decoder_libXMPLite *decoder = (Decoder_libXMPLite*)decoder_void;

	xmp_restart_module(decoder->context);

	decoder->buffer_done = 0;
	decoder->buffer_size = 0;
}

size_t Decoder_libXMPLite_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_libXMPLite *decoder = (Decoder_libXMPLite*)decoder_void;

	size_t frames_done = 0;
	while (frames_done != frames_to_do)
	{
		if (decoder->buffer_done == decoder->buffer_size)
		{
			decoder->buffer_done = 0;

			xmp_play_frame(decoder->context);

			xmp_get_frame_info(decoder->context, &decoder->frame_info);

			decoder->buffer_size = decoder->frame_info.buffer_size / (CHANNEL_COUNT * sizeof(short));
		}

		if (!decoder->loop)
			if (decoder->frame_info.loop_count != 0)
				return frames_done;

		size_t frames = MIN(frames_to_do - frames_done, decoder->buffer_size - decoder->buffer_done);

		memcpy(&buffer[frames_done * CHANNEL_COUNT], &((short*)decoder->frame_info.buffer)[decoder->buffer_done * CHANNEL_COUNT], frames * CHANNEL_COUNT * sizeof(short));

		decoder->buffer_done += frames;
		frames_done += frames;
	}

	return frames_to_do;
}
