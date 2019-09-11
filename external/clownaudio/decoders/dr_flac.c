#include "dr_flac.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#include "libs/dr_flac.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "common.h"
#include "memory_file.h"

struct DecoderData_DR_FLAC
{
	unsigned char *file_buffer;
	size_t file_size;
};

struct Decoder_DR_FLAC
{
	DecoderData_DR_FLAC *data;
	drflac *instance;
	bool loops;
};

DecoderData_DR_FLAC* Decoder_DR_FLAC_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_DR_FLAC *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_DR_FLAC));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_DR_FLAC_UnloadData(DecoderData_DR_FLAC *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(DecoderData_DR_FLAC *data, bool loops, DecoderInfo *info)
{
	Decoder_DR_FLAC *this = NULL;

	if (data && data->file_buffer)
	{
		drflac *instance = drflac_open_memory(data->file_buffer, data->file_size);

		if (instance)
		{
			this = malloc(sizeof(Decoder_DR_FLAC));

			this->instance = instance;
			this->data = data;
			this->loops = loops;

			info->sample_rate = instance->sampleRate;
			info->channel_count = instance->channels;
			info->decoded_size = (unsigned long)instance->totalSampleCount * sizeof(drflac_int32);
			info->format = DECODER_FORMAT_S32;
		}
	}

	return this;
}

void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *this)
{
	if (this)
	{
		drflac_close(this->instance);
		free(this);
	}
}

void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *this)
{
	drflac_seek_to_pcm_frame(this->instance, 0);
}

unsigned long Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *this, void *buffer_void, unsigned long frames_to_do)
{
	drflac_int32 *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = (unsigned long)drflac_read_pcm_frames_s32(this->instance, frames_to_do - frames_done_total, buffer + (frames_done_total * this->instance->channels));

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (this->loops)
				Decoder_DR_FLAC_Rewind(this);
			else
				break;
		}
	}

	return frames_done_total;
}
