#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_PxTone DecoderData_PxTone;
typedef struct Decoder_PxTone Decoder_PxTone;

DecoderData_PxTone* Decoder_PxTone_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_PxTone_UnloadData(DecoderData_PxTone *data);
Decoder_PxTone* Decoder_PxTone_Create(DecoderData_PxTone *data, bool loops, DecoderInfo *info);
void Decoder_PxTone_Destroy(Decoder_PxTone *decoder);
void Decoder_PxTone_Rewind(Decoder_PxTone *decoder);
unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do);
