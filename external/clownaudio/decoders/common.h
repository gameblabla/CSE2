#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
	DECODER_FORMAT_S16,
	DECODER_FORMAT_S32,
	DECODER_FORMAT_F32
} DecoderFormat;

typedef struct
{
	unsigned int sample_rate;
	unsigned int channel_count;
	unsigned long decoded_size;
	DecoderFormat format;
} DecoderInfo;

struct LinkedBackend;

typedef struct DecoderBackend
{
	void* (*LoadData)(const char *file_path, struct LinkedBackend *linked_backend);
	void (*UnloadData)(void *data);
	void* (*Create)(void *data, bool loops, DecoderInfo *info);
	void (*Destroy)(void *this);
	void (*Rewind)(void *this);
	unsigned long (*GetSamples)(void *this, void *buffer_void, unsigned long frames_to_do);
} DecoderBackend;

typedef struct LinkedBackend
{
	struct LinkedBackend *next;
	const DecoderBackend *backend;
} LinkedBackend;

static inline unsigned int GetSizeOfFrame(DecoderInfo *info)
{
	const unsigned int sizes[3] = {sizeof(short), sizeof(long), sizeof(float)};

	return sizes[info->format] * info->channel_count;
}
