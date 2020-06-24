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

#include "pxtone.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libs/pxtone/pxtnService.h"
#include "libs/pxtone/pxtnError.h"

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

typedef struct Decoder_PxTone
{
	pxtnService *pxtn;
	bool loop;
} Decoder_PxTone;

void* Decoder_PxTone_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)wanted_spec;

	pxtnService *pxtn = new pxtnService();

	if (pxtn->init() == pxtnOK)
	{
		unsigned long sample_rate = wanted_spec->sample_rate == 0 ? SAMPLE_RATE : wanted_spec->sample_rate;

		if (pxtn->set_destination_quality(CHANNEL_COUNT, sample_rate))
		{
			pxtnDescriptor desc;

			if (desc.set_memory_r((void*)data, data_size) && pxtn->read(&desc) == pxtnOK && pxtn->tones_ready() == pxtnOK)
			{
				pxtnVOMITPREPARATION prep = pxtnVOMITPREPARATION();
				if (loop)
					prep.flags |= pxtnVOMITPREPFLAG_loop;
				prep.start_pos_float = 0;
				prep.master_volume = 0.80f;

				if (pxtn->moo_preparation(&prep))
				{
					Decoder_PxTone *decoder = (Decoder_PxTone*)malloc(sizeof(Decoder_PxTone));

					if (decoder != NULL)
					{
						decoder->pxtn = pxtn;
						decoder->loop = loop;

						spec->sample_rate = sample_rate;
						spec->channel_count = CHANNEL_COUNT;
						spec->is_complex = true;

						return decoder;
					}
				}
			}

			pxtn->evels->Release();
		}
	}

	delete pxtn;

	return NULL;
}

void Decoder_PxTone_Destroy(void *decoder_void)
{
	Decoder_PxTone *decoder = (Decoder_PxTone*)decoder_void;

	decoder->pxtn->evels->Release();
	delete decoder->pxtn;
	free(decoder);
}

void Decoder_PxTone_Rewind(void *decoder_void)
{
	Decoder_PxTone *decoder = (Decoder_PxTone*)decoder_void;

	pxtnVOMITPREPARATION prep = pxtnVOMITPREPARATION();
	if (decoder->loop)
		prep.flags |= pxtnVOMITPREPFLAG_loop;
	prep.start_pos_float = 0;
	prep.master_volume = 0.80f;

	decoder->pxtn->moo_preparation(&prep);
}

size_t Decoder_PxTone_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	Decoder_PxTone *decoder = (Decoder_PxTone*)decoder_void;

	const size_t size_of_frame = sizeof(int16_t) * CHANNEL_COUNT;

	memset(buffer, 0, frames_to_do * size_of_frame);

	return decoder->pxtn->Moo(buffer, frames_to_do * size_of_frame);
}
