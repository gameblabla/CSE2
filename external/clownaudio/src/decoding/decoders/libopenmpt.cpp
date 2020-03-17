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

#include <libopenmpt/libopenmpt.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

Decoder_libOpenMPT* Decoder_libOpenMPT_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	openmpt_module *module = openmpt_module_create_from_memory2(data, data_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

	if (module != NULL)
	{
		spec->sample_rate = SAMPLE_RATE;
		spec->channel_count = CHANNEL_COUNT;
		spec->format = DECODER_FORMAT_F32;
		spec->is_complex = true;

		if (loop)
			openmpt_module_set_repeat_count(module, -1);
	}

	return (Decoder_libOpenMPT*)module;
}

void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *decoder)
{
	openmpt_module_destroy((openmpt_module*)decoder);
}

void Decoder_libOpenMPT_Rewind(Decoder_libOpenMPT *decoder)
{
	openmpt_module_set_position_seconds((openmpt_module*)decoder, 0);
}

size_t Decoder_libOpenMPT_GetSamples(Decoder_libOpenMPT *decoder, void *buffer_void, size_t frames_to_do)
{
	float *buffer = (float*)buffer_void;

	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		size_t frames = openmpt_module_read_interleaved_float_stereo((openmpt_module*)decoder, SAMPLE_RATE, frames_to_do - frames_done, &buffer[frames_done * CHANNEL_COUNT]);

		if (frames == 0)
			break;

		frames_done += frames;
	}

	return frames_done;
}
