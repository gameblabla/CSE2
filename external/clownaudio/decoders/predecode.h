#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_Predecode Decoder_Predecode;
typedef struct DecoderData_Predecode DecoderData_Predecode;

DecoderData_Predecode* Decoder_Predecode_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_Predecode_UnloadData(DecoderData_Predecode *data);
Decoder_Predecode* Decoder_Predecode_Create(DecoderData_Predecode *data, bool loops, DecoderInfo *info);
void Decoder_Predecode_Destroy(Decoder_Predecode *this);
void Decoder_Predecode_Rewind(Decoder_Predecode *this);
unsigned long Decoder_Predecode_GetSamples(Decoder_Predecode *this, void *output_buffer, unsigned long frames_to_do);
