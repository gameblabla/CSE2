#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Split DecoderData_Split;
typedef struct Decoder_Split Decoder_Split;

DecoderData_Split* Decoder_Split_LoadData(const char *path, LinkedBackend *linked_backend);
void Decoder_Split_UnloadData(DecoderData_Split *data);
Decoder_Split* Decoder_Split_Create(DecoderData_Split *data, bool loop, DecoderInfo *info);
void Decoder_Split_Destroy(Decoder_Split *this);
void Decoder_Split_Rewind(Decoder_Split *this);
unsigned long Decoder_Split_GetSamples(Decoder_Split *this, void *output_buffer, unsigned long frames_to_do);
