#include "Sound.h"

#include <algorithm>
#include <cmath>
#include <stdint.h>
#include <string.h>

#include <ogc/lwp_threads.h>
#include <gccore.h>
#include <ogc/audio.h>

#include "Organya.h"
#include "PixTone.h"

#define clamp(x, y, z) ((x > z) ? z : (x < y) ? y : x)

#define SND_BUFFER_SIZE 4096
#define MIX_BUFFER_SIZE (SND_BUFFER_SIZE / 2 * sizeof(long))

uint8_t audio_frame[2][SND_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
bool audio_index; //NOTE: audio is double-buffered

extern "C"
{
void _cpu_context_save_fp(frame_context *);
void _cpu_context_restore_fp(frame_context *);
}

//Keep track of all existing sound buffers
SOUNDBUFFER *soundBuffers;

//Sound buffer code
SOUNDBUFFER::SOUNDBUFFER(size_t bufSize)
{
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
}

SOUNDBUFFER::~SOUNDBUFFER()
{
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
}

void SOUNDBUFFER::Release()
{
	//TODO: find a better and more stable(?) way to handle this function
	delete this;
}

void SOUNDBUFFER::Lock(uint8_t **outBuffer, size_t *outSize)
{
	if (outBuffer != NULL)
		*outBuffer = data;

	if (outSize != NULL)
		*outSize = size;
}

void SOUNDBUFFER::Unlock()
{
	//Nothing
}

void SOUNDBUFFER::SetCurrentPosition(uint32_t dwNewPosition)
{
	samplePosition = dwNewPosition;
}

void SOUNDBUFFER::SetFrequency(uint32_t dwFrequency)
{
	frequency = (double)dwFrequency;
}

float MillibelToVolume(int32_t lVolume)
{
	//Volume is in hundredths of decibels, from 0 to -10000
	lVolume = clamp(lVolume, (int32_t)-10000, (int32_t)0);
	return (float)pow(10.0, lVolume / 2000.0);
}

void SOUNDBUFFER::SetVolume(int32_t lVolume)
{
	volume = MillibelToVolume(lVolume);
}

void SOUNDBUFFER::SetPan(int32_t lPan)
{
	volume_l = MillibelToVolume(-lPan);
	volume_r = MillibelToVolume(lPan);
}

void SOUNDBUFFER::Play(bool bLooping)
{
	playing = true;
	looping = bLooping;
}

void SOUNDBUFFER::Stop()
{
	playing = false;
}

void SOUNDBUFFER::Mix(long *stream, uint32_t samples)
{
	if (!playing) //This sound buffer isn't playing
		return;

	for (size_t sample = 0; sample < samples; sample++)
	{
		const double freqPosition = frequency / 48000 ; //This is added to position at the end
		
		//Get the in-between sample this is (linear interpolation)
		const uint8_t sample1 = ((looped || ((size_t)samplePosition) >= 1) ? data[(size_t)samplePosition] : 0x80);
		const uint8_t sample2 = ((looping || (((size_t)samplePosition) + 1) < size) ? data[(((size_t)samplePosition) + 1) % size] : 0x80);

		//Interpolate sample
		const float subPos = (float)std::fmod(samplePosition, 1.0);
		const uint8_t sampleA = sample1 + (sample2 - sample1) * subPos;

		//Convert sample to signed
		const int8_t sampleConvert = sampleA - 0x80;

		//Mix (NOTE: Wii buffer is interlaced Right Left stereo)
		stream[sample * 2] += sampleConvert * volume * volume_r;
		stream[sample * 2 + 1] += sampleConvert * volume * volume_l;
		
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

//Sound things
SOUNDBUFFER* lpSECONDARYBUFFER[SOUND_NO];

void StreamCallback()
{
	//Play organya
	gOrgTimer += SND_BUFFER_SIZE / 4;
	
	if (gOrgTimer > gOrgSamplePerStep)
	{
		OrganyaPlayData();
		gOrgTimer %= gOrgSamplePerStep;
	}
	
	long mix_buffer[MIX_BUFFER_SIZE];
	memset(mix_buffer, 0, MIX_BUFFER_SIZE);

	_cpu_context_save_fp(&_thr_executing->context);
	for (SOUNDBUFFER *sound = soundBuffers; sound != NULL; sound = sound->next)
		sound->Mix(mix_buffer, SND_BUFFER_SIZE / 4);
	_cpu_context_restore_fp(&_thr_executing->context);

	int16_t *out_buffer = (int16_t*)audio_frame[audio_index];

	for (unsigned int i = 0; i < SND_BUFFER_SIZE / 2; ++i)
		out_buffer[i] = clamp(mix_buffer[i], -0x7F, 0x7F) << 8;

	DCStoreRange(out_buffer, SND_BUFFER_SIZE);
	AUDIO_InitDMA((intptr_t)out_buffer, SND_BUFFER_SIZE);
	audio_index = !audio_index;
}

bool InitDirectSound()
{
	//Init sound
	AUDIO_Init(NULL);
	AUDIO_StopDMA();
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ);
	
	AUDIO_RegisterDMACallback(StreamCallback);
	AUDIO_StartDMA();
	
	//Start organya
	StartOrganya();
	return true;
}

void EndDirectSound()
{
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
