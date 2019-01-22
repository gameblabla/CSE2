#include <string>
#include <math.h>
#include <algorithm>
#include <stdint.h>

#include <SDL_audio.h>

#include "Sound.h"
#include "Organya.h"
#include "PixTone.h"

#define FREQUENCY 44100
#define STREAM_SIZE (FREQUENCY / 200)

#define clamp(x, y, z) ((x > z) ? z : (x < y) ? y : x)

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
	
	frequency = 0.0;
	volume = 1.0;
	volume_l = 1.0;
	volume_r = 1.0;
	samplePosition = 0.0;
	
	//Create waveform buffer
	data = new uint8_t[bufSize];
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
	for (SOUNDBUFFER **soundBuffer = &soundBuffers; *soundBuffer != nullptr; soundBuffer = &(*soundBuffer)->next)
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
	if (this == NULL)
		return;
	
	//TODO: find a better and more stable(?) way to handle this function
	delete this;
}

void SOUNDBUFFER::Lock(uint8_t **outBuffer, size_t *outSize)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);

	if (outBuffer != nullptr)
		*outBuffer = data;

	if (outSize != nullptr)
		*outSize = size;
}

void SOUNDBUFFER::Unlock()
{
	if (this == NULL)
		return;
	
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetCurrentPosition(uint32_t dwNewPosition)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	samplePosition = dwNewPosition;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetFrequency(uint32_t dwFrequency)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	frequency = (double)dwFrequency;
	SDL_UnlockAudioDevice(audioDevice);
}

float MillibelToVolume(int32_t lVolume)
{
	//Volume is in hundredths of decibels, from 0 to -10000
	lVolume = clamp(lVolume, (decltype(lVolume))-10000, (decltype(lVolume))0);
	return pow(10.0f, lVolume / 2000.0f);
}

void SOUNDBUFFER::SetVolume(int32_t lVolume)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	volume = MillibelToVolume(lVolume);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetPan(int32_t lPan)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	volume_l = MillibelToVolume(-lPan);
	volume_r = MillibelToVolume(lPan);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Play(bool bLooping)
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	playing = true;
	looping = bLooping;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Stop()
{
	if (this == NULL)
		return;
	
	SDL_LockAudioDevice(audioDevice);
	playing = false;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::Mix(float *buffer, int len)
{
	if (this == NULL)
		return;
	
	if (!playing) //This sound buffer isn't playing
		return;
	
	size_t samples = len / (sizeof(float) * 2);
	
	for (size_t sample = 0; sample < samples; sample++)
	{
		double freqPosition = (frequency / (double)FREQUENCY); //This is added to position at the end
		
		//Get the in-between sample this is (linear interpolation)
		uint8_t sample1 = ((looped || ((size_t)samplePosition) >= 1) ? data[(size_t)samplePosition] : 0x80);
		uint8_t sample2 = 0x80;
		if (looping || (((size_t)samplePosition) + 1) < size)
			sample2 = data[(((size_t)samplePosition) + 1) % size];
		
		//Interpolate sample
		float subPos = std::fmod(samplePosition, 1.0);
		float sampleA = (float)sample1 + ((float)sample2 - (float)sample1) * subPos;
		
		//Convert sample to float32
		float sampleConvert = (sampleA - 128.0) / 256.0;
		
		//Mix
		buffer[sample * 2] += sampleConvert * volume * volume_l;
		buffer[sample * 2 + 1] += sampleConvert * volume * volume_r;
		
		//Increment position
		samplePosition += freqPosition;
		
		if (samplePosition >= size)
		{
			if (looping)
			{
				samplePosition = std::fmod(samplePosition, size);
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
void AudioCallback(void *userdata, uint8_t *stream, int len)
{
	//Clear stream
	memset(stream, 0, len);
	
	//Mix sounds to primary buffer
	for (SOUNDBUFFER *sound = soundBuffers; sound != nullptr; sound = sound->next)
	{
		sound->Mix((float*)stream, len);
	}
}

//Sound things
SOUNDBUFFER* lpSECONDARYBUFFER[SOUND_NO];

bool InitDirectSound()
{
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
		return false;
	}
	
	//Unpause audio device
	SDL_PauseAudioDevice(audioDevice, 0);
	
	//Start organya
	StartOrganya();
	
	//Load sound effects
	MakeWaveTables();
	
	char path[0x100];
	uint8_t *buf = nullptr;
	size_t len;
		
	for (size_t n = 0; n < SOUND_NO; n++)
	{
		sprintf(path, "%2.2X.pxt", n);
		
		if (LoadPxt(path, &buf, &len))
		{
			lpSECONDARYBUFFER[n] = new SOUNDBUFFER(len);
			
			uint8_t *sBuf;
			size_t sLen;
			lpSECONDARYBUFFER[n]->Lock(&sBuf, &sLen);
			memcpy(sBuf, buf, sLen);
			lpSECONDARYBUFFER[n]->Unlock();
			lpSECONDARYBUFFER[n]->SetFrequency(22050);
		}
		
		//Free buffer, we're done with it
		if (buf)
		{
			free(buf);
			buf = nullptr;
		}
	}
	
	return true;
}

void EndDirectSound()
{
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
		if (mode == -1)
		{
			lpSECONDARYBUFFER[no]->Play(true);
		}
		else if ( mode )
		{
			if ( mode == 1 )
			{
				lpSECONDARYBUFFER[no]->Stop();
				lpSECONDARYBUFFER[no]->SetCurrentPosition(0);
				lpSECONDARYBUFFER[no]->Play(false);
			}
		}
		else
		{
			lpSECONDARYBUFFER[no]->Stop();
		}
	}
}

void ChangeSoundFrequency(int no, uint32_t rate)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetFrequency(10 * rate + 100);
}

void ChangeSoundVolume(int no, int32_t volume)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetVolume(8 * volume - 2400);
}

void ChangeSoundPan(int no, int32_t pan)
{
	if (lpSECONDARYBUFFER[no])
		lpSECONDARYBUFFER[no]->SetPan(10 * (pan - 256));
}
