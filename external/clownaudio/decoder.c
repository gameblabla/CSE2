#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "decoders/common.h"
#include "decoders/misc_utilities.h"
#include "decoders/predecode.h"
#include "decoders/split.h"

#ifdef USE_LIBVORBIS
#include "decoders/libvorbis.h"
#endif
#ifdef USE_TREMOR
#include "decoders/tremor.h"
#endif
#ifdef USE_STB_VORBIS
#include "decoders/stb_vorbis.h"
#endif
#ifdef USE_LIBFLAC
#include "decoders/libflac.h"
#endif
#ifdef USE_DR_FLAC
#include "decoders/dr_flac.h"
#endif
#ifdef USE_DR_WAV
#include "decoders/dr_wav.h"
#endif
#ifdef USE_LIBSNDFILE
#include "decoders/libsndfile.h"
#endif
#ifdef USE_LIBXMPLITE
#include "decoders/libxmp-lite.h"
#endif
#ifdef USE_LIBOPENMPT
#include "decoders/libopenmpt.h"
#endif
#ifdef USE_SNES_SPC
#include "decoders/snes_spc.h"
#endif
#ifdef USE_PXTONE
#include "decoders/pxtone.h"
#endif

#define BACKEND_FUNCTIONS(name) \
{ \
	(void*(*)(const char*,LinkedBackend*))Decoder_##name##_LoadData, \
	(void(*)(void*))Decoder_##name##_UnloadData, \
	(void*(*)(void*,bool,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(unsigned long(*)(void*,void*,unsigned long))Decoder_##name##_GetSamples \
}

struct DecoderData
{
	LinkedBackend *linked_backend;
	void *backend_data;
};

struct Decoder
{
	void *backend_object;
	DecoderData *data;
};

static const struct
{
	const char **file_extensions;
	const DecoderBackend decoder;
	bool can_be_predecoded;
	bool can_be_split;
} backends[] = {
#ifdef USE_LIBVORBIS
	{(const char*[]){".ogg", NULL}, BACKEND_FUNCTIONS(libVorbis), true, true},
#endif
#ifdef USE_TREMOR
	{(const char*[]){".ogg", NULL}, BACKEND_FUNCTIONS(Tremor), true, true},
#endif
#ifdef USE_STB_VORBIS
	{(const char*[]){".ogg", NULL}, BACKEND_FUNCTIONS(STB_Vorbis), true, true},
#endif
#ifdef USE_LIBFLAC
	{(const char*[]){".flac", NULL}, BACKEND_FUNCTIONS(libFLAC), true, true},
#endif
#ifdef USE_DR_FLAC
	{(const char*[]){".flac", NULL}, BACKEND_FUNCTIONS(DR_FLAC), true, true},
#endif
#ifdef USE_DR_WAV
	{(const char*[]){".wav", NULL}, BACKEND_FUNCTIONS(DR_WAV), true, true},
#endif
#ifdef USE_LIBSNDFILE
	{(const char*[]){".ogg", ".flac", ".wav", ".aiff", NULL}, BACKEND_FUNCTIONS(libSndfile), true, true},
#endif
#ifdef USE_LIBOPENMPT
	{(const char*[]){".mod", ".s3m", ".xm", ".it", ".mptm", NULL}, BACKEND_FUNCTIONS(libOpenMPT), false, false},
#endif
#ifdef USE_LIBXMPLITE
	{(const char*[]){".mod", ".s3m", ".xm", ".it", NULL}, BACKEND_FUNCTIONS(libXMPLite), false, false},
#endif
#ifdef USE_SNES_SPC
	{(const char*[]){".spc", NULL}, BACKEND_FUNCTIONS(SNES_SPC), false, false},
#endif
#ifdef USE_PXTONE
	{(const char*[]){".ptcop", ".pttune", NULL}, BACKEND_FUNCTIONS(PxTone), false, false},
#endif
};

static void* TryOpen(const DecoderBackend *backend, LinkedBackend **out_linked_backend, const char *file_path, bool predecode, bool split)
{
	LinkedBackend *last_backend;
	LinkedBackend *linked_backend = malloc(sizeof(LinkedBackend));
	linked_backend->next = NULL;
	linked_backend->backend = backend;

	if (predecode)
	{
		static const DecoderBackend backend = BACKEND_FUNCTIONS(Predecode);

		last_backend = linked_backend;
		linked_backend = malloc(sizeof(LinkedBackend));
		linked_backend->next = last_backend;
		linked_backend->backend = &backend;
	}

	if (split)
	{
		static const DecoderBackend backend = BACKEND_FUNCTIONS(Split);

		last_backend = linked_backend;
		linked_backend = malloc(sizeof(LinkedBackend));
		linked_backend->next = last_backend;
		linked_backend->backend = &backend;
	}

	void *backend_object = linked_backend->backend->LoadData(file_path, linked_backend->next);

	if (backend_object == NULL)
	{
		for (LinkedBackend *current_backend = linked_backend, *next_backend; current_backend != NULL; current_backend = next_backend)
		{
			next_backend = current_backend->next;
			free(current_backend);
		}
	}
	else
	{
		*out_linked_backend = linked_backend;
	}

	return backend_object;
}

DecoderData* Decoder_LoadData(const char *file_path, bool predecode)
{
	void *backend_data = NULL;
	for (unsigned int i = 0; i < sizeof(backends) / sizeof(backends[0]); ++i)
	{
		char *extension;
		DecoderUtil_SplitFileExtension(file_path, NULL, &extension);

		for (unsigned int j = 0; backends[i].file_extensions[j] != NULL; ++j)
		{
			if (!strcmp(extension, backends[i].file_extensions[j]))
			{
				free(extension);

				LinkedBackend *linked_backend = NULL;
				backend_data = TryOpen(&backends[i].decoder, &linked_backend, file_path, predecode && backends[i].can_be_predecoded, backends[i].can_be_split);

				DecoderData *this = NULL;

				if (backend_data)
				{
					this = malloc(sizeof(DecoderData));
					this->backend_data = backend_data;
					this->linked_backend = linked_backend;
				}

				return this;
			}
		}

		free(extension);
	}

	return NULL;
}

void Decoder_UnloadData(DecoderData *data)
{
	if (data)
	{
		data->linked_backend->backend->UnloadData(data->backend_data);

		for (LinkedBackend *current_backend = data->linked_backend, *next_backend; current_backend != NULL; current_backend = next_backend)
		{
			next_backend = current_backend->next;
			free(current_backend);
		}

		free(data);
	}
}

Decoder* Decoder_Create(DecoderData *data, bool loop, DecoderInfo *info)
{
	Decoder *decoder = NULL;

	if (data && data->backend_data)
	{
		void *backend_object = data->linked_backend->backend->Create(data->backend_data, loop, info);

		if (backend_object)
		{
			decoder = malloc(sizeof(Decoder));
			decoder->backend_object = backend_object;
			decoder->data = data;
		}
	}

	return decoder;
}

void Decoder_Destroy(Decoder *decoder)
{
	if (decoder)
	{
		decoder->data->linked_backend->backend->Destroy(decoder->backend_object);
		free(decoder);
	}
}

void Decoder_Rewind(Decoder *decoder)
{
	decoder->data->linked_backend->backend->Rewind(decoder->backend_object);
}

unsigned long Decoder_GetSamples(Decoder *decoder, void *output_buffer, unsigned long frames_to_do)
{
	return decoder->data->linked_backend->backend->GetSamples(decoder->backend_object, output_buffer, frames_to_do);
}
