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

#include "libopenmpt.h"

#include <stddef.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct Decoder_libOpenMPT
{
	openmpt_module *module;
	unsigned long sample_rate;
} Decoder_libOpenMPT;

void* Decoder_libOpenMPT_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	Decoder_libOpenMPT *decoder = (Decoder_libOpenMPT*)malloc(sizeof(Decoder_libOpenMPT));

	if (decoder != NULL)
	{
		unsigned long sample_rate = wanted_spec->sample_rate == 0 ? SAMPLE_RATE : wanted_spec->sample_rate;

		decoder->module = openmpt_module_create_from_memory2(data, data_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);
		decoder->sample_rate = sample_rate;

		if (decoder->module != NULL)
		{
			spec->sample_rate = sample_rate;
			spec->channel_count = CHANNEL_COUNT;
			spec->is_complex = true;

			if (loop)
				openmpt_module_set_repeat_count(decoder->module, -1);

			return decoder;
		}

		free(decoder);
	}

	return NULL;
}

void Decoder_libOpenMPT_Destroy(void *decoder_void)
{
	Decoder_libOpenMPT *decoder = (Decoder_libOpenMPT*)decoder_void;

	openmpt_module_destroy(decoder->module);
	free(decoder);
}

void Decoder_libOpenMPT_Rewind(void *decoder_void)
{
	Decoder_libOpenMPT *decoder = (Decoder_libOpenMPT*)decoder_void;

	openmpt_module_set_position_seconds(decoder->module, 0);
}

size_t Decoder_libOpenMPT_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_libOpenMPT *decoder = (Decoder_libOpenMPT*)decoder_void;

	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		size_t frames = openmpt_module_read_interleaved_stereo(decoder->module, decoder->sample_rate, frames_to_do - frames_done, &buffer[frames_done * CHANNEL_COUNT]);

		if (frames == 0)
			break;

		frames_done += frames;
	}

	return frames_done;
}
