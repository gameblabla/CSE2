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

#ifndef MINIAUDIO_ENABLE_DEVICE_IO
 #define MA_NO_DEVICE_IO
#endif
#include "../miniaudio.h"

struct BackendStream
{
	void (*user_callback)(void*, float*, size_t);
	void *user_data;

	ma_device device;
	float volume;
};

static void Callback(ma_device *device, void *output_buffer_void, const void *input_buffer, ma_uint32 frames_to_do)
{
	(void)input_buffer;

	BackendStream *stream = (BackendStream*)device->pUserData;
	float *output_buffer = (float*)output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since mini_al's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;
}

bool Backend_Init(void)
{
	return true;
}

void Backend_Deinit(void)
{
	
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, size_t), void *user_data)
{
	BackendStream *stream = (BackendStream*)malloc(sizeof(BackendStream));

	if (stream != NULL)
	{
		ma_device_config config = ma_device_config_init(ma_device_type_playback);
		config.playback.pDeviceID = NULL;
		config.playback.format = ma_format_f32;
		config.playback.channels = STREAM_CHANNEL_COUNT;
		config.sampleRate = STREAM_SAMPLE_RATE;
		config.noPreZeroedOutputBuffer = MA_TRUE;
		config.dataCallback = Callback;
		config.pUserData = stream;

		if (ma_device_init(NULL, &config, &stream->device) == MA_SUCCESS)
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
	if (stream != NULL)
	{
		ma_device_uninit(&stream->device);
		free(stream);
	}

	return true;
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

	if (stream != NULL && ma_device_is_started(&stream->device))
		success = ma_device_stop(&stream->device) == MA_SUCCESS;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL && !ma_device_is_started(&stream->device))
		success = ma_device_start(&stream->device) == MA_SUCCESS;

	return success;
}
