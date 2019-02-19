#pragma once

#include "WindowsWrapper.h"

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

extern BACK gBack;
extern int gWaterY;

BOOL InitBack(const char *fName, int type);
void ActBack();
void PutBack(int fx, int fy);
void PutFront(int fx, int fy);
