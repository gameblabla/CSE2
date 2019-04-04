#include "PixTone.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "CommonDefines.h"
#include "Tags.h"

#include "WindowsWrapper.h"

static signed char gWaveModelTable[6][256];

void MakeWaveTables(void)
{
	int i, a;

	// Sine wave
	for (i = 0; i < 256; ++i)
	{
		gWaveModelTable[0][i] = (signed char)(sin(i * 6.283184 / 256.0) * 64.0);
		a = gWaveModelTable[0][i];
	}

	// Triangle wave
	for (a = 0, i = 0; i < 0x40; ++i)
	{
		// Upwards
		gWaveModelTable[1][i] = (a * 0x40) / 0x40;
		++a;
	}
	for (a = 0; i < 0xC0; ++i)
	{
		// Downwards
		gWaveModelTable[1][i] = 0x40 - (a * 0x40) / 0x40;
		++a;
	}
	for (a = 0; i < 0x100; ++i)
	{
		// Back up
		gWaveModelTable[1][i] = (a * 0x40) / 0x40 - 0x40;
		++a;
	}

	// Saw up wave
	for (i = 0; i < 0x100; ++i)
		gWaveModelTable[2][i] = i / 2 - 0x40;

	// Saw down wave
	for (i = 0; i < 0x100; ++i)
		gWaveModelTable[3][i] = 0x40 - i / 2;

	// Square wave
	for (i = 0; i < 0x80; ++i)
		gWaveModelTable[4][i] = 0x40;
	for (; i < 0x100; ++i)
		gWaveModelTable[4][i] = -0x40;

	// White noise wave
	//char path[PATH_LENGTH];
	rep_srand(0);
	for (i = 0; i < 256; ++i)
		gWaveModelTable[5][i] = (signed char)(rep_rand() & 0xFF) / 2; //had to make this signed, char is unsigned by default why
}

BOOL MakePixelWaveData(const PIXTONEPARAMETER *ptp, unsigned char *pData)
{
	// The Linux port added a cute optimisation here, where MakeWaveTables is only called once during the game's execution
	MakeWaveTables();

	signed char envelopeTable[0x100];
	memset(envelopeTable, 0, 0x100);

	int i = 0;
	double dEnvelope;

	dEnvelope = ptp->initial;
	while (i < ptp->pointAx)
	{
		envelopeTable[i] = (signed char)dEnvelope;
		dEnvelope = ((double)ptp->pointAy - ptp->initial) / ptp->pointAx + dEnvelope;
		++i;
	}

	dEnvelope = ptp->pointAy;
	while (i < ptp->pointBx)
	{
		envelopeTable[i] = (signed char)dEnvelope;
		dEnvelope = ((double)ptp->pointBy - ptp->pointAy) / (double)(ptp->pointBx - ptp->pointAx) + dEnvelope;
		++i;
	}

	dEnvelope = ptp->pointBy;
	while (i < ptp->pointCx)
	{
		envelopeTable[i] = (signed char)dEnvelope;
		dEnvelope = ((double)ptp->pointCy - ptp->pointBy) / (double)(ptp->pointCx - ptp->pointBx) + dEnvelope;
		++i;
	}

	dEnvelope = ptp->pointCy;
	while (i < 256)
	{
		envelopeTable[i] = (signed char)dEnvelope;
		dEnvelope = (double)dEnvelope - ptp->pointCy / (double)(256 - ptp->pointCx);
		++i;
	}

	double dPitch = ptp->oPitch.offset;
	double dMain = ptp->oMain.offset;
	double dVolume = ptp->oVolume.offset;

	double d1;
	double d2;
	double d3;

	if (ptp->oMain.num == 0.0)
		d1 = 0.0;
	else
		d1 = 256.0 / (ptp->size / ptp->oMain.num);

	if (ptp->oPitch.num == 0.0)
		d2 = 0.0;
	else
		d2 = 256.0 / (ptp->size / ptp->oPitch.num);

	if (ptp->oVolume.num == 0.0)
		d3 = 0.0;
	else
		d3 = 256.0 / (ptp->size / ptp->oVolume.num);

	for (i = 0; i < ptp->size; ++i)
	{
		const int a = (int)dMain % 256;
		const int b = (int)dPitch % 256;
		const int c = (int)dVolume % 256;
		const int d = (int)((double)(i * 0x100) / ptp->size);
		pData[i] = gWaveModelTable[ptp->oMain.model][a]
			* ptp->oMain.top
			/ 64
			* (gWaveModelTable[ptp->oVolume.model][c] * ptp->oVolume.top / 64 + 64)
			/ 64
			* envelopeTable[d]
			/ 64
			+ 128;

		if (gWaveModelTable[ptp->oPitch.model][b] < 0)
			dMain = d1 - d1 * 0.5 * -gWaveModelTable[ptp->oPitch.model][b] * ptp->oPitch.top / 64.0 / 64.0 + dMain;
		else
			dMain = d1 + d1 * 2.0 * gWaveModelTable[ptp->oPitch.model][b] * ptp->oPitch.top / 64.0 / 64.0 + dMain;

		dPitch = dPitch + d2;
		dVolume = dVolume + d3;
	}

	return TRUE;
}
