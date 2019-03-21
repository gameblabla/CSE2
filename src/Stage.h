#pragma once

#include "WindowsWrapper.h"

struct STAGE_TABLE
{
	char parts[0x10];
	char map[0x10];
	char bkType;
	char back[0x10];
	char npc[0x10];
	char boss[0x10];
	char boss_no;
	char name[0x22];
};

extern int gStageNo;
extern int gMusicNo;

bool LoadStageTable(const char *path);
BOOL TransferStage(int no, int w, int x, int y);
void ChangeMusic(int no);
void ReCallMusic();
