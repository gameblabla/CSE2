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

#include "libvorbis.h"

#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "common.h"
#include "memory_stream.h"

typedef struct Decoder_libVorbis
{
	OggVorbis_File vorbis_file;
	unsigned int channel_count;
} Decoder_libVorbis;

static size_t fread_wrapper(void *output, size_t size, size_t count, void *file)
{
	return ROMemoryStream_Read((ROMemoryStream*)file, output, size, count);
}

static int fseek_wrapper(void *file, ogg_int64_t offset, int origin)
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

	return (ROMemoryStream_SetPosition((ROMemoryStream*)file, offset, memory_stream_origin) ? 0 : -1);
}

static int fclose_wrapper(void *file)
{
	ROMemoryStream_Destroy((ROMemoryStream*)file);

	return 0;
}

static long ftell_wrapper(void *file)
{
	return ROMemoryStream_GetPosition((ROMemoryStream*)file);
}

static const ov_callbacks ov_callback_memory = {
	fread_wrapper,
	fseek_wrapper,
	fclose_wrapper,
	ftell_wrapper
};

void* Decoder_libVorbis_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

	if (memory_stream != NULL)
	{
		OggVorbis_File vorbis_file;

		if (ov_open_callbacks(memory_stream, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			Decoder_libVorbis *decoder = (Decoder_libVorbis*)malloc(sizeof(Decoder_libVorbis));

			if (decoder != NULL)
			{
				vorbis_info *v_info = ov_info(&vorbis_file, -1);

				decoder->vorbis_file = vorbis_file;
				decoder->channel_count = v_info->channels;

				spec->sample_rate = v_info->rate;
				spec->channel_count = v_info->channels;
				spec->is_complex = false;

				return decoder;
			}

			ov_clear(&vorbis_file);
		}

		ROMemoryStream_Destroy(memory_stream);
	}

	return NULL;
}

void Decoder_libVorbis_Destroy(void *decoder_void)
{
	Decoder_libVorbis *decoder = (Decoder_libVorbis*)decoder_void;

	ov_clear(&decoder->vorbis_file);
	free(decoder);
}

void Decoder_libVorbis_Rewind(void *decoder_void)
{
	Decoder_libVorbis *decoder = (Decoder_libVorbis*)decoder_void;

	ov_time_seek(&decoder->vorbis_file, 0);
}

size_t Decoder_libVorbis_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_libVorbis *decoder = (Decoder_libVorbis*)decoder_void;

	const size_t size_of_frame = sizeof(ogg_int16_t) * decoder->channel_count;

	return ov_read(&decoder->vorbis_file, (char*)buffer, frames_to_do * size_of_frame, 0, 2, 1, NULL) / size_of_frame;
}
