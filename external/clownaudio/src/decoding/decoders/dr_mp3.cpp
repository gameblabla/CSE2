/*
 *  (C) 2020 Clownacy
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

#include "dr_mp3.h"

#include <stddef.h>
#include <stdlib.h>

#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_STDIO
#include "libs/dr_mp3.h"

#include "common.h"

void* Decoder_DR_MP3_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	(void)loop;	// This is ignored in simple decoders
	(void)wanted_spec;

	if ((data[0] == 0xFF && data[1] == 0xFB) || (data[0] == 0x49 && data[1] == 0x44 && data[2] == 0x33))
	{
		drmp3 *instance = (drmp3*)malloc(sizeof(drmp3));

		if (instance != NULL)
		{
			if (drmp3_init_memory(instance, data, data_size, NULL))
			{
				spec->sample_rate = instance->sampleRate;
				spec->channel_count = instance->channels;
				spec->is_complex = false;

				return instance;
			}

			free(instance);
		}
	}

	return NULL;
}

void Decoder_DR_MP3_Destroy(void *decoder)
{
	drmp3 *instance = (drmp3*)decoder;

	drmp3_uninit(instance);
	free(instance);
}

void Decoder_DR_MP3_Rewind(void *decoder)
{
	drmp3_seek_to_pcm_frame((drmp3*)decoder, 0);
}

size_t Decoder_DR_MP3_GetSamples(void *decoder, short *buffer, size_t frames_to_do)
{
	return drmp3_read_pcm_frames_s16((drmp3*)decoder, frames_to_do, buffer);
}
