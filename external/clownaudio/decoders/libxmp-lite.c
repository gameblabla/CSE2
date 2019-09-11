#include "libxmp-lite.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define BUILDING_STATIC
#include <xmp.h>

#include "common.h"
#include "memory_file.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct DecoderData_libXMPLite
{
	unsigned char *file_buffer;
	size_t file_size;
};

struct Decoder_libXMPLite
{
	DecoderData_libXMPLite *data;
	xmp_context context;
	bool loops;
};

DecoderData_libXMPLite* Decoder_libXMPLite_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_libXMPLite *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_libXMPLite));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_libXMPLite_UnloadData(DecoderData_libXMPLite *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_libXMPLite* Decoder_libXMPLite_Create(DecoderData_libXMPLite *data, bool loops, DecoderInfo *info)
{
	Decoder_libXMPLite *decoder = NULL;

	if (data)
	{
		xmp_context context = xmp_create_context();

		if (!xmp_load_module_from_memory(context, data->file_buffer, data->file_size))
		{
			xmp_start_player(context, SAMPLE_RATE, 0);

			decoder = malloc(sizeof(Decoder_libXMPLite));

			decoder->context = context;
			decoder->data = data;
			decoder->loops = loops;

			info->sample_rate = SAMPLE_RATE;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_S16;
		}
	}

	return decoder;
}

void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *decoder)
{
	if (decoder)
	{
		xmp_end_player(decoder->context);
		xmp_release_module(decoder->context);
		xmp_free_context(decoder->context);
		free(decoder);
	}
}

void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *decoder)
{
	xmp_seek_time(decoder->context, 0);
}

unsigned long Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *decoder, void *buffer, unsigned long frames_to_do)
{
	xmp_play_buffer(decoder->context, buffer, frames_to_do * CHANNEL_COUNT * sizeof(short), !decoder->loops);

	return frames_to_do;
}
