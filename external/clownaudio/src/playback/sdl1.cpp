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

#include "playback.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct ClownAudio_Stream
{
	void (*user_callback)(void*, float*, size_t);
	void *user_data;

	float volume;
};

static bool sdl_already_init;

static unsigned int NextPowerOfTwo(unsigned int value)
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;

	return value;
}

static void Callback(void *user_data, Uint8 *output_buffer_uint8, int bytes_to_do)
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)user_data;
	const unsigned long frames_to_do = bytes_to_do / (sizeof(short) * CLOWNAUDIO_STREAM_CHANNEL_COUNT);

	// This playback backend doesn't support float32, so we have to convert the samples to S16
	short *output_buffer_pointer = (short*)output_buffer_uint8;
	for (unsigned long frames_done = 0; frames_done < frames_to_do; frames_done += 0x1000)
	{
		float read_buffer[0x1000 * CLOWNAUDIO_STREAM_CHANNEL_COUNT];

		stream->user_callback(stream->user_data, read_buffer, MIN(0x1000, frames_to_do - frames_done));

		float *read_buffer_pointer = read_buffer;
		for (unsigned long i = 0; i < frames_to_do * CLOWNAUDIO_STREAM_CHANNEL_COUNT; ++i)
		{
			float sample = *read_buffer_pointer++;

			if (sample > 1.0f)
				sample = 1.0f;
			else if (sample < -1.0f)
				sample = -1.0f;

			*output_buffer_pointer++ = (short)(sample * stream->volume * 32767.0f);
		}
	}
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
	bool success = true;

	sdl_already_init = SDL_WasInit(SDL_INIT_AUDIO);

	if (!sdl_already_init)
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
			success = false;

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	if (!sdl_already_init)
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_CreateStream(void (*user_callback)(void*, float*, size_t), void *user_data)
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

	if (stream != NULL)
	{
		SDL_AudioSpec want;
		memset(&want, 0, sizeof(want));
		want.freq = CLOWNAUDIO_STREAM_SAMPLE_RATE;
		want.format = AUDIO_S16;
		want.channels = CLOWNAUDIO_STREAM_CHANNEL_COUNT;
		want.samples = NextPowerOfTwo(((CLOWNAUDIO_STREAM_SAMPLE_RATE * 10) / 1000) * CLOWNAUDIO_STREAM_CHANNEL_COUNT);	// A low-latency buffer of 10 milliseconds
		want.callback = Callback;
		want.userdata = stream;

		if (SDL_OpenAudio(&want, NULL) == 0)
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
	if (stream != NULL)
	{
		SDL_CloseAudio();
		free(stream);
	}

	return true;
}

CLOWNAUDIO_EXPORT bool ClownAudio_SetStreamVolume(ClownAudio_Stream *stream, float volume)
{
	if (stream != NULL)
		stream->volume = volume * volume;

	return true;
}

CLOWNAUDIO_EXPORT bool ClownAudio_PauseStream(ClownAudio_Stream *stream)
{
	if (stream != NULL)
		SDL_PauseAudio(-1);

	return true;
}

CLOWNAUDIO_EXPORT bool ClownAudio_ResumeStream(ClownAudio_Stream *stream)
{
	if (stream != NULL)
		SDL_PauseAudio(0);

	return true;
}
