#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BACK
{
	BOOL flag;	// Basically unused
	int partsW;
	int partsH;
	int numX;
	int numY;
	int type;
	int fx;
} BACK;

extern BACK gBack;
extern int gWaterY;

BOOL InitBack(const char *fName, int type);
void ActBack(void);
void PutBack(int fx, int fy);
void PutFront(int fx, int fy);

#ifdef __cplusplus
}
#endif
