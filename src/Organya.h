#pragma once

#include "WindowsWrapper.h"

#define MAXTRACK 16
#define MAXMELODY 8
#define MAXDRAM 8

extern unsigned short organya_timer;

extern BOOL g_mute[MAXTRACK];	// Used by the debug Mute menu

BOOL MakeOrganyaWave(signed char track, signed char wave_no, signed char pipi);
void OrganyaPlayData(void);
void SetPlayPointer(long x);
BOOL LoadOrganya(const char *name);
void SetOrganyaPosition(unsigned int x);
unsigned int GetOrganyaPosition(void);
void PlayOrganyaMusic(void);
BOOL ChangeOrganyaVolume(signed int volume);
void StopOrganyaMusic(void);
void SetOrganyaFadeout(void);
BOOL StartOrganya(const char *wave_filename);
void EndOrganya(void);
void UpdateOrganya(void);
