#pragma once

#include <stdbool.h>

#include "decoders/common.h"

typedef struct DecoderData DecoderData;
typedef struct Decoder Decoder;

DecoderData* Decoder_LoadData(const char *file_path, bool predecode);
void Decoder_UnloadData(DecoderData *data);
Decoder* Decoder_Create(DecoderData *data, bool loop, DecoderInfo *info);
void Decoder_Destroy(Decoder *this);
void Decoder_Rewind(Decoder *this);
unsigned long Decoder_GetSamples(Decoder *this, void *output_buffer, unsigned long frames_to_do);
