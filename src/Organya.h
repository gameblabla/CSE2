#pragma once

#include <stdint.h>

//Below are Organya song data structures
struct NOTELIST {
	NOTELIST *from; //Previous address
	NOTELIST *to; //Next address

	int32_t x; //Position
	unsigned char length; //Sound length
	unsigned char y; //Sound height
	unsigned char volume; //Volume
	unsigned char pan;
};

//Track data * 8
struct TRACKDATA {
	uint16_t freq; //Frequency (1000 is default)
	uint8_t wave_no; //Waveform No.
	int8_t pipi;
	uint16_t note_num; //Number of notes

	NOTELIST *note_p;
	NOTELIST *note_list;
};

//Unique information held in songs
struct MUSICINFO {
	uint16_t wait;
	bool loaded;
	bool playing;
	unsigned char line; //Number of lines in one measure
	unsigned char dot; //Number of dots per line
	uint16_t alloc_note; //Number of allocated notes
	int32_t repeat_x; //Repeat
	int32_t end_x; //End of song (Return to repeat)
	TRACKDATA tdata[16];
};

extern unsigned int gOrgTimer;
extern unsigned int gOrgSamplePerStep;

bool MakeOrganyaWave(int8_t track, int8_t wave_no, int8_t pipi);
void OrganyaPlayData();
void SetPlayPointer(int32_t x);
void LoadOrganya(const char *name);
void SetOrganyaPosition(unsigned int x);
unsigned int GetOrganyaPosition();
void PlayOrganyaMusic();
bool ChangeOrganyaVolume(signed int volume);
void StopOrganyaMusic();
void SetOrganyaFadeout();
void OrganyaStartTimer(unsigned int wait);
void OrganyaEndTimer();
void StartOrganya();
void EndOrganya();
