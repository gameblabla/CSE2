#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_libXMPLite DecoderData_libXMPLite;
typedef struct Decoder_libXMPLite Decoder_libXMPLite;

DecoderData_libXMPLite* Decoder_libXMPLite_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_libXMPLite_UnloadData(DecoderData_libXMPLite *data);
Decoder_libXMPLite* Decoder_libXMPLite_Create(DecoderData_libXMPLite *data, bool loops, DecoderInfo *info);
void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *this);
void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *this);
unsigned long Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *this, void *buffer, unsigned long bytes_to_do);
