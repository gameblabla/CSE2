#include "stb_vorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_STATIC
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-value"
#endif
#include "libs/stb_vorbis.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "common.h"
#include "memory_file.h"

struct DecoderData_STB_Vorbis
{
	unsigned char *file_buffer;
	size_t file_size;
};

struct Decoder_STB_Vorbis
{
	DecoderData_STB_Vorbis *data;
	stb_vorbis *instance;
	bool loops;
};

DecoderData_STB_Vorbis* Decoder_STB_Vorbis_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_STB_Vorbis *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_STB_Vorbis));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_STB_Vorbis_UnloadData(DecoderData_STB_Vorbis *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(DecoderData_STB_Vorbis *data, bool loops, DecoderInfo *info)
{
	Decoder_STB_Vorbis *this = NULL;

	if (data && data->file_buffer)
	{
		stb_vorbis *instance = stb_vorbis_open_memory(data->file_buffer, data->file_size, NULL, NULL);

		if (instance)
		{
			this = malloc(sizeof(Decoder_STB_Vorbis));

			const stb_vorbis_info vorbis_info = stb_vorbis_get_info(instance);

			this->instance = instance;
			this->data = data;
			this->loops = loops;

			info->sample_rate = vorbis_info.sample_rate;
			info->channel_count = vorbis_info.channels;
			info->decoded_size = stb_vorbis_stream_length_in_samples(instance) * vorbis_info.channels * sizeof(float);
			info->format = DECODER_FORMAT_F32;
		}
	}

	return this;
}

void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *this)
{
	if (this)
	{
		stb_vorbis_close(this->instance);
		free(this);
	}
}

void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *this)
{
	stb_vorbis_seek_start(this->instance);
}

unsigned long Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *this, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = stb_vorbis_get_samples_float_interleaved(this->instance, this->instance->channels, buffer + (frames_done_total * this->instance->channels), (frames_to_do - frames_done_total) * this->instance->channels);

		if (frames_done == 0)
		{
			if (this->loops)
				Decoder_STB_Vorbis_Rewind(this);
			else
				break;
		}
	}

	return frames_done_total;
}
