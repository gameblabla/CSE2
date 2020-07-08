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

#include "decoder_selector.h"

#include <stddef.h>
#include <stdlib.h>

#include "decoders/common.h"
#include "predecoder.h"

#ifdef USE_LIBVORBIS
#include "decoders/libvorbis.h"
#endif
#ifdef USE_STB_VORBIS
#include "decoders/stb_vorbis.h"
#endif
#ifdef USE_DR_MP3
#include "decoders/dr_mp3.h"
#endif
#ifdef USE_LIBOPUS
#include "decoders/libopus.h"
#endif
#ifdef USE_LIBFLAC
#include "decoders/libflac.h"
#endif
#ifdef USE_DR_FLAC
#include "decoders/dr_flac.h"
#endif
#ifdef USE_DR_WAV
#include "decoders/dr_wav.h"
#endif
#ifdef USE_LIBSNDFILE
#include "decoders/libsndfile.h"
#endif
#ifdef USE_LIBOPENMPT
#include "decoders/libopenmpt.h"
#endif
#ifdef USE_LIBXMPLITE
#include "decoders/libxmp-lite.h"
#endif
#ifdef USE_PXTONE
#include "decoders/pxtone.h"
#include "decoders/pxtone_noise.h"
#endif
#ifdef USE_SNES_SPC
#include "decoders/snes_spc.h"
#endif

#define DECODER_FUNCTIONS(name) \
{ \
	Decoder_##name##_Create, \
	Decoder_##name##_Destroy, \
	Decoder_##name##_Rewind, \
	Decoder_##name##_GetSamples \
}

typedef enum DecoderType
{
	DECODER_TYPE_PREDECODER,
	DECODER_TYPE_COMPLEX,
	DECODER_TYPE_SIMPLE
} DecoderType;

typedef struct DecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, short *buffer, size_t frames_to_do);
} DecoderFunctions;

typedef struct DecoderSelector
{
	void *decoder;
	DecoderSelectorData *data;
	bool loop;
} DecoderSelector;

struct DecoderSelectorData
{
	const unsigned char *file_buffer;
	size_t file_size;
	DecoderType decoder_type;
	const DecoderFunctions *decoder_functions;
	PredecoderData *predecoder_data;
	unsigned int channel_count;
};

static const DecoderFunctions decoder_function_list[] = {
#ifdef USE_LIBVORBIS
	DECODER_FUNCTIONS(libVorbis),
#endif
#ifdef USE_STB_VORBIS
	DECODER_FUNCTIONS(STB_Vorbis),
#endif
#ifdef USE_DR_MP3
	DECODER_FUNCTIONS(DR_MP3),
#endif
#ifdef USE_LIBOPUS
	DECODER_FUNCTIONS(libOpus),
#endif
#ifdef USE_LIBFLAC
	DECODER_FUNCTIONS(libFLAC),
#endif
#ifdef USE_DR_FLAC
	DECODER_FUNCTIONS(DR_FLAC),
#endif
#ifdef USE_DR_WAV
	DECODER_FUNCTIONS(DR_WAV),
#endif
#ifdef USE_LIBSNDFILE
	DECODER_FUNCTIONS(libSndfile),
#endif
#ifdef USE_LIBOPENMPT
	DECODER_FUNCTIONS(libOpenMPT),
#endif
#ifdef USE_LIBXMPLITE
	DECODER_FUNCTIONS(libXMPLite),
#endif
#ifdef USE_PXTONE
	DECODER_FUNCTIONS(PxTone),
#endif
#ifdef USE_PXTONE
	DECODER_FUNCTIONS(PxToneNoise),
#endif
#ifdef USE_SNES_SPC
	DECODER_FUNCTIONS(SNES_SPC),
#endif
};

static const DecoderFunctions predecoder_functions = {
	NULL,
	Predecoder_Destroy,
	Predecoder_Rewind,
	Predecoder_GetSamples
};

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode, bool must_predecode, const DecoderSpec *wanted_spec)
{
	DecoderType decoder_type;
	const DecoderFunctions *decoder_functions = NULL;
	PredecoderData *predecoder_data = NULL;

	DecoderSpec spec;

	// Figure out what format this sound is
	for (size_t i = 0; i < sizeof(decoder_function_list) / sizeof(decoder_function_list[0]); ++i)
	{
		void *decoder = decoder_function_list[i].Create(file_buffer, file_size, false, wanted_spec, &spec);

		if (decoder != NULL)
		{
			decoder_type = spec.is_complex ? DECODER_TYPE_COMPLEX : DECODER_TYPE_SIMPLE;
			decoder_functions = &decoder_function_list[i];

			DecoderStage stage;
			stage.decoder = decoder;
			stage.Destroy = decoder_functions->Destroy;
			stage.Rewind = decoder_functions->Rewind;
			stage.GetSamples = decoder_functions->GetSamples;
			stage.SetLoop = NULL;

			if (decoder_type == DECODER_TYPE_SIMPLE && (predecode || must_predecode))
			{
				predecoder_data = Predecoder_DecodeData(&spec, wanted_spec, &stage);

				if (predecoder_data != NULL)
				{
					decoder_type = DECODER_TYPE_PREDECODER;
					decoder_functions = &predecoder_functions;
					break;
				}
			}

			decoder_function_list[i].Destroy(decoder);

			break;
		}
	}

	if (decoder_functions != NULL && (!must_predecode || decoder_type == DECODER_TYPE_PREDECODER))
	{
		DecoderSelectorData *data = (DecoderSelectorData*)malloc(sizeof(DecoderSelectorData));

		if (data != NULL)
		{
			data->file_buffer = file_buffer;
			data->file_size = file_size;
			data->decoder_type = decoder_type;
			data->decoder_functions = decoder_functions;
			data->predecoder_data = predecoder_data;
			data->channel_count = spec.channel_count;

			return data;
		}
	}

	if (predecoder_data != NULL)
		Predecoder_UnloadData(predecoder_data);

	return NULL;
}

void DecoderSelector_UnloadData(DecoderSelectorData *data)
{
	if (data->predecoder_data != NULL)
		Predecoder_UnloadData(data->predecoder_data);

	free(data);
}

void* DecoderSelector_Create(DecoderSelectorData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	DecoderSelector *selector = (DecoderSelector*)malloc(sizeof(DecoderSelector));

	if (selector != NULL)
	{
		if (data->decoder_type == DECODER_TYPE_PREDECODER)
			selector->decoder = Predecoder_Create(data->predecoder_data, loop, wanted_spec, spec);
		else
			selector->decoder = data->decoder_functions->Create(data->file_buffer, data->file_size, loop, wanted_spec, spec);

		if (selector->decoder != NULL)
		{
			selector->data = data;
			selector->loop = loop;
			return selector;
		}

		free(selector);
	}

	return NULL;
}

void DecoderSelector_Destroy(void *selector_void)
{
	DecoderSelector *selector = (DecoderSelector*)selector_void;

	selector->data->decoder_functions->Destroy(selector->decoder);
	free(selector);
}

void DecoderSelector_Rewind(void *selector_void)
{
	DecoderSelector *selector = (DecoderSelector*)selector_void;

	selector->data->decoder_functions->Rewind(selector->decoder);
}

size_t DecoderSelector_GetSamples(void *selector_void, short *buffer, size_t frames_to_do)
{
	DecoderSelector *selector = (DecoderSelector*)selector_void;

	size_t frames_done = 0;

	switch (selector->data->decoder_type)
	{
		default:
			return 0;

		case DECODER_TYPE_PREDECODER:
			return Predecoder_GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_COMPLEX:
			return selector->data->decoder_functions->GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_SIMPLE:
			// Handle looping here, since the simple decoders don't do it by themselves
			while (frames_done != frames_to_do)
			{
				const size_t frames = selector->data->decoder_functions->GetSamples(selector->decoder, &buffer[frames_done * selector->data->channel_count], frames_to_do - frames_done);

				if (frames == 0)
				{
					if (selector->loop)
						selector->data->decoder_functions->Rewind(selector->decoder);
					else
						break;
				}

				frames_done += frames;
			}

			return frames_done;
	}
}

void DecoderSelector_SetLoop(void *selector_void, bool loop)
{
	DecoderSelector *selector = (DecoderSelector*)selector_void;

	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_SetLoop(selector->decoder, loop);
			break;

		case DECODER_TYPE_SIMPLE:
			selector->loop = loop;
			break;

		case DECODER_TYPE_COMPLEX:
			// TODO - This is impossible to implement
			break;
	}
}
