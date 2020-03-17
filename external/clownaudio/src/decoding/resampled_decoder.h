/*
 *  (C) 2019 Clownacy
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

#pragma once

#include <stddef.h>

#include "decoders/common.h"

typedef struct DecoderSelectorData ResampledDecoderData;	// This is deliberate
typedef struct ResampledDecoder ResampledDecoder;

ResampledDecoderData* ResampledDecoder_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode);
void ResampledDecoder_UnloadData(ResampledDecoderData *data);
ResampledDecoder* ResampledDecoder_Create(ResampledDecoderData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec);
void ResampledDecoder_Destroy(ResampledDecoder *resampled_decoder);
void ResampledDecoder_Rewind(ResampledDecoder *resampled_decoder);
size_t ResampledDecoder_GetSamples(ResampledDecoder *resampled_decoder, void *buffer, size_t frames_to_do);
void ResampledDecoder_SetLoop(ResampledDecoder *resampled_decoder, bool loop);
void ResampledDecoder_SetSampleRate(ResampledDecoder *resampled_decoder, unsigned long sample_rate);
