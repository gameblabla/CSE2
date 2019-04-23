// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

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

typedef struct DecoderData_Pxtone
{
	const char *file_path;
} DecoderData_Pxtone;

typedef struct Decoder_Pxtone
{
	DecoderData_Pxtone *data;
	pxtnService *pxtn;
	bool loop;
} Decoder_Pxtone;

DecoderData_Pxtone* Decoder_Pxtone_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_Pxtone *data = malloc(sizeof(DecoderData_Pxtone));
	data->file_path = file_path;

	return data;
}

void Decoder_Pxtone_UnloadData(DecoderData_Pxtone *data)
{
	if (data)
		free(data);
}

Decoder_Pxtone* Decoder_Pxtone_Create(DecoderData_Pxtone *data, bool loop, DecoderInfo *info)
{
	Decoder_Pxtone *decoder = NULL;

	pxtnService *pxtn = Pxtone_Open(data->file_path, loop, SAMPLE_RATE, CHANNEL_COUNT);
	if (pxtn)
	{
		decoder = malloc(sizeof(Decoder_Pxtone));
		decoder->pxtn = pxtn;
		decoder->data = data;
		decoder->loop = loop;

		info->sample_rate = SAMPLE_RATE;
		info->channel_count = CHANNEL_COUNT;
		info->format = DECODER_FORMAT_S16;
	}

	return decoder;
}

void Decoder_Pxtone_Destroy(Decoder_Pxtone *decoder)
{
	if (decoder)
	{
		Pxtone_Close(decoder->pxtn);
		free(decoder);
	}
}

void Decoder_Pxtone_Rewind(Decoder_Pxtone *decoder)
{
	Pxtone_Rewind(decoder->pxtn, decoder->loop);
}

unsigned long Decoder_Pxtone_GetSamples(Decoder_Pxtone *decoder, void *buffer, unsigned long frames_to_do)
{
	const unsigned long bytes_to_do = frames_to_do * sizeof(short) * 2;

	memset(buffer, 0, bytes_to_do);
	Pxtone_GetSamples(decoder->pxtn, buffer, bytes_to_do);

	return frames_to_do;
}
