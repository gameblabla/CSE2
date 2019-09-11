#include "pxtone.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/pxtone/shim.h"

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct DecoderData_PxTone
{
	const char *file_path;
};

struct Decoder_PxTone
{
	DecoderData_PxTone *data;
	pxtnService *pxtn;
	bool loop;
};

DecoderData_PxTone* Decoder_PxTone_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_PxTone *data = malloc(sizeof(DecoderData_PxTone));
	data->file_path = file_path;

	return data;
}

void Decoder_PxTone_UnloadData(DecoderData_PxTone *data)
{
	if (data)
		free(data);
}

Decoder_PxTone* Decoder_PxTone_Create(DecoderData_PxTone *data, bool loop, DecoderInfo *info)
{
	Decoder_PxTone *decoder = NULL;

	pxtnService *pxtn = PxTone_Open(data->file_path, loop, SAMPLE_RATE, CHANNEL_COUNT);
	if (pxtn)
	{
		decoder = malloc(sizeof(Decoder_PxTone));
		decoder->pxtn = pxtn;
		decoder->data = data;
		decoder->loop = loop;

		info->sample_rate = SAMPLE_RATE;
		info->channel_count = CHANNEL_COUNT;
		info->format = DECODER_FORMAT_S16;
	}

	return decoder;
}

void Decoder_PxTone_Destroy(Decoder_PxTone *decoder)
{
	if (decoder)
	{
		PxTone_Close(decoder->pxtn);
		free(decoder);
	}
}

void Decoder_PxTone_Rewind(Decoder_PxTone *decoder)
{
	PxTone_Rewind(decoder->pxtn, decoder->loop);
}

unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do)
{
	const unsigned long bytes_to_do = frames_to_do * sizeof(short) * 2;

	memset(buffer, 0, bytes_to_do);
	PxTone_GetSamples(decoder->pxtn, buffer, bytes_to_do);

	return frames_to_do;
}
