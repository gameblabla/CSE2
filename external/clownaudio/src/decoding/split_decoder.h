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

void* SplitDecoder_Create(DecoderStage *next_stage_intro, DecoderStage *next_stage_loop, unsigned int channel_count);
void SplitDecoder_Destroy(void *split_decoder);
void SplitDecoder_Rewind(void *split_decoder);
size_t SplitDecoder_GetSamples(void *split_decoder, short *buffer, size_t frames_to_do);
void SplitDecoder_SetLoop(void *split_decoder, bool loop);
void SplitDecoder_SetSampleRate(void *split_decoder, unsigned long sample_rate);
