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

#ifdef JAPANESE
#define STAGE_ENTRY(parts, map, bkType, back, npc, boss, boss_no, name_en, name_jp) {parts, map, bkType, back, npc, boss, boss_no, name_jp}
#else
#define STAGE_ENTRY(parts, map, bkType, back, npc, boss, boss_no, name_en, name_jp) {parts, map, bkType, back, npc, boss, boss_no, name_en}
#endif

int gStageNo;

static const STAGE_TABLE gTMTDefault[95] = {
	STAGE_ENTRY("0", "0", 4, "bk0", "Guest", "0", 0, "Null", "–³"),
	STAGE_ENTRY("Pens", "Pens1", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "ƒA[ƒT[‚Ì‰Æ"),
	STAGE_ENTRY("Eggs", "Eggs", 1, "bkGreen", "Eggs1", "Ravil", 0, "Egg Corridor", "ƒ^ƒ}ƒS‰ñ˜L"),
	STAGE_ENTRY("EggX", "EggX", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "ƒ^ƒ}ƒS No.00"),
	STAGE_ENTRY("EggIn", "Egg6", 4, "bk0", "Eggs1", "0", 0, "Egg No. 06", "ƒ^ƒ}ƒS No.06"),
	STAGE_ENTRY("Store", "EggR", 4, "bk0", "Eggs1", "0", 0, "Egg Observation Room", "ƒ^ƒ}ƒSŠÄŽ‹Žº"),
	STAGE_ENTRY("Weed", "Weed", 1, "bkBlue", "Weed", "0", 0, "Grasstown", "ƒNƒTƒ€ƒ‰"),
	STAGE_ENTRY("Barr", "Santa", 4, "bk0", "Weed", "0", 0, "Santa's House", "ƒTƒ“ƒ^‚Ì‰Æ"),
	STAGE_ENTRY("Barr", "Chako", 1, "bkBlue", "Guest", "0", 0, "Chaco's House", "ƒ`ƒƒƒR‚Ì‰Æ"),
	STAGE_ENTRY("Maze", "MazeI", 4, "bk0", "Maze", "0", 0, "Labyrinth I", "–À‹{‚h"),
	STAGE_ENTRY("Sand", "Sand", 1, "bkGreen", "Sand", "Omg", 1, "Sand Zone", "»‹æ"),
	STAGE_ENTRY("Mimi", "Mimi", 1, "bkBlue", "Guest", "0", 0, "Mimiga Village", "ƒ~ƒ~ƒK[‚Ì‘º"),
	STAGE_ENTRY("Cave", "Cave", 4, "bk0", "Cemet", "0", 0, "First Cave", "Å‰‚Ì“´ŒA"),
	STAGE_ENTRY("Cave", "Start", 4, "bk0", "Cemet", "0", 0, "Start Point", "ƒXƒ^[ƒg’n“_"),
	STAGE_ENTRY("Mimi", "Barr", 4, "bk0", "Cemet", "Bllg", 0, "Shack", "ƒoƒ‰ƒbƒN¬‰®"),
	STAGE_ENTRY("Mimi", "Pool", 1, "bkBlue", "Guest", "0", 0, "Reservoir", "’™…’r"),
	STAGE_ENTRY("Mimi", "Cemet", 4, "bk0", "Cemet", "0", 0, "Graveyard", "‚Í‚©‚Î"),
	STAGE_ENTRY("Mimi", "Plant", 1, "bkGreen", "Plant", "0", 0, "Yamashita Farm", "ŽR‰º”_‰€"),
	STAGE_ENTRY("Store", "Shelt", 4, "bk0", "Eggs1", "0", 0, "Shelter", "ƒVƒFƒ‹ƒ^["),
	STAGE_ENTRY("Pens", "Comu", 1, "bkBlue", "Guest", "0", 0, "Assembly Hall", "W‰ïê"),
	STAGE_ENTRY("Mimi", "MiBox", 4, "bk0", "0", "0", 0, "Save Point", "ƒZ[ƒuƒ|ƒCƒ“ƒg"),
	STAGE_ENTRY("Store", "EgEnd1", 4, "bk0", "0", "0", 0, "Side Room", "ƒ^ƒ}ƒS‰ñ˜L‚ÌŒÂŽº"),
	STAGE_ENTRY("Store", "Cthu", 4, "bk0", "0", "0", 0, "Cthulhu's Abode", "ƒNƒgƒDƒ‹[‚ÌZˆ"),
	STAGE_ENTRY("EggIn", "Egg1", 4, "bk0", "Eggs1", "0", 0, "Egg No. 01", "ƒ^ƒ}ƒS No.01"),
	STAGE_ENTRY("Pens", "Pens2", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "ƒA[ƒT[‚Ì‰Æ"),
	STAGE_ENTRY("Barr", "Malco", 1, "bkBlue", "Weed", "Bllg", 0, "Power Room", "“dŒ¹Žº"),
	STAGE_ENTRY("Barr", "WeedS", 1, "bkBlue", "0", "0", 0, "Save Point", "ƒZ[ƒuƒ|ƒCƒ“ƒg"),
	STAGE_ENTRY("Store", "WeedD", 1, "bkBlue", "0", "0", 0, "Execution Chamber", "ˆŒYŽº"),
	STAGE_ENTRY("Weed", "Frog", 2, "bkGreen", "Weed", "Frog", 2, "Gum", "ƒKƒ€"),
	STAGE_ENTRY("Sand", "Curly", 4, "bk0", "Sand", "Curly", 0, "Sand Zone Residence", "»‹æ’“ÝŠ"),
	STAGE_ENTRY("Pens", "WeedB", 1, "bkBlue", "Ravil", "0", 0, "Grasstown Hut", "ƒNƒTƒ€ƒ‰‚Ì¬‰®"),
	STAGE_ENTRY("River", "Stream", 5, "bkBlue", "Stream", "IronH", 5, "Main Artery", "‘å“®–¬"),
	STAGE_ENTRY("Pens", "CurlyS", 4, "bk0", "Sand", "Curly", 0, "Small Room", "¬•”‰®"),
	STAGE_ENTRY("Barr", "Jenka1", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "ƒWƒFƒ“ƒJ‚Ì‰Æ"),
	STAGE_ENTRY("Sand", "Dark", 1, "bkBlack", "Sand", "0", 0, "Deserted House", "”p‰®"),
	STAGE_ENTRY("Gard", "Gard", 1, "bkGard", "Toro", "Bllg", 0, "Sand Zone Storehouse", "»‹æ‘qŒÉ"),
	STAGE_ENTRY("Barr", "Jenka2", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "ƒWƒFƒ“ƒJ‚Ì‰Æ"),
	STAGE_ENTRY("Sand", "SandE", 1, "bkGreen", "Sand", "Bllg", 0, "Sand Zone", "»‹æ"),
	STAGE_ENTRY("Maze", "MazeH", 4, "bk0", "Maze", "0", 0, "Labyrinth H", "–À‹{‚g"),
	STAGE_ENTRY("Maze", "MazeW", 1, "bkMaze", "Maze", "X", 3, "Labyrinth W", "–À‹{‚v"),
	STAGE_ENTRY("Maze", "MazeO", 4, "bk0", "Guest", "0", 0, "Camp", "ƒLƒƒƒ“ƒv"),
	STAGE_ENTRY("Maze", "MazeD", 4, "bk0", "Guest", "Dark", 0, "Clinic Ruins", "f—ÃŠÕ"),
	STAGE_ENTRY("Store", "MazeA", 4, "bk0", "Maze", "0", 0, "Labyrinth Shop", "–À‹{‚Ì“X"),
	STAGE_ENTRY("Maze", "MazeB", 1, "bkBlue", "Maze", "0", 0, "Labyrinth B", "–À‹{‚a"),
	STAGE_ENTRY("Maze", "MazeS", 2, "bkGray", "Maze", "Bllg", 0, "Boulder Chamber", "‘åÎ‚ÌÇ‚®Š"),
	STAGE_ENTRY("Maze", "MazeM", 1, "bkRed", "Maze", "0", 0, "Labyrinth M", "–À‹{‚l"),
	STAGE_ENTRY("Cave", "Drain", 3, "bkWater", "Cemet", "0", 0, "Dark Place", "ˆÃ‚¢Š"),
	STAGE_ENTRY("Almond", "Almond", 3, "bkWater", "Cemet", "Almo1", 4, "Core", "ƒRƒA"),
	STAGE_ENTRY("River", "River", 2, "bkGreen", "Weed", "0", 0, "Waterway", "…˜H"),
	STAGE_ENTRY("Eggs", "Eggs2", 1, "bkGreen", "Eggs2", "0", 0, "Egg Corridor?", "ƒ^ƒ}ƒS‰ñ˜LH"),
	STAGE_ENTRY("Store", "Cthu2", 4, "bk0", "Eggs1", "0", 0, "Cthulhu's Abode?", "ƒNƒgƒDƒ‹[‚ÌZˆH"),
	STAGE_ENTRY("Store", "EggR2", 4, "bk0", "Eggs1", "TwinD", 6, "Egg Observation Room?", "ƒ^ƒ}ƒSŠÄŽ‹ŽºH"),
	STAGE_ENTRY("EggX", "EggX2", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "ƒ^ƒ}ƒS No.00"),
	STAGE_ENTRY("Oside", "Oside", 6, "bkMoon", "Moon", "0", 0, "Outer Wall", "ŠO•Ç"),
	STAGE_ENTRY("Store", "EgEnd2", 4, "bk0", "Eggs1", "0", 0, "Side Room", "ƒ^ƒ}ƒS‰ñ˜L‚ÌŒÂŽº"),
	STAGE_ENTRY("Store", "Itoh", 2, "bkBlue", "Guest", "0", 0, "Storehouse", "‘qŒÉ"),
	STAGE_ENTRY("Cent", "Cent", 1, "bkGreen", "Guest", "Cent", 0, "Plantation", "‘å”_‰€"),
	STAGE_ENTRY("Jail", "Jail1", 4, "bk0", "Guest", "Cent", 0, "Jail No. 1", "‘æ‚P˜S"),
	STAGE_ENTRY("Jail", "Momo", 4, "bk0", "Guest", "0", 0, "Hideout", "ƒJƒNƒŒƒK"),
	STAGE_ENTRY("Jail", "Lounge", 4, "bk0", "Guest", "0", 0, "Rest Area", "‹xŒeŠ"),
	STAGE_ENTRY("Store", "CentW", 4, "bk0", "Guest", "Cent", 0, "Teleporter", "“]‘—Žº"),
	STAGE_ENTRY("Store", "Jail2", 4, "bk0", "Guest", "Cent", 0, "Jail No. 2", "‘æ‚Q˜S"),
	STAGE_ENTRY("White", "Blcny1", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "ƒoƒ‹ƒRƒj["),
	STAGE_ENTRY("Jail", "Priso1", 4, "bkGray", "Red", "0", 0, "Final Cave", "ÅŒã‚Ì“´ŒA"),
	STAGE_ENTRY("White", "Ring1", 7, "bkFog", "Guest", "Miza", 0, "Throne Room", "‰¤‚Ì‹ÊÀ"),
	STAGE_ENTRY("White", "Ring2", 7, "bkFog", "Guest", "Dr", 0, "The King's Table", "‰¤‚ÌH‘ì"),
	STAGE_ENTRY("Pens", "Prefa1", 4, "bk0", "0", "0", 0, "Prefab Building", "ƒvƒŒƒnƒu"),
	STAGE_ENTRY("Jail", "Priso2", 4, "bkGray", "Red", "0", 0, "Last Cave (Hidden)", "ÅŒã‚Ì“´ŒAE— "),
	STAGE_ENTRY("White", "Ring3", 4, "bk0", "Miza", "Almo2", 7, "Black Space", "•‚¢LŠÔ"),
	STAGE_ENTRY("Pens", "Little", 2, "bkBlue", "Guest", "0", 0, "Little House", "ƒŠƒgƒ‹‰Æ"),
	STAGE_ENTRY("White", "Blcny2", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "ƒoƒ‹ƒRƒj["),
	STAGE_ENTRY("Fall", "Fall", 1, "bkFall", "Guest", "Heri", 0, "Fall", "—Ž‰º"),
	STAGE_ENTRY("White", "Kings", 4, "bk0", "Kings", "0", 0, "u", "u"),
	STAGE_ENTRY("Pens", "Pixel", 1, "bkBlue", "Guest", "0", 0, "Waterway Cabin", "…˜H‚Ì¬•”‰®"),
	STAGE_ENTRY("Maze", "e_Maze", 1, "bkMaze", "Guest", "Maze", 3, "", ""),
	STAGE_ENTRY("Barr", "e_Jenk", 4, "bk0", "Sand", "Bllg", 0, "", ""),
	STAGE_ENTRY("Barr", "e_Malc", 1, "bkBlue", "Weed", "Bllg", 0, "", ""),
	STAGE_ENTRY("Mimi", "e_Ceme", 4, "bk0", "Plant", "0", 0, "", ""),
	STAGE_ENTRY("Fall", "e_Sky", 1, "bkFall", "Guest", "Heri", 0, "", ""),
	STAGE_ENTRY("Pens", "Prefa2", 4, "bk0", "0", "0", 0, "Prefab House", "ƒvƒŒƒnƒu"),
	STAGE_ENTRY("Hell", "Hell1", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B1", "¹ˆæ’n‰º‚PŠK"),
	STAGE_ENTRY("Hell", "Hell2", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B2", "¹ˆæ’n‰º‚QŠK"),
	STAGE_ENTRY("Hell", "Hell3", 1, "bkRed", "Hell", "Press", 8, "Sacred Ground - B3", "¹ˆæ’n‰º‚RŠK"),
	STAGE_ENTRY("Cave", "Mapi", 2, "bk0", "Cemet", "0", 0, "Storage", "•¨’u"),
	STAGE_ENTRY("Hell", "Hell4", 4, "bk0", "Hell", "0", 0, "Passage?", "’Ê˜HH"),
	STAGE_ENTRY("Hell", "Hell42", 4, "bk0", "Hell", "Press", 8, "Passage?", "’Ê˜HH"),
	STAGE_ENTRY("Hell", "Statue", 1, "bkBlue", "0", "Cent", 0, "Statue Chamber", "Î‘œ‚ÌŠÔ"),
	STAGE_ENTRY("Hell", "Ballo1", 2, "bkBlue", "Priest", "Ballos", 9, "Seal Chamber", "••ˆó‚ÌŠÔ"),
	STAGE_ENTRY("White", "Ostep", 7, "bkFog", "0", "0", 0, "Corridor", "‚í‚½‚è˜L‰º"),
	STAGE_ENTRY("Labo", "e_Labo", 4, "bk0", "Guest", "0", 0, "", ""),
	STAGE_ENTRY("Cave", "Pole", 4, "bk0", "Guest", "0", 0, "Hermit Gunsmith", "‚Í‚®‚êe’b–è"),
	STAGE_ENTRY("0", "Island", 4, "bk0", "Island", "0", 0, "", ""),
	STAGE_ENTRY("Hell", "Ballo2", 2, "bkBlue", "Priest", "Bllg", 9, "Seal Chamber", "••ˆó‚ÌŠÔ"),
	STAGE_ENTRY("White", "e_Blcn", 7, "bkFog", "Miza", "0", 9, "", ""),
	STAGE_ENTRY("Oside", "Clock", 6, "bkMoon", "Moon", "0", 0, "Clock Room", "ŽžŒv‰®"),
};

static const STAGE_TABLE *gTMT = gTMTDefault;

bool LoadStageTable(const char *path)
{
	bool success = false;

	unsigned char *file_buffer;
	const long file_size = LoadFileToMemory(path, &file_buffer);

	if (file_size != -1 && file_size >= 4)
	{
		const long entry_count = file_buffer[0] | (file_buffer[1] << 8) | (file_buffer[2] << 16) | (file_buffer[3] << 24);

		STAGE_TABLE *pTMT = (STAGE_TABLE*)malloc(entry_count * sizeof(STAGE_TABLE));

		if (pTMT != NULL)
		{
			for (long i = 0; i < entry_count; ++i)
			{
				// For compatibility with Booster's Lab, we store our stage table in "MOD_KS" format.
				// This way, BL will load the sprites as PNG files instead of BMP.
				unsigned char *entry = file_buffer + 4 + i * 0x74;

				memcpy(pTMT[i].parts, entry, 0x10);
				memcpy(pTMT[i].map, entry + 0x10, 0x10);
				pTMT[i].bkType = entry[0x20];
				memcpy(pTMT[i].back, entry + 0x21, 0x10);
				memcpy(pTMT[i].npc, entry + 0x31, 0x10);
				memcpy(pTMT[i].boss, entry + 0x41, 0x10);
				pTMT[i].boss_no = entry[0x51];
				memcpy(pTMT[i].name, entry + 0x52, 0x22);
			}

			gTMT = pTMT;
			success = true;
		}

		free(file_buffer);
	}

	if (success == false)
		printf("Failed to load mrmap.bin\n");

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
