#pragma once

#include "WindowsWrapper.h"

struct PIXTONEPARAMETER2
{
	int model;
	double num;
	int top;
	int offset;
};

struct PIXTONEPARAMETER
{
	int use;
	int size;
	PIXTONEPARAMETER2 oMain;
	PIXTONEPARAMETER2 oPitch;
	PIXTONEPARAMETER2 oVolume;
	int initial;
	int pointAx;
	int pointAy;
	int pointBx;
	int pointBy;
	int pointCx;
	int pointCy;
};

void MakeWaveTables(void);
BOOL MakePixelWaveData(const PIXTONEPARAMETER *ptp, unsigned char *pData);
