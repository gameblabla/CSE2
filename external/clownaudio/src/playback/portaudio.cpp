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

#include "portaudio.h"

struct ClownAudio_Stream
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

	ClownAudio_Stream *stream = (ClownAudio_Stream*)user_data;
	float *output_buffer = (float*)output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since PortAudio's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * CLOWNAUDIO_STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;

	return 0;
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
	return Pa_Initialize() == paNoError;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	Pa_Terminate();
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_CreateStream(void (*user_callback)(void*, float*, size_t), void *user_data)
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

	if (stream != NULL)
	{
		if (Pa_OpenDefaultStream(&stream->pa_stream, 0, CLOWNAUDIO_STREAM_CHANNEL_COUNT, paFloat32, CLOWNAUDIO_STREAM_SAMPLE_RATE, paFramesPerBufferUnspecified, Callback, stream ) == paNoError)
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

CLOWNAUDIO_EXPORT bool ClownAudio_DestroyStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
	{
		success = Pa_CloseStream(stream->pa_stream) == paNoError;
		free(stream);
	}

	return success;
}

CLOWNAUDIO_EXPORT bool ClownAudio_SetStreamVolume(ClownAudio_Stream *stream, float volume)
{
	if (stream != NULL)
		stream->volume = volume * volume;

	return true;
}

CLOWNAUDIO_EXPORT bool ClownAudio_PauseStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StopStream(stream->pa_stream) == paNoError;

	return success;
}

CLOWNAUDIO_EXPORT bool ClownAudio_ResumeStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StartStream(stream->pa_stream) == paNoError;

	return success;
}
