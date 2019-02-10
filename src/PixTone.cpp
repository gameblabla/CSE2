#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <vector>
#include <cstring>
#include <math.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"
#include "PixTone.h"

int8_t gWaveModelTable[6][0x100];
bool wave_tables_made;

void MakeWaveTables()
{
	/* Sine wave */
	for (int i = 0; i < 0x100; i++)
		gWaveModelTable[0][i] = (sin(i * 6.283184 / 256.0) * 64.0);
	
	/* Triangle wave */
	int triangle = 0;
	for (int i = 0; i < 0x40; ++i) //Upwards
		gWaveModelTable[1][i] = (triangle++ << 6) / 0x40;
	triangle = 0;
	for (int i = 0x40; i < 0xC0; ++i) //Downwards
		gWaveModelTable[1][i] = 0x40 - (triangle++ << 6) / 0x40;
	triangle = 0;
	for (int i = 0xC0; i < 0x100; ++i) //Back upwards
		gWaveModelTable[1][i] = (triangle++ << 6) / 0x40 - 0x40;
		
	/* Saw up wave */
	for (int i = 0; i < 0x100; i++)
		gWaveModelTable[2][i] = i / 2 - 0x40;

	/* Saw down wave */
	for (int i = 0; i < 0x100; i++)
		gWaveModelTable[3][i] = 0x40 - i / 2;

	/* Square wave */
	for (int i = 0; i < 0x80; i++)
		gWaveModelTable[4][i] = 0x40;
	for (int i = 0x80; i < 0x100; i++)
		gWaveModelTable[4][i] = -0x40;

	/* White noise wave */
	rep_srand(0);
	for (int i = 0; i < 0x100; i++)
		gWaveModelTable[5][i] = (int8_t)rep_rand() / 2;
}

//Loading .pxt files
double fgetv(FILE *fp) // Load a numeric value from text file; one per line.
{
	//Create buffer
	char Buf[0x1000];
	Buf[0xFFF] = '\0';
	char *p = Buf;
	
	if (!std::fgets(Buf, sizeof(Buf) - 1, fp))
		return 0.0;
	
	// Ignore empty lines. If the line was empty, try next line.
	if (!Buf[0] || Buf[0] == '\r' || Buf[0] == '\n')
		return fgetv(fp);
	
	while (*p && *p++ != ':')
	{
	}
	
	return std::strtod(p, 0); // Parse the value and return it.
}

bool MakePixelWaveData(const std::vector<double>& pxtData, uint8_t *data)
{
	//Make wave tables if not created already
	if (!wave_tables_made)
	{
		MakeWaveTables();
		wave_tables_made = true;
	}
	
	//Get some envelope stuff
	char envelopeTable[0x100];
	memset(envelopeTable, 0, sizeof(envelopeTable));

	size_t i = 0;

	//Point A
	long double currentEnvelope = pxtData[14];
	while (i < pxtData[15])
	{
		envelopeTable[i] = (char)currentEnvelope;
		currentEnvelope = (pxtData[16] - pxtData[14])
			/ pxtData[15]
			+ currentEnvelope;
		++i;
	}

	//Point B
	long double currentEnvelopea = pxtData[16];
	while (i < pxtData[17])
	{
		envelopeTable[i] = (char)currentEnvelopea;
		currentEnvelopea = (pxtData[18] - pxtData[16])
			/ (pxtData[17] - pxtData[15])
			+ currentEnvelopea;
		++i;
	}

	//Point C
	long double currentEnvelopeb = pxtData[18];
	while (i < pxtData[19])
	{
		envelopeTable[i] = (char)currentEnvelopeb;
		currentEnvelopeb = (pxtData[20] - pxtData[18])
			/ (pxtData[19] - pxtData[17])
			+ currentEnvelopeb;
		++i;
	}

	//End
	long double currentEnvelopec = pxtData[20];
	while (i < 0x100)
	{
		envelopeTable[i] = (char)currentEnvelopec;
		currentEnvelopec = currentEnvelopec
			- pxtData[20] / (0x100 - pxtData[19]);
		++i;
	}

	long double pitchOffset = pxtData[9];
	long double mainOffset = pxtData[5];
	long double volumeOffset = pxtData[13];

	//Main
	long double mainFreq;
	if (pxtData[3] == 0.0)
		mainFreq = 0.0;
	else
		mainFreq = 256.0 / (pxtData[1] / pxtData[3]);

	//Pitch
	long double pitchFreq;
	if (pxtData[7] == 0.0)
		pitchFreq = 0.0;
	else
		pitchFreq = 256.0 / (pxtData[1] / pxtData[7]);

	//Volume
	long double volumeFreq;
	if (pxtData[11] == 0.0)
		volumeFreq = 0.0;
	else
		volumeFreq = 256.0 / (pxtData[1] / pxtData[11]);

	for (i = 0; i < pxtData[1]; ++i)
	{
		const int a = (int)(uint64_t)mainOffset % 256;
		const int v2 = (int)(uint64_t)pitchOffset % 256;

		//Input data
		data[i] = envelopeTable[(uint64_t)((long double)(i << 8) / pxtData[1])]
			* (pxtData[4]
				* gWaveModelTable[(size_t)pxtData[2]][a]
				/ 0x40
				* (pxtData[12]
					* gWaveModelTable[(size_t)pxtData[10]][(signed int)(uint64_t)volumeOffset % 0x100]
					/ 0x40
					+ 0x40)
				/ 0x40)
			/ 0x40
			+ 0x80;

		long double newMainOffset;
		if (gWaveModelTable[(size_t)pxtData[6]][v2] >= 0)
			newMainOffset = (mainFreq * 2)
			* (long double)gWaveModelTable[(size_t)pxtData[6]][(signed int)(uint64_t)pitchOffset % 256]
			* pxtData[8]
			/ 64.0
			/ 64.0
			+ mainFreq
			+ mainOffset;
		else
			newMainOffset = mainFreq
			- mainFreq
			* 0.5
			* (long double)-gWaveModelTable[(size_t)pxtData[6]][v2]
			* pxtData[8]
			/ 64.0
			/ 64.0
			+ mainOffset;

		mainOffset = newMainOffset;
		pitchOffset = pitchOffset + pitchFreq;
		volumeOffset = volumeOffset + volumeFreq;
	}

	return true;
}

bool LoadPxt(char *name, uint8_t **buf, size_t *length)
{
	//Open file
	char path[PATH_LENGTH];
	sprintf(path, "%s/Sound/%s", gDataPath, name);
	FILE *fp = fopen(path, "rb");
	
	if (!fp)
		return false;

	//Read data
	std::vector<double> lineNumbers[4];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 21; j++)
		{
			double val = fgetv(fp);
			lineNumbers[i].push_back(val);
		}
	}
	
	//Close file
	fclose(fp);

	//Get size
	size_t size = 0;
	for (int i = 0; i < 4; i++)
	{
		if (lineNumbers[i][1] > size)
			size = (size_t)lineNumbers[i][1];
	}

	//Allocate buffers
	uint8_t *dest = (uint8_t*)malloc(size);
	uint8_t *pBlock = (uint8_t*)malloc(size);

	if (dest && pBlock)
	{
		//Set buffers to default value of 0x80
		memset(dest, 0x80, size);
		memset(pBlock, 0x80, size);

		for (int i = 0; i < 4; ++i)
		{
			//Get wave data
			if (!MakePixelWaveData(lineNumbers[i], dest))
			{
				printf("MakePixelWaveData failed for %s\n", name);
				free(dest);
				free(pBlock);
				return -1;
			}

			//Put data into buffer
			for (int j = 0; j < lineNumbers[i][1]; ++j)
			{
				if (dest[j] + pBlock[j] - 0x100 >= -0x7F)
				{
					if (dest[j] + pBlock[j] - 0x100 <= 0x7F)
						pBlock[j] += dest[j] - 0x80;
					else
						pBlock[j] = (uint8_t)-1;
				}
				else
				{
					pBlock[j] = 0;
				}
			}
		}

		//Put data from buffers into main sound buffer
		*buf = (uint8_t*)malloc(size);

		if (!*buf)
		{
			printf("Failed to allocate buffer for %s\n", name);
			free(dest);
			free(pBlock);
			return false;
		}

		*length = size;
		memcpy(*buf, pBlock, size);

		//Free the two buffers
		free(dest);
		free(pBlock);
		return true;
	}
	
	printf("Failed to allocate dest or pBlock for %s\n", name);
	free(dest);
	free(pBlock);
	return false;
}
