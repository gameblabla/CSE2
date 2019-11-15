#pragma once

struct FRAME
{
	int x;
	int y;
	int *tgt_x;
	int *tgt_y;
	int wait;
	int quake;
	int quake2;
};

extern FRAME gFrame;

void MoveFrame3(void);
void GetFramePosition(int *fx, int *fy);
void SetFramePosition(int fx, int fy);
void SetFrameMyChar(void);
void SetFrameTargetMyChar(int wait);
void SetFrameTargetNpChar(int event, int wait);
void SetFrameTargetBoss(int no, int wait);
void SetQuake(int time);
void SetQuake2(int time);
void ResetQuake(void);
