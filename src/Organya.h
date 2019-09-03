#pragma once

#include <dsound.h>

#include "WindowsWrapper.h"

#define MAXTRACK 16
#define MAXMELODY 8
#define MAXDRAM 8

extern BOOL g_mute[MAXTRACK];	// Used by the debug Mute menu

BOOL MakeOrganyaWave(signed char track, signed char wave_no, signed char pipi);
void OrganyaPlayData();
void SetPlayPointer(long x);
BOOL LoadOrganya(const char *name);
void SetOrganyaPosition(unsigned int x);
unsigned int GetOrganyaPosition();
void PlayOrganyaMusic();
BOOL ChangeOrganyaVolume(signed int volume);
void StopOrganyaMusic();
void SetOrganyaFadeout();
BOOL StartOrganya(const char *wave_filename);
void EndOrganya();
