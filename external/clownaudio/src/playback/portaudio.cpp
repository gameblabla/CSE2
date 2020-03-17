/*
 *  (C) 2018-2019 Clownacy
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

#include "playback.h"

#include <stddef.h>
#include <stdlib.h>

#include "portaudio.h"

struct BackendStream
{
	void (*user_callback)(void*, float*, size_t);
	void *user_data;

	PaStream *pa_stream;
	float volume;
};

static int Callback(const void *input_buffer, void *output_buffer_void, unsigned long frames_to_do, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
	(void)input_buffer;
	(void)time_info;
	(void)status_flags;

	BackendStream *stream = (BackendStream*)user_data;
	float *output_buffer = (float*)output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since PortAudio's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;

	return 0;
}

bool Backend_Init(void)
{
	return Pa_Initialize() == paNoError;
}

void Backend_Deinit(void)
{
	Pa_Terminate();
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, size_t), void *user_data)
{
	BackendStream *stream = (BackendStream*)malloc(sizeof(BackendStream));

	if (stream != NULL)
	{
		if (Pa_OpenDefaultStream(&stream->pa_stream, 0, STREAM_CHANNEL_COUNT, paFloat32, STREAM_SAMPLE_RATE, paFramesPerBufferUnspecified, Callback, stream ) == paNoError)
		{
			stream->user_callback = user_callback;
			stream->user_data = user_data;

			stream->volume = 1.0f;

			return stream;
		}

		free(stream);
	}

	return NULL;
}

bool Backend_DestroyStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL)
	{
		success = Pa_CloseStream(stream->pa_stream) == paNoError;
		free(stream);
	}

	return success;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream != NULL)
		stream->volume = volume * volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StopStream(stream->pa_stream) == paNoError;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StartStream(stream->pa_stream) == paNoError;

	return success;
}
