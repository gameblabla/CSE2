#include <stdint.h>
#include <string.h>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Map.h"
#include "MapName.h"
#include "MyChar.h"
#include "Draw.h"
#include "Tags.h"
#include "Frame.h"
#include "Caret.h"
#include "NpChar.h"
#include "TextScr.h"
#include "Organya.h"
#include "Back.h"
#include "Stage.h"

STAGE_TABLE *gTMT;
int gStageNo;

//Stage table functions
bool InitStageTable()
{
	//Get path
	char path[PATH_LENGTH];
	sprintf(path, "%s/stage.tbl", gDataPath);
	
	//Open file
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	
	if (!fp)
		return false;
	
	//Get amount of stages and allocate stage data
	size_t stages = SDL_RWsize(fp) / 0xC8;
	gTMT = (STAGE_TABLE*)calloc(stages, sizeof(STAGE_TABLE));
	
	//Read data
	for (size_t i = 0; i < stages; i++)
	{
		SDL_RWread(fp, &gTMT[i].parts, 1, 0x20);
		SDL_RWread(fp, &gTMT[i].map, 1, 0x20);
		gTMT[i].bkType = SDL_ReadLE32(fp);
		SDL_RWread(fp, &gTMT[i].back, 1, 0x20);
		SDL_RWread(fp, &gTMT[i].npc, 1, 0x20);
		SDL_RWread(fp, &gTMT[i].boss, 1, 0x20);
		gTMT[i].boss_no = SDL_ReadU8(fp);
		SDL_RWread(fp, &gTMT[i].name, 1, 0x20);
		
		//Padding (3 bytes)
		uint8_t nul[3];
		SDL_RWread(fp, &nul, 1, 3);
	}
	
	SDL_RWclose(fp);
	return true;
}

void ReleaseStageTable()
{
	free(gTMT);
	gTMT = nullptr;
}

bool TransferStage(int no, int w, int x, int y)
{
	//Move character
	SetMyCharPosition(x << 13, y << 13);
	
	bool bError = false;
	bool result;
	
	//Get path
	char path_dir[20];
	strcpy(path_dir, "Stage");
	
	//Load tileset
	char path[PATH_LENGTH];
	sprintf(path, "%s/Prt%s", path_dir, gTMT[no].parts);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_TILESET))
		bError = true;

	sprintf(path, "%s/%s.pxa", path_dir, gTMT[no].parts);
	if (!LoadAttributeData(path))
		bError = true;
	
	//Load tilemap
	sprintf(path, "%s/%s.pxm", path_dir, gTMT[no].map);
	if (!LoadMapData2(path))
		bError = true;

	//Load NPCs
	sprintf(path, "%s/%s.pxe", path_dir, gTMT[no].map);
	if (!LoadEvent(path))
		bError = true;

	//Load script
	sprintf(path, "%s/%s.tsc", path_dir, gTMT[no].map);
	if (!LoadTextScript_Stage(path))
		bError = true;
	
	//Load background
	strcpy(path, gTMT[no].back);
	if (!InitBack(path, gTMT[no].bkType))
		bError = true;
	
	//Get path
	strcpy(path_dir, "Npc");
	
	//Load NPC sprite sheets
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].npc);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_1))
		bError = true;
	
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].boss);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_2))
		bError = true;
	
	if (bError)
	{
		printf("Failed to load stage %d\n", no);
		return false;
	}
	else
	{
		//Load map name
		ReadyMapName(gTMT[no].name);
		
		//StartTextScript(w);
		SetFrameMyChar();
		//ClearBullet();
		InitCaret();
		//ClearValueView();
		ResetQuake();
		//InitBossChar(gTMT[no].boss_no);
		//ResetFlash();
		gStageNo = no;
		return true;
	}
	
	return false;
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
