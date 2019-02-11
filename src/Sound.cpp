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
	looped = false;
	
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
	//TODO: find a better and more stable(?) way to handle this function
	delete this;
}

void SOUNDBUFFER::Lock(uint8_t **outBuffer, size_t *outSize)
{
	SDL_LockAudioDevice(audioDevice);

	if (outBuffer != nullptr)
		*outBuffer = data;

	if (outSize != nullptr)
		*outSize = size;
}

void SOUNDBUFFER::Unlock()
{
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetCurrentPosition(uint32_t dwNewPosition)
{
	SDL_LockAudioDevice(audioDevice);
	samplePosition = dwNewPosition;
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetFrequency(uint32_t dwFrequency)
{
	SDL_LockAudioDevice(audioDevice);
	frequency = (double)dwFrequency;
	SDL_UnlockAudioDevice(audioDevice);
}

float MillibelToVolume(int32_t lVolume)
{
	//Volume is in hundredths of decibels, from 0 to -10000
	lVolume = clamp(lVolume, (decltype(lVolume))-10000, (decltype(lVolume))0);
	return pow(10.0, lVolume / 2000.0);
}

void SOUNDBUFFER::SetVolume(int32_t lVolume)
{
	SDL_LockAudioDevice(audioDevice);
	volume = MillibelToVolume(lVolume);
	SDL_UnlockAudioDevice(audioDevice);
}

void SOUNDBUFFER::SetPan(int32_t lPan)
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

void SOUNDBUFFER::Mix(float (*buffer)[2], size_t samples)
{
	if (!playing) //This sound buffer isn't playing
		return;

	for (size_t sample = 0; sample < samples; sample++)
	{
		const double freqPosition = frequency / FREQUENCY; //This is added to position at the end
		
		//Get the in-between sample this is (linear interpolation)
		const float sample1 = ((looped || ((size_t)samplePosition) >= 1) ? data[(size_t)samplePosition] : 0x80);
		const float sample2 = ((looping || (((size_t)samplePosition) + 1) < size) ? data[(((size_t)samplePosition) + 1) % size] : 0x80);
		
		//Interpolate sample
		const float subPos = std::fmod(samplePosition, 1.0);
		const float sampleA = sample1 + (sample2 - sample1) * subPos;
		
		//Convert sample to float32
		const float sampleConvert = (sampleA - 128.0f) / 128.0f;
		
		//Mix
		buffer[sample][0] += sampleConvert * volume * volume_l;
		buffer[sample][1] += sampleConvert * volume * volume_r;
		
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
	(void)userdata;

	float (*buffer)[2] = (float(*)[2])stream;
	const size_t samples = len / (sizeof(float) * 2);

	//Clear stream
	for (size_t sample = 0; sample < samples; ++sample)
	{
		buffer[sample][0] = 0.0f;
		buffer[sample][1] = 0.0f;
	}
	
	//Mix sounds to primary buffer
	for (SOUNDBUFFER *sound = soundBuffers; sound != nullptr; sound = sound->next)
		sound->Mix(buffer, samples);
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

size_t MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no)
{
	size_t sample_count = 0;
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
