#include "../Audio.h"

#include <stddef.h>
#include <stdio.h>
#include <string>

#include "SDL.h"

#include "../Misc.h"
#include "../../Organya.h"
#include "../../WindowsWrapper.h"

#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static SDL_AudioDeviceID device_id;

static unsigned long output_frequency;

static unsigned short organya_timer;

static void Callback(void *user_data, Uint8 *stream_uint8, int len)
{
	(void)user_data;

	float *stream = (float*)stream_uint8;
	unsigned int frames_total = len / sizeof(float) / 2;

	for (unsigned int i = 0; i < frames_total * 2; ++i)
		stream[i] = 0.0f;

	if (organya_timer == 0)
	{
		Mixer_MixSounds(stream, frames_total);
	}
	else
	{
		// Synchronise audio generation with Organya.
		// In the original game, Organya ran asynchronously in a separate thread,
		// firing off commands to DirectSound in realtime. To match that, we'd
		// need a very low-latency buffer, otherwise we'd get mistimed instruments.
		// Instead, we can just do this.
		unsigned int frames_done = 0;

		while (frames_done != frames_total)
		{
			static unsigned long organya_countdown;

			if (organya_countdown == 0)
			{
				organya_countdown = (organya_timer * output_frequency) / 1000;	// organya_timer is in milliseconds, so convert it to audio frames
				UpdateOrganya();
			}

			const unsigned int frames_to_do = MIN(organya_countdown, frames_total - frames_done);

			Mixer_MixSounds(stream + frames_done * 2, frames_to_do);

			frames_done += frames_to_do;
			organya_countdown -= frames_to_do;
		}
	}
}

BOOL AudioBackend_Init(void)
{
	Backend_PrintInfo("Initializing SDL2 audio backend...");
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		std::string errorMessage = std::string("'SDL_InitSubSystem(SDL_INIT_AUDIO)' failed: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDL2 audio backend)", errorMessage.c_str());
		return FALSE;
	}

	Backend_PrintInfo("Available SDL audio drivers:");

	for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i)
		Backend_PrintInfo("%s", SDL_GetAudioDriver(i));

	SDL_AudioSpec specification;
	specification.freq = 48000;
	specification.format = AUDIO_F32;
	specification.channels = 2;
	specification.samples = 0x400;	// Roughly 10 milliseconds for 48000Hz
	specification.callback = Callback;
	specification.userdata = NULL;

	SDL_AudioSpec obtained_specification;
	device_id = SDL_OpenAudioDevice(NULL, 0, &specification, &obtained_specification, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if (device_id == 0)
	{
		std::string error_message = std::string("'SDL_OpenAudioDevice' failed: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDL2 audio backend)", error_message.c_str());
		return FALSE;
	}

	output_frequency = obtained_specification.freq;
	Mixer_Init(obtained_specification.freq);

	SDL_PauseAudioDevice(device_id, 0);

	Backend_PrintInfo("Selected SDL audio driver: %s", SDL_GetCurrentAudioDriver());

	Backend_PrintInfo("Succesfully initialized SDL2 audio backend");
	return TRUE;
}

void AudioBackend_Deinit(void)
{
	Backend_PrintInfo("De-initializing SDL2 audio backend...");
	SDL_CloseAudioDevice(device_id);

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	Backend_PrintInfo("Finished de-initializing SDL2 audio backend");
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	SDL_LockAudioDevice(device_id);

	Mixer_Sound *sound = Mixer_CreateSound(frequency, samples, length);

	SDL_UnlockAudioDevice(device_id);

	return (AudioBackend_Sound*)sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_DestroySound((Mixer_Sound*)sound);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, BOOL looping)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_PlaySound((Mixer_Sound*)sound, looping);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_StopSound((Mixer_Sound*)sound);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_RewindSound((Mixer_Sound*)sound);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_SetSoundFrequency((Mixer_Sound*)sound, frequency);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_SetSoundVolume((Mixer_Sound*)sound, volume);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	SDL_LockAudioDevice(device_id);

	Mixer_SetSoundPan((Mixer_Sound*)sound, pan);

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetOrganyaTimer(unsigned short timer)
{
	SDL_LockAudioDevice(device_id);

	organya_timer = timer;

	SDL_UnlockAudioDevice(device_id);
}
