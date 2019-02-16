#pragma once

struct STAGE_TABLE
{
	char parts[0x20];
	char map[0x20];
	int bkType;
	char back[0x20];
	char npc[0x20];
	char boss[0x20];
	char boss_no;
	char name[0x20];
};

extern int gStageNo;
extern int gMusicNo;

bool LoadStageTable(const char *path);
bool TransferStage(int no, int w, int x, int y);
void ChangeMusic(int no);
void ReCallMusic();
