#pragma once

#include "WindowsWrapper.h"

#include "MainLoop.h"

typedef struct StageSelectLoop_Data
{
	int event;
	void (*caller_return)(MainLoopMeta *meta, int return_value);
} StageSelectLoop_Data;

struct PERMIT_STAGE
{
	int index;
	int event;
};

extern PERMIT_STAGE gPermitStage[8];

void ClearPermitStage(void);
BOOL AddPermitStage(int index, int event);
BOOL SubPermitStage(int index);
void MoveStageSelectCursor(void);
void PutStageSelectObject(void);
void StageSelectLoop(MainLoopMeta *meta);
