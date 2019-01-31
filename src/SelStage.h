#pragma once

void ClearPermitStage(void);
bool AddPermitStage(int index, int event);
bool SubPermitStage(int index);
void MoveStageSelectCursor(void);
void PutStageSelectObject(void);
int StageSelectLoop(int *p_event);
