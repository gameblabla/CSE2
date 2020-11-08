// Released under the MIT licence.
// See LICENCE.txt for details.

#include "Backend.h"

#include <stddef.h>
#include <string.h>
#include <string>

#include "SDL.h"

#include "../../Misc.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void (*parent_callback)(long *stream, size_t frames_total);

static void Callback(void *user_data, Uint8 *stream_uint8, int len)
{
	(void)user_data;

	short *stream = (short*)stream_uint8;
	const size_t frames_total = len / sizeof(short) / 2;

	size_t frames_done = 0;

	while (frames_done != frames_total)
	{
		long mix_buffer[0x800 * 2];	// 2 because stereo

		size_t subframes = MIN(0x800, frames_total - frames_done);

		memset(mix_buffer, 0, subframes * sizeof(long) * 2);

		parent_callback(mix_buffer, subframes);

		for (size_t i = 0; i < subframes * 2; ++i)
		{
			if (mix_buffer[i] > 0x7FFF)
				*stream++ = 0x7FFF;
			else if (mix_buffer[i] < -0x7FFF)
				*stream++ = -0x7FFF;
			else
				*stream++ = mix_buffer[i];
		}

		frames_done += subframes;
	}
}

unsigned long SoftwareMixerBackend_Init(void (*callback)(long *stream, size_t frames_total))
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		std::string errorMessage = std::string("'SDL_InitSubSystem(SDL_INIT_AUDIO)' failed: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDL1 audio backend)", errorMessage.c_str());
		return false;
	}

	SDL_AudioSpec specification;
	specification.freq = 48000;
	specification.format = AUDIO_S16;
	specification.channels = 2;
	specification.samples = 0x400;	// Roughly 10 milliseconds for 48000Hz
	specification.callback = Callback;
	specification.userdata = NULL;

	SDL_AudioSpec obtained_specification;
	if (SDL_OpenAudio(&specification, &obtained_specification) != 0)
	{
		std::string error_message = std::string("'SDL_OpenAudio' failed: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDL1 audio backend)", error_message.c_str());
		return false;
	}

	char driver[20];
	Backend_PrintInfo("Selected SDL audio driver: %s", SDL_AudioDriverName(driver, 20));

	parent_callback = callback;

	return obtained_specification.freq;
}

void SoftwareMixerBackend_Deinit(void)
{
	SDL_CloseAudio();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool SoftwareMixerBackend_Start(void)
{
	SDL_PauseAudio(0);

	return true;
}

void SoftwareMixerBackend_LockMixerMutex(void)
{
	SDL_LockAudio();
}

void SoftwareMixerBackend_UnlockMixerMutex(void)
{
	SDL_UnlockAudio();
}

void SoftwareMixerBackend_LockOrganyaMutex(void)
{
	SDL_LockAudio();
}

void SoftwareMixerBackend_UnlockOrganyaMutex(void)
{
	SDL_UnlockAudio();
}
