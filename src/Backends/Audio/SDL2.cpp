#include "../Audio.h"

#include <stddef.h>
#include <string.h>
#include <string>

#include "SDL.h"

#include "../Misc.h"
#ifdef EXTRA_SOUND_FORMATS
#include "../../ExtraSoundFormats.h"
#endif

#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static SDL_AudioDeviceID device_id;

static unsigned long output_frequency;

static void (*organya_callback)(void);
static unsigned int organya_callback_milliseconds;

static void MixSoundsAndUpdateOrganya(long *stream, size_t frames_total)
{
	if (organya_callback_milliseconds == 0)
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
				organya_countdown = (organya_callback_milliseconds * output_frequency) / 1000;	// organya_timer is in milliseconds, so convert it to audio frames
				organya_callback();
			}

			const unsigned int frames_to_do = MIN(organya_countdown, frames_total - frames_done);

			Mixer_MixSounds(stream + frames_done * 2, frames_to_do);

			frames_done += frames_to_do;
			organya_countdown -= frames_to_do;
		}
	}
}

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

		MixSoundsAndUpdateOrganya(mix_buffer, subframes);

	#ifdef EXTRA_SOUND_FORMATS
		ExtraSound_Mix(mix_buffer, subframes);
	#endif

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

bool AudioBackend_Init(void)
{
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		std::string errorMessage = std::string("'SDL_InitSubSystem(SDL_INIT_AUDIO)' failed: ") + SDL_GetError();
		Backend_ShowMessageBox("Fatal error (SDL2 audio backend)", errorMessage.c_str());
		return false;
	}

	Backend_PrintInfo("Available SDL audio drivers:");

	for (int i = 0; i < SDL_GetNumAudioDrivers(); ++i)
		Backend_PrintInfo("%s", SDL_GetAudioDriver(i));

	SDL_AudioSpec specification;
	specification.freq = 48000;
	specification.format = AUDIO_S16;
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
		return false;
	}

	output_frequency = obtained_specification.freq;
	Mixer_Init(obtained_specification.freq);

#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Init(output_frequency);
#endif

	SDL_PauseAudioDevice(device_id, 0);

	Backend_PrintInfo("Selected SDL audio driver: %s", SDL_GetCurrentAudioDriver());

	return true;
}

void AudioBackend_Deinit(void)
{
#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_Deinit();
#endif

	SDL_CloseAudioDevice(device_id);

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
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

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
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

void AudioBackend_SetOrganyaCallback(void (*callback)(void))
{
	SDL_LockAudioDevice(device_id);

	organya_callback = callback;

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_SetOrganyaTimer(unsigned int milliseconds)
{
	SDL_LockAudioDevice(device_id);

	organya_callback_milliseconds = milliseconds;

	SDL_UnlockAudioDevice(device_id);
}

void AudioBackend_Lock(void)
{
	SDL_LockAudioDevice(device_id);
}

void AudioBackend_Unlock(void)
{
	SDL_UnlockAudioDevice(device_id);
}
