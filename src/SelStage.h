#pragma once

#include "WindowsWrapper.h"

typedef struct PERMIT_STAGE
{
	int index;
	int event;
} PERMIT_STAGE;

extern PERMIT_STAGE gPermitStage[8];

extern int gSelectedStage;
extern int gStageSelectTitleY;

void ClearPermitStage(void);
BOOL AddPermitStage(int index, int event);
BOOL SubPermitStage(int index);
void MoveStageSelectCursor(void);
void PutStageSelectObject(void);
int StageSelectLoop(int *p_event);
