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

#include "libsndfile.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "common.h"
#include "memory_stream.h"

typedef struct Decoder_libSndfile
{
	ROMemoryStream *memory_stream;
	SNDFILE *sndfile;
} Decoder_libSndfile;

static sf_count_t fread_wrapper(void *output, sf_count_t count, void *user)
{
	return ROMemoryStream_Read((ROMemoryStream*)user, output, 1, count);
}

static sf_count_t fseek_wrapper(sf_count_t offset, int origin, void *user)
{
	enum MemoryStream_Origin memory_stream_origin;
	switch (origin)
	{
		case SEEK_SET:
			memory_stream_origin = MEMORYSTREAM_START;
			break;

		case SEEK_CUR:
			memory_stream_origin = MEMORYSTREAM_CURRENT;
			break;

		case SEEK_END:
			memory_stream_origin = MEMORYSTREAM_END;
			break;

		default:
			return -1;
	}

	return (ROMemoryStream_SetPosition((ROMemoryStream*)user, offset, memory_stream_origin) ? 0 : -1);
}

static sf_count_t ftell_wrapper(void *user)
{
	return ROMemoryStream_GetPosition((ROMemoryStream*)user);
}

static sf_count_t GetStreamSize(void *user)
{
	const sf_count_t old_offset = ftell_wrapper(user);

	fseek_wrapper(0, SEEK_END, user);
	const sf_count_t size = ftell_wrapper(user);

	fseek_wrapper(old_offset, SEEK_SET, user);

	return size;
}

static SF_VIRTUAL_IO sfvirtual = {
	GetStreamSize,
	fseek_wrapper,
	fread_wrapper,
	NULL,
	ftell_wrapper
};

void* Decoder_libSndfile_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

	if (memory_stream != NULL)
	{
		SF_INFO sf_info;
		memset(&sf_info, 0, sizeof(SF_INFO));

		SNDFILE *sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, memory_stream);

		if (sndfile != NULL)
		{
			Decoder_libSndfile *decoder = (Decoder_libSndfile*)malloc(sizeof(Decoder_libSndfile));

			if (decoder != NULL)
			{
				decoder->sndfile = sndfile;
				decoder->memory_stream = memory_stream;

				spec->sample_rate = sf_info.samplerate;
				spec->channel_count = sf_info.channels;
				spec->is_complex = false;

				return decoder;
			}

			sf_close(sndfile);
		}

		ROMemoryStream_Destroy(memory_stream);
	}

	return NULL;
}

void Decoder_libSndfile_Destroy(void *decoder_void)
{
	Decoder_libSndfile *decoder = (Decoder_libSndfile*)decoder_void;

	sf_close(decoder->sndfile);
	ROMemoryStream_Destroy(decoder->memory_stream);
	free(decoder);
}

void Decoder_libSndfile_Rewind(void *decoder_void)
{
	Decoder_libSndfile *decoder = (Decoder_libSndfile*)decoder_void;

	sf_seek(decoder->sndfile, 0, SEEK_SET);
}

size_t Decoder_libSndfile_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_libSndfile *decoder = (Decoder_libSndfile*)decoder_void;

	return sf_readf_short(decoder->sndfile, buffer, frames_to_do);
}
