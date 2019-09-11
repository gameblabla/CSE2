#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_STB_Vorbis DecoderData_STB_Vorbis;
typedef struct Decoder_STB_Vorbis Decoder_STB_Vorbis;

DecoderData_STB_Vorbis* Decoder_STB_Vorbis_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_STB_Vorbis_UnloadData(DecoderData_STB_Vorbis *data);
Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(DecoderData_STB_Vorbis *data, bool loops, DecoderInfo *info);
void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *this);
void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *this);
unsigned long Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *this, void *buffer_void, unsigned long frames_to_do);
