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

#include "libflac.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <FLAC/stream_decoder.h>

#include "common.h"
#include "memory_stream.h"

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Decoder_libFLAC
{
	ROMemoryStream *memory_stream;
	FLAC__StreamDecoder *flac_stream_decoder;
	DecoderSpec *spec;

	unsigned int channel_count;

	bool error;

	unsigned int bits_per_sample;

	unsigned char *block_buffer;
	unsigned long block_buffer_index;
	unsigned long block_buffer_size;
} Decoder_libFLAC;

static FLAC__StreamDecoderReadStatus fread_wrapper(const FLAC__StreamDecoder *flac_stream_decoder, FLAC__byte *output, size_t *count, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	FLAC__StreamDecoderReadStatus status = FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;

	*count = ROMemoryStream_Read(decoder->memory_stream, output, sizeof(FLAC__byte), *count);

	if (*count == 0)
		status = FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;

	return status;
}

static FLAC__StreamDecoderSeekStatus fseek_wrapper(const FLAC__StreamDecoder *flac_stream_decoder, FLAC__uint64 offset, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	return ROMemoryStream_SetPosition(decoder->memory_stream, offset, MEMORYSTREAM_START) ? FLAC__STREAM_DECODER_SEEK_STATUS_OK : FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
}

static FLAC__StreamDecoderTellStatus ftell_wrapper(const FLAC__StreamDecoder *flac_stream_decoder, FLAC__uint64 *offset, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	*offset = ROMemoryStream_GetPosition(decoder->memory_stream);

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

static FLAC__StreamDecoderLengthStatus GetSize(const FLAC__StreamDecoder *flac_stream_decoder, FLAC__uint64 *length, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	const size_t old_offset = ROMemoryStream_GetPosition(decoder->memory_stream);

	ROMemoryStream_SetPosition(decoder->memory_stream, 0, MEMORYSTREAM_END);
	*length = ROMemoryStream_GetPosition(decoder->memory_stream);

	ROMemoryStream_SetPosition(decoder->memory_stream, old_offset, MEMORYSTREAM_START);

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

static FLAC__bool CheckEOF(const FLAC__StreamDecoder *flac_stream_decoder, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	const size_t offset = ROMemoryStream_GetPosition(decoder->memory_stream);

	ROMemoryStream_SetPosition(decoder->memory_stream, 0, MEMORYSTREAM_END);
	const size_t size = ROMemoryStream_GetPosition(decoder->memory_stream);

	ROMemoryStream_SetPosition(decoder->memory_stream, offset, MEMORYSTREAM_START);

	return (offset == size);
}

static FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder *flac_stream_decoder, const FLAC__Frame *frame, const FLAC__int32* const buffer[], void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	FLAC__int16 *block_buffer_pointer = (FLAC__int16*)decoder->block_buffer;
	for (unsigned int i = 0; i < frame->header.blocksize; ++i)
	{
		for (unsigned int j = 0; j < frame->header.channels; ++j)
		{
			const FLAC__int32 sample = buffer[j][i];

			// Downscale/upscale to 16-bit
			if (decoder->bits_per_sample < 16)
				*block_buffer_pointer++ = (FLAC__int16)(sample << (16 - decoder->bits_per_sample));
			else if (decoder->bits_per_sample > 16)
				*block_buffer_pointer++ = (FLAC__int16)(sample >> (decoder->bits_per_sample - 16));
			else
				*block_buffer_pointer++ = (FLAC__int16)sample;
		}
	}

	decoder->block_buffer_index = 0;
	decoder->block_buffer_size = (block_buffer_pointer - (FLAC__int16*)decoder->block_buffer) / decoder->channel_count;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void MetadataCallback(const FLAC__StreamDecoder *flac_stream_decoder, const FLAC__StreamMetadata *metadata, void *user)
{
	(void)flac_stream_decoder;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	decoder->spec->sample_rate = metadata->data.stream_info.sample_rate;
	decoder->spec->channel_count = decoder->channel_count = metadata->data.stream_info.channels;
	decoder->spec->is_complex = false;

	decoder->bits_per_sample = metadata->data.stream_info.bits_per_sample;

	// Init block buffer
	decoder->block_buffer = (unsigned char*)malloc(metadata->data.stream_info.max_blocksize * sizeof(FLAC__int16) * metadata->data.stream_info.channels);
	decoder->block_buffer_index = 0;
	decoder->block_buffer_size = 0;
}

static void ErrorCallback(const FLAC__StreamDecoder *flac_stream_decoder, FLAC__StreamDecoderErrorStatus status, void *user)
{
	(void)flac_stream_decoder;
	(void)status;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)user;

	decoder->error = true;
}

void* Decoder_libFLAC_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	Decoder_libFLAC *decoder = (Decoder_libFLAC*)malloc(sizeof(Decoder_libFLAC));

	if (decoder != NULL)
	{
		decoder->flac_stream_decoder = FLAC__stream_decoder_new();

		if (decoder->flac_stream_decoder != NULL)
		{
			decoder->memory_stream = ROMemoryStream_Create(data, data_size);

			if (decoder->memory_stream != NULL)
			{
				if (FLAC__stream_decoder_init_stream(decoder->flac_stream_decoder, fread_wrapper, fseek_wrapper, ftell_wrapper, GetSize, CheckEOF, WriteCallback, MetadataCallback, ErrorCallback, decoder) == FLAC__STREAM_DECODER_INIT_STATUS_OK)
				{
					decoder->error = false;
					decoder->spec = spec;
					FLAC__stream_decoder_process_until_end_of_metadata(decoder->flac_stream_decoder);

					if (!decoder->error)
						return decoder;

					FLAC__stream_decoder_finish(decoder->flac_stream_decoder);
				}

				ROMemoryStream_Destroy(decoder->memory_stream);
			}

			FLAC__stream_decoder_delete(decoder->flac_stream_decoder);
		}

		free(decoder);
	}

	return NULL;
}

void Decoder_libFLAC_Destroy(void *decoder_void)
{
	Decoder_libFLAC *decoder = (Decoder_libFLAC*)decoder_void;

	FLAC__stream_decoder_finish(decoder->flac_stream_decoder);
	FLAC__stream_decoder_delete(decoder->flac_stream_decoder);
	ROMemoryStream_Destroy(decoder->memory_stream);
	free(decoder->block_buffer);
	free(decoder);
}

void Decoder_libFLAC_Rewind(void *decoder_void)
{
	Decoder_libFLAC *decoder = (Decoder_libFLAC*)decoder_void;

	FLAC__stream_decoder_seek_absolute(decoder->flac_stream_decoder, 0);
}

size_t Decoder_libFLAC_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_libFLAC *decoder = (Decoder_libFLAC*)decoder_void;

	if (decoder->block_buffer_index == decoder->block_buffer_size)
	{
		FLAC__stream_decoder_process_single(decoder->flac_stream_decoder);

		if (FLAC__stream_decoder_get_state(decoder->flac_stream_decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
			return 0;
	}

	const unsigned int SIZE_OF_FRAME = sizeof(FLAC__int16) * decoder->channel_count;

	const unsigned long block_frames_to_do = MIN(frames_to_do, decoder->block_buffer_size - decoder->block_buffer_index);

	memcpy(buffer, &decoder->block_buffer[decoder->block_buffer_index * SIZE_OF_FRAME], block_frames_to_do * SIZE_OF_FRAME);

	decoder->block_buffer_index += block_frames_to_do;

	return block_frames_to_do;
}
