#include "Stage.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Back.h"
#include "Boss.h"
#include "Bullet.h"
#include "Caret.h"
#include "CommonDefines.h"
#include "Draw.h"
#include "File.h"
#include "Flash.h"
#include "Frame.h"
#include "Map.h"
#include "MapName.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Organya.h"
#include "Tags.h"
#include "TextScr.h"
#include "ValueView.h"

int gStageNo;

STAGE_TABLE *gTMT;

bool LoadStageTable(const char *path)
{
	bool success = false;

	unsigned char *file_buffer;
	const long file_size = LoadFileToMemory(path, &file_buffer);

	if (file_size != -1)
	{
		if (file_size % 0xC8)
			printf("stage.tbl has partial stage entry\n");

		const long entry_count = file_size / 0xE5;

		gTMT = (STAGE_TABLE*)malloc(entry_count * sizeof(STAGE_TABLE));

		if (gTMT != NULL)
		{
			for (long i = 0; i < entry_count; ++i)
			{
				unsigned char *entry = file_buffer + i * 0xE5;

				memcpy(gTMT[i].parts, entry, 0x20);
				memcpy(gTMT[i].map, entry + 0x20, 0x20);
				gTMT[i].bkType = (entry[0x40 + 3] << 24) | (entry[0x40 + 2] << 16) | (entry[0x40 + 1] << 8) | entry[0x40];
				memcpy(gTMT[i].back, entry + 0x44, 0x20);
				memcpy(gTMT[i].npc, entry + 0x64, 0x20);
				memcpy(gTMT[i].boss, entry + 0x84, 0x20);
				gTMT[i].boss_no = entry[0xA4];
#ifdef JAPANESE
				memcpy(gTMT[i].name, entry + 0xA5, 0x20);
#else
				memcpy(gTMT[i].name, entry + 0xC5, 0x20);
#endif
			}

			success = true;
		}

		free(file_buffer);
	}

	if (success == false)
		printf("Failed to load stage.tbl\n");

	return success;
}

BOOL TransferStage(int no, int w, int x, int y)
{
	//Move character
	SetMyCharPosition(x << 13, y << 13);
	
	BOOL bError = FALSE;
	
	//Get path
	char path_dir[20];
	strcpy(path_dir, "Stage");
	
	//Load tileset
	char path[PATH_LENGTH];
	sprintf(path, "%s/Prt%s", path_dir, gTMT[no].parts);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_TILESET))
		bError = TRUE;

	sprintf(path, "%s/%s.pxa", path_dir, gTMT[no].parts);
	if (!LoadAttributeData(path))
		bError = TRUE;
	
	//Load tilemap
	sprintf(path, "%s/%s.pxm", path_dir, gTMT[no].map);
	if (!LoadMapData2(path))
		bError = TRUE;

	//Load NPCs
	sprintf(path, "%s/%s.pxe", path_dir, gTMT[no].map);
	if (!LoadEvent(path))
		bError = TRUE;

	//Load script
	sprintf(path, "%s/%s.tsc", path_dir, gTMT[no].map);
	if (!LoadTextScript_Stage(path))
		bError = TRUE;
	
	//Load background
	strcpy(path, gTMT[no].back);
	if (!InitBack(path, gTMT[no].bkType))
		bError = TRUE;
	
	//Get path
	strcpy(path_dir, "Npc");
	
	//Load NPC sprite sheets
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].npc);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_1))
		bError = TRUE;
	
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].boss);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_2))
		bError = TRUE;
	
	if (bError)
	{
		printf("Failed to load stage %d\n", no);
		return FALSE;
	}
	else
	{
		//Load map name
		ReadyMapName(gTMT[no].name);
		
		StartTextScript(w);
		SetFrameMyChar();
		ClearBullet();
		InitCaret();
		ClearValueView();
		ResetQuake();
		InitBossChar(gTMT[no].boss_no);
		ResetFlash();
		gStageNo = no;
		return TRUE;
	}
	
	return FALSE;
}

//Music
const char *gMusicTable[42] =
{
	"XXXX",
	"WANPAKU",
	"ANZEN",
	"GAMEOVER",
	"GRAVITY",
	"WEED",
	"MDOWN2",
	"FIREEYE",
	"VIVI",
	"MURA",
	"FANFALE1",
	"GINSUKE",
	"CEMETERY",
	"PLANT",
	"KODOU",
	"FANFALE3",
	"FANFALE2",
	"DR",
	"ESCAPE",
	"JENKA",
	"MAZE",
	"ACCESS",
	"IRONH",
	"GRAND",
	"CURLY",
	"OSIDE",
	"REQUIEM",
	"WANPAK2",
	"QUIET",
	"LASTCAVE",
	"BALCONY",
	"LASTBTL",
	"LASTBT3",
	"ENDING",
	"ZONBIE",
	"BDOWN",
	"HELL",
	"JENKA2",
	"MARINE",
	"BALLOS",
	"TOROKO",
	"WHITE"
};

unsigned int gOldPos;
int gOldNo;
int gMusicNo;

void ChangeMusic(int no)
{
	if (!no || no != gMusicNo)
	{
		//Stop and keep track of old song
		gOldPos = GetOrganyaPosition();
		gOldNo = gMusicNo;
		StopOrganyaMusic();
		
		//Load .org
		LoadOrganya(gMusicTable[no]);
		
		//Reset position, volume, and then play the song
		ChangeOrganyaVolume(100);
		SetOrganyaPosition(0);
		PlayOrganyaMusic();
		gMusicNo = no;
	}
}

void ReCallMusic()
{
	//Stop old song
	StopOrganyaMusic();
	
	//Load .org that was playing before
	LoadOrganya(gMusicTable[gOldNo]);
	
	//Reset position, volume, and then play the song
	SetOrganyaPosition(gOldPos);
	ChangeOrganyaVolume(100);
	PlayOrganyaMusic();
	gMusicNo = gOldNo;
}
