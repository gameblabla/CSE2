#pragma once

#include <stddef.h>

#include "WindowsWrapper.h"

#include "PixTone.h"

class SOUNDBUFFER
{
	public:
		SOUNDBUFFER(size_t bufSize);
		~SOUNDBUFFER();

		void Release();

		void Lock(unsigned char **buffer, size_t *size);
		void Unlock();

		void SetCurrentPosition(unsigned long dwNewPosition);
		void SetFrequency(unsigned long dwFrequency);
		void SetVolume(long lVolume);
		void SetPan(long lPan);
		void Play(bool bLooping);
		void Stop();

		void Mix(float *buffer, size_t frames);

		SOUNDBUFFER *next;

	private:
		unsigned char *data;
		size_t size;

		bool playing;
		bool looping;
		bool looped;

		double frequency;
		double volume;
		double volume_l;
		double volume_r;
		double samplePosition;
};

#define SE_MAX 160	// According to the Organya source code release, this is the real name for this constant
extern SOUNDBUFFER* lpSECONDARYBUFFER[SE_MAX];

BOOL InitDirectSound();
void EndDirectSound();
void PlaySoundObject(int no, int mode);
void ChangeSoundFrequency(int no, unsigned long rate);
void ChangeSoundVolume(int no, long volume);
void ChangeSoundPan(int no, long pan);
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no);
