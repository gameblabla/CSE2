#pragma once

#include <stddef.h>

#include "WindowsWrapper.h"

#include "PixTone.h"
#include "SoftwareMixer.h"

class SOUNDBUFFER
{
	public:
		SOUNDBUFFER(unsigned int frequency, const unsigned char *samples, size_t length);
		~SOUNDBUFFER();

		void Release();

		void SetCurrentPosition(unsigned long dwNewPosition);
		void SetFrequency(unsigned long dwFrequency);
		void SetVolume(long lVolume);
		void SetPan(long lPan);
		void Play(bool bLooping);
		void Stop();

		void Mix(float *buffer, size_t frames);

		SOUNDBUFFER *next;

	private:
		Mixer_Sound *mixer_sound;
};

//Music ID enum
enum MUSIC_IDS
{
	mus_Silence = 0x0,
	mus_MischievousRobot = 0x1,
	mus_Safety = 0x2,
	mus_GameOver = 0x3,
	mus_Gravity = 0x4,
	mus_OnToGrasstown = 0x5,
	mus_Meltdown2 = 0x6,
	mus_EyesOfFlame = 0x7,
	mus_Gestation = 0x8,
	mus_MimigaTown = 0x9,
	mus_GetItem = 0xA,
	mus_BalrogsTheme = 0xB,
	mus_Cemetary = 0xC,
	mus_Plant = 0xD,
	mus_Pulse = 0xE,
	mus_Victory = 0xF,
	mus_GetLifeCapsule = 0x10,
	mus_Tyrant = 0x11,
	mus_Run = 0x12,
	mus_Jenka1 = 0x13,
	mus_LabyrinthFight = 0x14,
	mus_Access = 0x15,
	mus_Oppression = 0x16,
	mus_Geothermal = 0x17,
	mus_CaveStory = 0x18,
	mus_Moonsong = 0x19,
	mus_Herosend = 0x1A,
	mus_ScorchingBack = 0x1B,
	mus_Quiet = 0x1C,
	mus_FinalCave = 0x1D,
	mus_Balcony = 0x1E,
	mus_Charge = 0x1F,
	mus_LastBattle = 0x20,
	mus_TheWayBackHome = 0x21,
	mus_Zombie = 0x22,
	mus_BreakDown = 0x23,
	mus_RunningHell = 0x24,
	mus_Jenka2 = 0x25,
	mus_LivingWaterway = 0x26,
	mus_SealChamber = 0x27,
	mus_TorokosTheme = 0x28,
	mus_White = 0x29
};

#define SOUND_NO 0x100
extern SOUNDBUFFER* lpSECONDARYBUFFER[SOUND_NO];

BOOL InitDirectSound();
void EndDirectSound();
void PlaySoundObject(int no, int mode);
void ChangeSoundFrequency(int no, unsigned long rate);
void ChangeSoundVolume(int no, long volume);
void ChangeSoundPan(int no, long pan);
int MakePixToneObject(const PIXTONEPARAMETER *ptp, int ptp_num, int no);
