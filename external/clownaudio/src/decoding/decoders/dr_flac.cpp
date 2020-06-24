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

#include "dr_flac.h"

#include <stddef.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#include "libs/dr_flac.h"

#include "common.h"

void* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	drflac *backend = drflac_open_memory(data, data_size, NULL);

	if (backend != NULL)
	{
		spec->sample_rate = backend->sampleRate;
		spec->channel_count = backend->channels;
		spec->is_complex = false;
	}

	return backend;
}

void Decoder_DR_FLAC_Destroy(void *decoder)
{
	drflac_close((drflac*)decoder);
}

void Decoder_DR_FLAC_Rewind(void *decoder)
{
	drflac_seek_to_pcm_frame((drflac*)decoder, 0);
}

size_t Decoder_DR_FLAC_GetSamples(void *decoder, short *buffer, size_t frames_to_do)
{
	return drflac_read_pcm_frames_s16((drflac*)decoder, frames_to_do, buffer);
}
