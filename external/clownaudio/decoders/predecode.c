#include "predecode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "common.h"
#include "memory_file.h"

struct DecoderData_Predecode
{
	unsigned char *buffer;
	size_t buffer_size;
	unsigned int size_of_frame;
	DecoderInfo info;
};

struct Decoder_Predecode
{
	MemoryFile *file;
	unsigned int size_of_frame;
	bool loop;
};

DecoderData_Predecode* Decoder_Predecode_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	DecoderData_Predecode *data = NULL;

	void *decoder_data = linked_backend->backend->LoadData(file_path, linked_backend->next);

	if (decoder_data)
	{
		DecoderInfo info;
		void *backend_object = linked_backend->backend->Create(decoder_data, false, &info);

		if (backend_object)
		{
			data = malloc(sizeof(DecoderData_Predecode));

			data->size_of_frame = GetSizeOfFrame(&info);
			data->info = info;

			data->buffer = malloc(info.decoded_size);
			data->buffer_size = info.decoded_size;

			linked_backend->backend->GetSamples(backend_object, data->buffer, data->buffer_size / data->size_of_frame);
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
		free(data->buffer);
		free(data);
	}
}

Decoder_Predecode* Decoder_Predecode_Create(DecoderData_Predecode *data, bool loop, DecoderInfo *info)
{
	Decoder_Predecode *decoder = malloc(sizeof(Decoder_Predecode));
	decoder->file = MemoryFile_fopen_from(data->buffer, data->buffer_size, false);
	decoder->size_of_frame = data->size_of_frame;
	decoder->loop = loop;

	*info = data->info;

	return decoder;
}

void Decoder_Predecode_Destroy(Decoder_Predecode *decoder)
{
	if (decoder)
	{
		MemoryFile_fclose(decoder->file);
		free(decoder);
	}
}

void Decoder_Predecode_Rewind(Decoder_Predecode *decoder)
{
	MemoryFile_fseek(decoder->file, 0, SEEK_SET);
}

unsigned long Decoder_Predecode_GetSamples(Decoder_Predecode *decoder, void *output_buffer_void, unsigned long frames_to_do)
{
	unsigned char *output_buffer = output_buffer_void;

	unsigned long bytes_to_do = frames_to_do * decoder->size_of_frame;

	unsigned long bytes_done_total = 0;

	for (;;)
	{
		bytes_done_total += MemoryFile_fread(output_buffer + bytes_done_total, 1, bytes_to_do - bytes_done_total, decoder->file);

		if (bytes_done_total == bytes_to_do || !decoder->loop)
			break;

		Decoder_Predecode_Rewind(decoder);
	}

	return bytes_done_total / decoder->size_of_frame;
}
