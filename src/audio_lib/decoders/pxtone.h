// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Pxtone DecoderData_Pxtone;
typedef struct Decoder_Pxtone Decoder_Pxtone;

DecoderData_Pxtone* Decoder_Pxtone_LoadData(const char *file_path, bool loops, LinkedBackend *linked_backend);
void Decoder_Pxtone_UnloadData(DecoderData_Pxtone *data);
Decoder_Pxtone* Decoder_Pxtone_Create(DecoderData_Pxtone *data, DecoderInfo *info);
void Decoder_Pxtone_Destroy(Decoder_Pxtone *decoder);
void Decoder_Pxtone_Rewind(Decoder_Pxtone *decoder);
unsigned long Decoder_Pxtone_GetSamples(Decoder_Pxtone *decoder, void *buffer, unsigned long frames_to_do);
