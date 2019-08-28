#pragma once

#include <dsound.h>

#include "WindowsWrapper.h"

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
BOOL StartOrganya(LPDIRECTSOUND lpDS, const char *wave_filename);
void EndOrganya();
