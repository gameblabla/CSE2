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

#include "pxtone_noise.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "libs/pxtone/pxtoneNoise.h"

#include "common.h"
#include "memory_stream.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct Decoder_PxToneNoise
{
	ROMemoryStream *memory_stream;
	void *buffer;
} Decoder_PxToneNoise;

void* Decoder_PxToneNoise_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	pxtoneNoise *pxtn = new pxtoneNoise();

	if (pxtn->init())
	{
		unsigned long sample_rate = wanted_spec->sample_rate == 0 ? SAMPLE_RATE : wanted_spec->sample_rate;

		if (pxtn->quality_set(CHANNEL_COUNT, sample_rate, 16))
		{
			pxtnDescriptor desc;

			if (desc.set_memory_r((void*)data, data_size))
			{
				void *buffer;
				int32_t buffer_size;

				if (pxtn->generate(&desc, &buffer, &buffer_size))
				{
					ROMemoryStream *memory_stream = ROMemoryStream_Create(buffer, buffer_size);

					if (memory_stream != NULL)
					{
						Decoder_PxToneNoise *decoder = (Decoder_PxToneNoise*)malloc(sizeof(Decoder_PxToneNoise));

						if (decoder != NULL)
						{
							decoder->memory_stream = memory_stream;
							decoder->buffer = buffer;

							spec->sample_rate = sample_rate;
							spec->channel_count = CHANNEL_COUNT;
							spec->is_complex = false;

							delete pxtn;

							return decoder;
						}

						ROMemoryStream_Destroy(memory_stream);
					}

					free(buffer);
				}
			}
		}
	}

	delete pxtn;

	return NULL;
}

void Decoder_PxToneNoise_Destroy(void *decoder_void)
{
	Decoder_PxToneNoise *decoder = (Decoder_PxToneNoise*)decoder_void;

	ROMemoryStream_Destroy(decoder->memory_stream);
	free(decoder->buffer);
	free(decoder);
}

void Decoder_PxToneNoise_Rewind(void *decoder_void)
{
	Decoder_PxToneNoise *decoder = (Decoder_PxToneNoise*)decoder_void;

	ROMemoryStream_Rewind(decoder->memory_stream);
}

size_t Decoder_PxToneNoise_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_PxToneNoise *decoder = (Decoder_PxToneNoise*)decoder_void;

	return ROMemoryStream_Read(decoder->memory_stream, buffer, sizeof(int16_t) * CHANNEL_COUNT, frames_to_do);
}
