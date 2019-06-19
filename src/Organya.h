#pragma once

#include "WindowsWrapper.h"

//Below are Organya song data structures
struct NOTELIST
{
	NOTELIST *from; //Previous address
	NOTELIST *to; //Next address

	long x; //Position
	unsigned char length; //Sound length
	unsigned char y; //Sound height
	unsigned char volume; //Volume
	unsigned char pan;
};

//Track data * 8
struct TRACKDATA
{
	unsigned short freq; //Frequency (1000 is default)
	unsigned char wave_no; //Waveform No.
	signed char pipi;
	unsigned short note_num; //Number of notes

	NOTELIST *note_p;
	NOTELIST *note_list;
};

//Unique information held in songs
struct MUSICINFO
{
	unsigned short wait;
	BOOL loaded;
	BOOL playing;
	unsigned char line; //Number of lines in one measure
	unsigned char dot; //Number of dots per line
	unsigned short alloc_note; //Number of allocated notes
	long repeat_x; //Repeat
	long end_x; //End of song (Return to repeat)
	TRACKDATA tdata[16];
};

extern unsigned int gOrgTimer;
extern unsigned int gOrgWait;

BOOL MakeOrganyaWave(signed char track, signed char wave_no, signed char pipi);
void OrganyaPlayData();
void SetPlayPointer(long x);
void LoadOrganya(const char *name);
void SetOrganyaPosition(unsigned int x);
unsigned int GetOrganyaPosition();
void PlayOrganyaMusic();
BOOL ChangeOrganyaVolume(signed int volume);
void StopOrganyaMusic();
void SetOrganyaFadeout();
void OrganyaStartTimer(unsigned int wait);
void OrganyaEndTimer();
void StartOrganya();
void EndOrganya();
