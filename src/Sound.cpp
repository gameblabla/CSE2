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

#define FREQUENCY 44100

#ifdef RASPBERRY_PI
#define STREAM_SIZE 0x400	// Larger buffer to prevent stutter
#else
#define STREAM_SIZE 0x100	// FREQUENCY/200 rounded to the nearest power of 2 (SDL2 *needs* a power-of-2 buffer size)
#endif

#define clamp(x, y, z) (((x) > (z)) ? (z) : ((x) < (y)) ? (y) : (x))

//Audio device
SDL_AudioDeviceID audioDevice;

//Keep track of all existing sound buffers
SOUNDBUFFER *soundBuffers;

//Sound buffer code
SOUNDBUFFER::SOUNDBUFFER(size_t bufSize)
{
	//Lock audio buffer
	SDL_LockAudioDevice(audioDevice);

	//Set parameters
	size = bufSize;

	playing = false;
	looping = false;
	looped = false;

	frequency = 0.0;
	volume = 1.0;
	volume_l = 1.0;
	volume_r = 1.0;
	samplePosition = 0.0;

	//Create waveform buffer
	data = new unsigned char[bufSize];
	memset(data, 0x80, bufSize);

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

	//Free buffer
	if (data)
		delete[] data;

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

void SOUNDBUFFER::Lock(unsigned char **outBuffer, size_t *outSize)
{
	SDL_LockAudioDevice(audioDevice);

	if (outBuffer != NULL)
		*outBuffer = data;

	if (outSize != NULL)
		*outSize = size;
}

void SOUNDBUFFER::Unlock()
{
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetCurrentPosition(unsigned long dwNewPosition)
{
	SDL_LockAudioDevice(audioDevice);
	samplePosition = dwNewPosition;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetFrequency(unsigned long dwFrequency)
{
	SDL_LockAudioDevice(audioDevice);
	frequency = (double)dwFrequency;
	SDL_UnlockAudioDevice(audioDevice);
}

float MillibelToVolume(long lVolume)
{
	//Volume is in hundredths of decibels, from 0 to -10000
	lVolume = clamp(lVolume, (long)-10000, (long)0);
	return (float)pow(10.0, lVolume / 2000.0);
}

void SOUNDBUFFER::SetVolume(long lVolume)
{
	SDL_LockAudioDevice(audioDevice);
	volume = MillibelToVolume(lVolume);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetPan(long lPan)
{
	SDL_LockAudioDevice(audioDevice);
	volume_l = MillibelToVolume(-lPan);
	volume_r = MillibelToVolume(lPan);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Play(bool bLooping)
{
	SDL_LockAudioDevice(audioDevice);
	playing = true;
	looping = bLooping;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Stop()
{
	SDL_LockAudioDevice(audioDevice);
	playing = false;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Mix(float *buffer, size_t frames)
{
	if (!playing) //This sound buffer isn't playing
		return;

	for (size_t i = 0; i < frames; ++i)
	{
		const double freqPosition = frequency / FREQUENCY; //This is added to position at the end

		//Get the in-between sample this is (linear interpolation)
		const float sample1 = ((looped || ((size_t)samplePosition) >= 1) ? data[(size_t)samplePosition] : 128.0f);
		const float sample2 = ((looping || (((size_t)samplePosition) + 1) < size) ? data[(((size_t)samplePosition) + 1) % size] : 128.0f);

		//Interpolate sample
		const float subPos = (float)fmod(samplePosition, 1.0);
		const float sampleA = sample1 + (sample2 - sample1) * subPos;

		//Convert sample to float32
		const float sampleConvert = (sampleA - 128.0f) / 128.0f;

		//Mix
		*buffer++ += (float)(sampleConvert * volume * volume_l);
		*buffer++ += (float)(sampleConvert * volume * volume_r);

		//Increment position
		samplePosition += freqPosition;

		if (samplePosition >= size)
		{
			if (looping)
			{
				samplePosition = fmod(samplePosition, size);
				looped = true;
			}
			else
			{
				samplePosition = 0.0;
				playing = false;
				looped = false;
				break;
			}
		}
	}
}

//Sound mixer
void AudioCallback(void *userdata, Uint8 *stream, int len)
{
	(void)userdata;

	float *buffer = (float*)stream;
	const size_t frames = len / (sizeof(float) * 2);

	//Clear stream
	for (size_t i = 0; i < frames * 2; ++i)
		buffer[i] = 0.0f;

	//Mix sounds to primary buffer
	for (SOUNDBUFFER *sound = soundBuffers; sound != NULL; sound = sound->next)
		sound->Mix(buffer, frames);
}

//Sound things
SOUNDBUFFER* lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound()
{
	//Init sound
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	//Open audio device
	SDL_AudioSpec want, have;

	//Set specifications we want
	SDL_memset(&want, 0, sizeof(want));
	want.freq = FREQUENCY;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = STREAM_SIZE;
	want.callback = AudioCallback;

	audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

	if (audioDevice == 0)
	{
		printf("Failed to open audio device\nSDL Error: %s\n", SDL_GetError());
		return FALSE;
	}

	//Unpause audio device
	SDL_PauseAudioDevice(audioDevice, 0);

	for (unsigned int i = 0; i < SE_MAX; ++i)
		lpSECONDARYBUFFER[i] = NULL;

	//Start organya
	StartOrganya();
	return TRUE;
}

void EndDirectSound()
{
	EndOrganya();

	for (unsigned int i = 0; i < SE_MAX; ++i)
		lpSECONDARYBUFFER[i]->Release();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	SDL_CloseAudioDevice(audioDevice);

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
	lpSECONDARYBUFFER[no]->SetFrequency((rate * 10) + 100);
}

void ChangeSoundVolume(int no, long volume)
{
	lpSECONDARYBUFFER[no]->SetVolume((volume - 300) * 8);
}

void ChangeSoundPan(int no, long pan)
{
	lpSECONDARYBUFFER[no]->SetPan((pan - 256) * 10);
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

	lpSECONDARYBUFFER[no] = new SOUNDBUFFER(sample_count);

	unsigned char *buf;
	lpSECONDARYBUFFER[no]->Lock(&buf, NULL);
	memcpy(buf, mixed_pcm_buffer, sample_count);
	lpSECONDARYBUFFER[no]->Unlock();
	lpSECONDARYBUFFER[no]->SetFrequency(22050);

	free(pcm_buffer);
	free(mixed_pcm_buffer);

	return sample_count;
}
