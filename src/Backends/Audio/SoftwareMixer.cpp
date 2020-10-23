// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Audio.h"

#include <stddef.h>

#include "SoftwareMixer/Backend.h"
#include "SoftwareMixer/Mixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static unsigned long output_frequency;

static void (*organya_callback)(void);
static unsigned int organya_callback_timer_master;

static void MixSoundsAndUpdateOrganya(long *stream, size_t frames_total)
{
	SoftwareMixerBackend_LockOrganyaMutex();

	if (organya_callback_timer_master == 0)
	{
		SoftwareMixerBackend_LockMixerMutex();
		Mixer_MixSounds(stream, frames_total);
		SoftwareMixerBackend_UnlockMixerMutex();
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
			static unsigned long organya_callback_timer;

			if (organya_callback_timer == 0)
			{
				organya_callback_timer = organya_callback_timer_master;
				organya_callback();
			}

			const unsigned int frames_to_do = MIN(organya_callback_timer, frames_total - frames_done);

			SoftwareMixerBackend_LockMixerMutex();
			Mixer_MixSounds(stream + frames_done * 2, frames_to_do);
			SoftwareMixerBackend_UnlockMixerMutex();

			frames_done += frames_to_do;
			organya_callback_timer -= frames_to_do;
		}
	}

	SoftwareMixerBackend_UnlockOrganyaMutex();
}

bool AudioBackend_Init(void)
{
	output_frequency = SoftwareMixerBackend_Init(MixSoundsAndUpdateOrganya);

	if (output_frequency != 0)
	{
		Mixer_Init(output_frequency);

		if (SoftwareMixerBackend_Start())
			return true;

		SoftwareMixerBackend_Deinit();
	}

	return false;
}

void AudioBackend_Deinit(void)
{
	return SoftwareMixerBackend_Deinit();
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	SoftwareMixerBackend_LockMixerMutex();

	Mixer_Sound *sound = Mixer_CreateSound(frequency, samples, length);

	SoftwareMixerBackend_UnlockMixerMutex();

	return (AudioBackend_Sound*)sound;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_DestroySound((Mixer_Sound*)sound);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_PlaySound((Mixer_Sound*)sound, looping);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_StopSound((Mixer_Sound*)sound);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_RewindSound((Mixer_Sound*)sound);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_SetSoundFrequency((Mixer_Sound*)sound, frequency);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_SetSoundVolume((Mixer_Sound*)sound, volume);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	if (sound == NULL)
		return;

	SoftwareMixerBackend_LockMixerMutex();

	Mixer_SetSoundPan((Mixer_Sound*)sound, pan);

	SoftwareMixerBackend_UnlockMixerMutex();
}

void AudioBackend_SetOrganyaCallback(void (*callback)(void))
{
	SoftwareMixerBackend_LockOrganyaMutex();

	organya_callback = callback;

	SoftwareMixerBackend_UnlockOrganyaMutex();
}

void AudioBackend_SetOrganyaTimer(unsigned int milliseconds)
{
	SoftwareMixerBackend_LockOrganyaMutex();

	organya_callback_timer_master = (milliseconds * output_frequency) / 1000; // convert milliseconds to audio frames

	SoftwareMixerBackend_UnlockOrganyaMutex();
}
