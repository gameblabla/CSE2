// Released under the MIT licence.
// See LICENCE.txt for details.

#include "../Audio.h"

#include <stddef.h>

bool AudioBackend_Init(void)
{
	return true;
}

void AudioBackend_Deinit(void)
{
	
}

AudioBackend_Sound* AudioBackend_CreateSound(unsigned int frequency, const unsigned char *samples, size_t length)
{
	(void)frequency;
	(void)samples;
	(void)length;

	return NULL;
}

void AudioBackend_DestroySound(AudioBackend_Sound *sound)
{
	(void)sound;
}

void AudioBackend_PlaySound(AudioBackend_Sound *sound, bool looping)
{
	(void)sound;
	(void)looping;
}

void AudioBackend_StopSound(AudioBackend_Sound *sound)
{
	(void)sound;
}

void AudioBackend_RewindSound(AudioBackend_Sound *sound)
{
	(void)sound;
}

void AudioBackend_SetSoundFrequency(AudioBackend_Sound *sound, unsigned int frequency)
{
	(void)sound;
	(void)frequency;
}

void AudioBackend_SetSoundVolume(AudioBackend_Sound *sound, long volume)
{
	(void)sound;
	(void)volume;
}

void AudioBackend_SetSoundPan(AudioBackend_Sound *sound, long pan)
{
	(void)sound;
	(void)pan;
}

void AudioBackend_SetOrganyaCallback(void (*callback)(void))
{
	(void)callback;
}

void AudioBackend_SetOrganyaTimer(unsigned int milliseconds)
{
	(void)milliseconds;
}
