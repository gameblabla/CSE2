// Some of the original source code for this file can be found here:
// https://github.com/shbow/organya/blob/master/source/Sound.cpp

#include "Sound.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "WindowsWrapper.h"

#include "Organya.h"
#include "PixTone.h"
#include "SoftwareMixer.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

unsigned long output_frequency;

//Audio device
SDL_AudioDeviceID audioDevice;

//Keep track of all existing sound buffers
SOUNDBUFFER *soundBuffers;

//Sound buffer code
SOUNDBUFFER::SOUNDBUFFER(unsigned int frequency, const unsigned char *samples, size_t length)
{
	//Lock audio buffer
	SDL_LockAudioDevice(audioDevice);

	mixer_sound = Mixer_CreateSound(frequency, samples, length);

	//Add to buffer list
	this->next = soundBuffers;
	soundBuffers = this;

	//Unlock audio buffer
	SDL_UnlockAudioDevice(audioDevice);
}

SOUNDBUFFER::~SOUNDBUFFER()
{
	//Lock audio buffer
	SDL_LockAudioDevice(audioDevice);

	if (mixer_sound)
		Mixer_DestroySound(mixer_sound);

	//Remove from buffer list
	for (SOUNDBUFFER **soundBuffer = &soundBuffers; *soundBuffer != NULL; soundBuffer = &(*soundBuffer)->next)
	{
		if (*soundBuffer == this)
		{
			*soundBuffer = this->next;
			break;
		}
	}

	//Unlock audio buffer
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Release()
{
	//TODO: find a better and more stable(?) way to handle this function
	delete this;
}

void SOUNDBUFFER::SetCurrentPosition(unsigned long dwNewPosition)
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_RewindSound(mixer_sound);	// dwNewPosition is always 0 anyway
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetFrequency(unsigned long dwFrequency)
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_SetSoundFrequency(mixer_sound, dwFrequency);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetVolume(long lVolume)
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_SetSoundVolume(mixer_sound, lVolume);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetPan(long lPan)
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_SetSoundPan(mixer_sound, lPan);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Play(bool bLooping)
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_PlaySound(mixer_sound, bLooping);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Stop()
{
	SDL_LockAudioDevice(audioDevice);
	Mixer_StopSound(mixer_sound);
	SDL_UnlockAudioDevice(audioDevice);
}

static void MixSoundsAndUpdateOrganya(long *stream, size_t frames_total)
{
	if (gOrgWait == -1)
	{
		//Mix sounds to primary buffer
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
				organya_countdown = gOrgWait * output_frequency / 1000;
				OrganyaPlayData();
			}

			const unsigned int frames_to_do = MIN(organya_countdown, frames_total - frames_done);

			//Mix sounds to primary buffer
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

//Sound things
SOUNDBUFFER* lpSECONDARYBUFFER[SOUND_NO];

BOOL InitDirectSound()
{
	//Init sound
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	//Open audio device
	SDL_AudioSpec want, have;

	//Set specifications we want
	SDL_memset(&want, 0, sizeof(want));
	want.freq = 48000;
	want.format = AUDIO_S16;
	want.channels = 2;
	want.samples = 0x400;	// Roughly 10 milliseconds for 48000Hz
	want.callback = Callback;

	audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

	if (audioDevice == 0)
	{
		printf("Failed to open audio device\nSDL Error: %s\n", SDL_GetError());
		return FALSE;
	}

	output_frequency = have.freq;
	Mixer_Init(have.freq);

	//Unpause audio device
	SDL_PauseAudioDevice(audioDevice, 0);

	//Start organya
	StartOrganya();
	return TRUE;
}

void EndDirectSound()
{
	//Quit sub-system
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	//Close audio device
	SDL_CloseAudioDevice(audioDevice);

	//End organya
	EndOrganya();
}

//Sound effects playing
void PlaySoundObject(int no, int mode)
{
	if (lpSECONDARYBUFFER[no])
	{
		switch (mode)
		{
			case 0:
				lpSECONDARYBUFFER[no]->Stop();
				break;

			case 1:
				lpSECONDARYBUFFER[no]->Stop();
				lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
				lpSECONDARYBUFFER[no]->Play(false);
				break;

			case -1:
				lpSECONDARYBUFFER[no]->Play(true);
				break;
		}
	}
}

void ChangeSoundFrequency(int no, unsigned long rate)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetFrequency(10 * rate + 100);
}

void ChangeSoundVolume(int no, long volume)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetVolume(8 * volume - 2400);
}

void ChangeSoundPan(int no, long pan)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetPan(10 * (pan - 256));
}

int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no)
{
	int sample_count = 0;
	for (int i = 0; i < ptp_num; ++i)
	{
		if (ptp[i].size > sample_count)
			sample_count = ptp[i].size;
	}

	unsigned char *pcm_buffer = (unsigned char*)malloc(sample_count);
	unsigned char *mixed_pcm_buffer = (unsigned char*)malloc(sample_count);
	memset(pcm_buffer, 0x80, sample_count);
	memset(mixed_pcm_buffer, 0x80, sample_count);

	for (int i = 0; i < ptp_num; ++i)
	{
		if (!MakePixelWaveData(&ptp[i], pcm_buffer))
		{
			free(pcm_buffer);
			free(mixed_pcm_buffer);
			return -1;
		}

		for (int j = 0; j < ptp[i].size; ++j)
		{
			if (pcm_buffer[j] + mixed_pcm_buffer[j] - 0x100 < -0x7F)
				mixed_pcm_buffer[j] = 0;
			else if (pcm_buffer[j] + mixed_pcm_buffer[j] - 0x100 > 0x7F)
				mixed_pcm_buffer[j] = 0xFF;
			else
				mixed_pcm_buffer[j] += pcm_buffer[j] + -0x80;
		}
	}

	lpSECONDARYBUFFER[no] = new SOUNDBUFFER(22050, mixed_pcm_buffer, sample_count);

	free(pcm_buffer);
	free(mixed_pcm_buffer);

	return sample_count;
}
