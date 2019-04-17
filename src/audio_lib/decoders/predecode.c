// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "predecode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "common.h"
#include "memory_file.h"

typedef struct DecoderData_Predecode
{
	MemoryFile *file;
	bool loop;
	unsigned int size_of_frame;
	DecoderInfo info;
} DecoderData_Predecode;

typedef struct Decoder_Predecode
{
	DecoderData_Predecode *data;
} Decoder_Predecode;

DecoderData_Predecode* Decoder_Predecode_LoadData(const char *file_path, bool loop, LinkedBackend *linked_backend)
{
	DecoderData_Predecode *data = NULL;

	void *decoder_data = linked_backend->backend->LoadData(file_path, loop, linked_backend->next);

	if (decoder_data)
	{
		DecoderInfo info;
		void *backend_object = linked_backend->backend->Create(decoder_data, &info);

		if (backend_object)
		{
			data = malloc(sizeof(DecoderData_Predecode));

			data->loop = loop;
			data->size_of_frame = GetSizeOfFrame(&info);
			data->info = info;

			unsigned char *buffer = malloc(info.decoded_size);
			linked_backend->backend->GetSamples(backend_object, buffer, info.decoded_size / data->size_of_frame);
			data->file = MemoryFile_fopen_from(buffer, info.decoded_size, true);
			linked_backend->backend->Destroy(backend_object);
			linked_backend->backend->UnloadData(decoder_data);
		}
	}

	return data;
}

void Decoder_Predecode_UnloadData(DecoderData_Predecode *data)
{
	if (data)
	{
		MemoryFile_fclose(data->file);
		free(data);
	}
}

Decoder_Predecode* Decoder_Predecode_Create(DecoderData_Predecode *data, DecoderInfo *info)
{
	Decoder_Predecode *decoder = malloc(sizeof(Decoder_Predecode));
	decoder->data = data;

	*info = data->info;

	return decoder;
}

void Decoder_Predecode_Destroy(Decoder_Predecode *decoder)
{
	if (decoder)
		free(decoder);
}

void Decoder_Predecode_Rewind(Decoder_Predecode *decoder)
{
	MemoryFile_fseek(decoder->data->file, 0, SEEK_SET);
}

unsigned long Decoder_Predecode_GetSamples(Decoder_Predecode *decoder, void *output_buffer_void, unsigned long frames_to_do)
{
	unsigned char *output_buffer = output_buffer_void;

	unsigned long bytes_to_do = frames_to_do * decoder->data->size_of_frame;

	unsigned long bytes_done_total = 0;

	for (;;)
	{
		bytes_done_total += MemoryFile_fread(output_buffer + bytes_done_total, 1, bytes_to_do - bytes_done_total, decoder->data->file);

		if (bytes_done_total == bytes_to_do || !decoder->data->loop)
			break;

		Decoder_Predecode_Rewind(decoder);
	}

	return bytes_done_total / decoder->data->size_of_frame;
}
