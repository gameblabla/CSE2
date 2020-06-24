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

#include "libopus.h"

#include <stddef.h>

#include <opus/opusfile.h>

#include "common.h"

void* Decoder_libOpus_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	OggOpusFile *backend = op_open_memory(data, data_size, NULL);

	if (backend != NULL)
	{
		spec->sample_rate = 48000;  // libopusfile always outputs at 48kHz (https://opus-codec.org/docs/opusfile_api-0.7/structOpusHead.html#a73b80a913eca33d829f1667caee80d9e)
		spec->channel_count = 2;    // We use op_read_float_stereo, so libopusfile will handle conversion if it needs to
		spec->is_complex = false;
	}

	return backend;
}

void Decoder_libOpus_Destroy(void *decoder)
{
	op_free((OggOpusFile*)decoder);
}

void Decoder_libOpus_Rewind(void *decoder)
{
	op_pcm_seek((OggOpusFile*)decoder, 0);
}

size_t Decoder_libOpus_GetSamples(void *decoder, short *buffer, size_t frames_to_do)
{
	return op_read_stereo((OggOpusFile*)decoder, buffer, frames_to_do * 2);	// You tell *me* why that last parameter is in samples and not frames
}
