#pragma once

struct BACK
{
	int flag;
	int partsW;
	int partsH;
	int numX;
	int numY;
	int type;
	int fx;
};

extern int gWaterY;

bool InitBack(char *fName, int type);
