#pragma once

struct PERMIT_STAGE
{
	int index;
	int event;
};

extern PERMIT_STAGE gPermitStage[8];

void ClearPermitStage(void);
bool AddPermitStage(int index, int event);
bool SubPermitStage(int index);
void MoveStageSelectCursor(void);
void PutStageSelectObject(void);
int StageSelectLoop(int *p_event);
