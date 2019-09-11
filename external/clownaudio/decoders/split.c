#include "split.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "misc_utilities.h"

#include "common.h"

struct DecoderData_Split
{
	bool is_split;

	void *subdecoder_data[2];

	const DecoderBackend *backend;
};

struct Decoder_Split
{
	DecoderData_Split *data;

	unsigned int current_file;

	bool playing_intro;

	void *subdecoders[2];

	unsigned int size_of_frame;
};

static int LoadFiles(const char *file_path, LinkedBackend *linked_backend, void *subdecoder_data[2])
{
	int result;

	char *file_path_no_extension, *file_extension;
	DecoderUtil_SplitFileExtension(file_path, &file_path_no_extension, &file_extension);

	// Look for split-file music (Cave Story 3D)
	char* const intro_file_path = DecoderUtil_sprintfMalloc("%s_intro%s", file_path_no_extension, file_extension);
	subdecoder_data[0] = linked_backend->backend->LoadData(intro_file_path, linked_backend->next);
	free(intro_file_path);

	char* const loop_file_path = DecoderUtil_sprintfMalloc("%s_loop%s", file_path_no_extension, file_extension);
	subdecoder_data[1] = linked_backend->backend->LoadData(loop_file_path, linked_backend->next);
	free(loop_file_path);

	if (subdecoder_data[0] == NULL || subdecoder_data[1] == NULL)
	{
		result = 0;

		if (subdecoder_data[0] == NULL && subdecoder_data[1] == NULL)
		{
			// Look for single-file music (Cave Story WiiWare)
			char* const single_file_path = DecoderUtil_sprintfMalloc("%s%s", file_path_no_extension, file_extension);
			subdecoder_data[0] = linked_backend->backend->LoadData(single_file_path, linked_backend->next);
			free(single_file_path);

			if (subdecoder_data[0] == NULL)
			{
				// Neither file could be opened
				result = -1;
			}
		}
		else
		{
			// Only one file could be opened
			if (subdecoder_data[0] == NULL)
			{
				// Swap files, since there has to be one in the first slot
				subdecoder_data[0] = subdecoder_data[1];
				subdecoder_data[1] = NULL;
			}
		}
	}
	else
	{
		// Both files opened successfully
		result = 1;
	}

	free(file_path_no_extension);
	free(file_extension);

	return result;
}

DecoderData_Split* Decoder_Split_LoadData(const char *path, LinkedBackend *linked_backend)
{
	DecoderData_Split *data = NULL;

	void *subdecoder_data[2];
	const int split_format = LoadFiles(path, linked_backend, subdecoder_data);

	if (split_format != -1)
	{
		data = malloc(sizeof(DecoderData_Split));
		data->backend = linked_backend->backend;

		data->is_split = split_format;

		data->subdecoder_data[0] = subdecoder_data[0];
		data->subdecoder_data[1] = subdecoder_data[1];
	}

	return data;
}

void Decoder_Split_UnloadData(DecoderData_Split *data)
{
	if (data)
	{
		for (int i = 0; i < (data->is_split ? 2 : 1); ++i)
			data->backend->UnloadData(data->subdecoder_data[i]);

		free(data);
	}
}

Decoder_Split* Decoder_Split_Create(DecoderData_Split *data, bool loop, DecoderInfo *info)
{
	Decoder_Split *decoder = NULL;

	if (data && data->subdecoder_data[0] && (!data->is_split || data->subdecoder_data[1]))
	{
		void *subdecoders[2];

		if (data->is_split)
		{
			subdecoders[0] = data->backend->Create(data->subdecoder_data[0], false, info);
			subdecoders[1] = data->backend->Create(data->subdecoder_data[1], loop, info);
		}
		else
		{
			subdecoders[0] = data->backend->Create(data->subdecoder_data[0], loop, info);
		}

		if (subdecoders[0] && (!data->is_split || subdecoders[1]))
		{
			decoder = malloc(sizeof(Decoder_Split));

			decoder->data = data;
			decoder->current_file = 0;
			decoder->playing_intro = data->is_split;
			decoder->subdecoders[0] = subdecoders[0];
			decoder->subdecoders[1] = subdecoders[1];
			decoder->size_of_frame = GetSizeOfFrame(info);
		}
	}

	return decoder;
}

void Decoder_Split_Destroy(Decoder_Split *decoder)
{
	if (decoder)
	{
		for (int i = 0; i < (decoder->data->is_split ? 2 : 1); ++i)
			decoder->data->backend->Destroy(decoder->subdecoders[i]);

		free(decoder);
	}
}

void Decoder_Split_Rewind(Decoder_Split *this)
{
	this->playing_intro = this->data->is_split;
	this->current_file = 0;

	for (int i = 0; i < (this->data->is_split ? 2 : 1); ++i)
		this->data->backend->Rewind(this->subdecoders[i]);
}

unsigned long Decoder_Split_GetSamples(Decoder_Split *this, void *output_buffer_void, unsigned long frames_to_do)
{
	unsigned char *output_buffer = output_buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = this->data->backend->GetSamples(this->subdecoders[this->current_file], output_buffer + (frames_done_total * this->size_of_frame), frames_to_do - frames_done_total);

		if (frames_done < frames_to_do - frames_done_total)	// EOF
		{
			if (this->playing_intro)
			{
				this->playing_intro = false;
				++this->current_file;
			}
			else
			{
				break;
			}
		}
	}

	return frames_done_total;
}
