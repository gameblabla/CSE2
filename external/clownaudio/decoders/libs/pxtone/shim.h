#pragma once

#include <stdbool.h>

typedef struct pxtnService pxtnService;

pxtnService* PxTone_Open(const char *file_path, bool loop, unsigned int sample_rate, unsigned int channel_count);
void PxTone_Close(pxtnService *decoder);
void PxTone_Rewind(pxtnService *decoder, bool loop);
unsigned long PxTone_GetSamples(pxtnService *decoder, void *buffer, unsigned long bytes_to_do);
