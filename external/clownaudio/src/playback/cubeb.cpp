/*
 *  (C) 2018-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "clownaudio/playback.h"

#include <stddef.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <cubeb/cubeb.h>

struct ClownAudio_Stream
{
	void (*user_callback)(void*, short*, size_t);
	void *user_data;

	cubeb_stream *cubeb_stream_pointer;


#ifdef _WIN32
	HANDLE mutex_handle;
#else
	pthread_mutex_t pthread_mutex;
#endif
};

static cubeb *cubeb_context;

static long DataCallback(cubeb_stream *c_stream, void *user_data, void const *input_buffer, void *output_buffer, long frames_to_do)
{
	(void)c_stream;
	(void)input_buffer;

	ClownAudio_Stream *stream = (ClownAudio_Stream*)user_data;

	stream->user_callback(stream->user_data, (short*)output_buffer, frames_to_do);

	return frames_to_do;
}

static void StateCallback(cubeb_stream *stream, void *user_data, cubeb_state state)
{
	(void)stream;
	(void)user_data;
	(void)state;
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
#ifdef _WIN32
	CoInitializeEx(NULL, COINIT_MULTITHREADED);	// Cubeb needs us to init COM
#endif

	return cubeb_init(&cubeb_context, NULL, NULL) == CUBEB_OK;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	cubeb_destroy(cubeb_context);

#ifdef _WIN32
	CoUninitialize();
#endif
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_CreateStream(unsigned long *sample_rate, void (*user_callback)(void*, short*, size_t))
{
	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_S16NE;
	output_params.prefs = CUBEB_STREAM_PREF_NONE;
	output_params.channels = CLOWNAUDIO_STREAM_CHANNEL_COUNT;
	output_params.layout = CLOWNAUDIO_STREAM_CHANNEL_COUNT == 2 ? CUBEB_LAYOUT_STEREO : CUBEB_LAYOUT_MONO;

	if (cubeb_get_preferred_sample_rate(cubeb_context, &output_params.rate) != CUBEB_OK)
		output_params.rate = *sample_rate;	// If the above line somehow fails, fallback on the default

	*sample_rate = output_params.rate;

	uint32_t latency_frames;

	if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) == CUBEB_OK)
	{
		ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

		if (stream != NULL)
		{
			cubeb_stream *cubeb_stream_pointer;

			if (cubeb_stream_init(cubeb_context, &cubeb_stream_pointer, "clownaudio stream", NULL, NULL, NULL, &output_params, latency_frames, DataCallback, StateCallback, stream) == CUBEB_OK)
			{
				stream->user_callback = user_callback;
				stream->user_data = NULL;

				stream->cubeb_stream_pointer = cubeb_stream_pointer;

			#ifdef _WIN32
				stream->mutex_handle = CreateEventA(NULL, FALSE, TRUE, NULL);
			#else
				pthread_mutex_init(&stream->pthread_mutex, NULL);
			#endif

				return stream;
			}

			free(stream);
		}
	}

	return NULL;
}

CLOWNAUDIO_EXPORT bool ClownAudio_DestroyStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
	{
		if (cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK)
		{
			cubeb_stream_destroy(stream->cubeb_stream_pointer);
			free(stream);
		}
		else
		{
			success = false;
		}
	}

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_SetStreamCallbackData(ClownAudio_Stream *stream, void *user_data)
{
	if (stream != NULL)
		stream->user_data = user_data;
}

CLOWNAUDIO_EXPORT bool ClownAudio_PauseStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}

CLOWNAUDIO_EXPORT bool ClownAudio_ResumeStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = cubeb_stream_start(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_LockStream(ClownAudio_Stream *stream)
{
	if (stream != NULL)
	{
	#ifdef _WIN32
		WaitForSingleObject(stream->mutex_handle, INFINITE);
	#else
		pthread_mutex_lock(&stream->pthread_mutex);
	#endif
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_UnlockStream(ClownAudio_Stream *stream)
{
	if (stream != NULL)
	{
	#ifdef _WIN32
		SetEvent(stream->mutex_handle);
	#else
		pthread_mutex_unlock(&stream->pthread_mutex);
	#endif
	}
}
