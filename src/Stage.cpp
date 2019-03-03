#include "Stage.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Back.h"
#include "Boss.h"
#include "Bullet.h"
#include "Caret.h"
#include "CommonDefines.h"
#include "Draw.h"
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

const STAGE_TABLE gTMT[95] = {
	STAGE_ENTRY("0", "0", 4, "bk0", "Guest", "0", 0, "Null", "無"),
	STAGE_ENTRY("Pens", "Pens1", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "アーサーの家"),
	STAGE_ENTRY("Eggs", "Eggs", 1, "bkGreen", "Eggs1", "Ravil", 0, "Egg Corridor", "タマゴ回廊"),
	STAGE_ENTRY("EggX", "EggX", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "タマゴ No.00"),
	STAGE_ENTRY("EggIn", "Egg6", 4, "bk0", "Eggs1", "0", 0, "Egg No. 06", "タマゴ No.06"),
	STAGE_ENTRY("Store", "EggR", 4, "bk0", "Eggs1", "0", 0, "Egg Observation Room", "タマゴ監視室"),
	STAGE_ENTRY("Weed", "Weed", 1, "bkBlue", "Weed", "0", 0, "Grasstown", "クサムラ"),
	STAGE_ENTRY("Barr", "Santa", 4, "bk0", "Weed", "0", 0, "Santa's House", "サンタの家"),
	STAGE_ENTRY("Barr", "Chako", 1, "bkBlue", "Guest", "0", 0, "Chaco's House", "チャコの家"),
	STAGE_ENTRY("Maze", "MazeI", 4, "bk0", "Maze", "0", 0, "Labyrinth I", "迷宮Ｉ"),
	STAGE_ENTRY("Sand", "Sand", 1, "bkGreen", "Sand", "Omg", 1, "Sand Zone", "砂区"),
	STAGE_ENTRY("Mimi", "Mimi", 1, "bkBlue", "Guest", "0", 0, "Mimiga Village", "ミミガーの村"),
	STAGE_ENTRY("Cave", "Cave", 4, "bk0", "Cemet", "0", 0, "First Cave", "最初の洞窟"),
	STAGE_ENTRY("Cave", "Start", 4, "bk0", "Cemet", "0", 0, "Start Point", "スタート地点"),
	STAGE_ENTRY("Mimi", "Barr", 4, "bk0", "Cemet", "Bllg", 0, "Shack", "バラック小屋"),
	STAGE_ENTRY("Mimi", "Pool", 1, "bkBlue", "Guest", "0", 0, "Reservoir", "貯水池"),
	STAGE_ENTRY("Mimi", "Cemet", 4, "bk0", "Cemet", "0", 0, "Graveyard", "はかば"),
	STAGE_ENTRY("Mimi", "Plant", 1, "bkGreen", "Plant", "0", 0, "Yamashita Farm", "山下農園"),
	STAGE_ENTRY("Store", "Shelt", 4, "bk0", "Eggs1", "0", 0, "Shelter", "シェルター"),
	STAGE_ENTRY("Pens", "Comu", 1, "bkBlue", "Guest", "0", 0, "Assembly Hall", "集会場"),
	STAGE_ENTRY("Mimi", "MiBox", 4, "bk0", "0", "0", 0, "Save Point", "セーブポイント"),
	STAGE_ENTRY("Store", "EgEnd1", 4, "bk0", "0", "0", 0, "Side Room", "タマゴ回廊の個室"),
	STAGE_ENTRY("Store", "Cthu", 4, "bk0", "0", "0", 0, "Cthulhu's Abode", "クトゥルーの住処"),
	STAGE_ENTRY("EggIn", "Egg1", 4, "bk0", "Eggs1", "0", 0, "Egg No. 01", "タマゴ No.01"),
	STAGE_ENTRY("Pens", "Pens2", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "アーサーの家"),
	STAGE_ENTRY("Barr", "Malco", 1, "bkBlue", "Weed", "Bllg", 0, "Power Room", "電源室"),
	STAGE_ENTRY("Barr", "WeedS", 1, "bkBlue", "0", "0", 0, "Save Point", "セーブポイント"),
	STAGE_ENTRY("Store", "WeedD", 1, "bkBlue", "0", "0", 0, "Execution Chamber", "処刑室"),
	STAGE_ENTRY("Weed", "Frog", 2, "bkGreen", "Weed", "Frog", 2, "Gum", "ガム"),
	STAGE_ENTRY("Sand", "Curly", 4, "bk0", "Sand", "Curly", 0, "Sand Zone Residence", "砂区駐在所"),
	STAGE_ENTRY("Pens", "WeedB", 1, "bkBlue", "Ravil", "0", 0, "Grasstown Hut", "クサムラの小屋"),
	STAGE_ENTRY("River", "Stream", 5, "bkBlue", "Stream", "IronH", 5, "Main Artery", "大動脈"),
	STAGE_ENTRY("Pens", "CurlyS", 4, "bk0", "Sand", "Curly", 0, "Small Room", "小部屋"),
	STAGE_ENTRY("Barr", "Jenka1", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "ジェンカの家"),
	STAGE_ENTRY("Sand", "Dark", 1, "bkBlack", "Sand", "0", 0, "Deserted House", "廃屋"),
	STAGE_ENTRY("Gard", "Gard", 1, "bkGard", "Toro", "Bllg", 0, "Sand Zone Storehouse", "砂区倉庫"),
	STAGE_ENTRY("Barr", "Jenka2", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "ジェンカの家"),
	STAGE_ENTRY("Sand", "SandE", 1, "bkGreen", "Sand", "Bllg", 0, "Sand Zone", "砂区"),
	STAGE_ENTRY("Maze", "MazeH", 4, "bk0", "Maze", "0", 0, "Labyrinth H", "迷宮Ｈ"),
	STAGE_ENTRY("Maze", "MazeW", 1, "bkMaze", "Maze", "X", 3, "Labyrinth W", "迷宮Ｗ"),
	STAGE_ENTRY("Maze", "MazeO", 4, "bk0", "Guest", "0", 0, "Camp", "キャンプ"),
	STAGE_ENTRY("Maze", "MazeD", 4, "bk0", "Guest", "Dark", 0, "Clinic Ruins", "診療所跡"),
	STAGE_ENTRY("Store", "MazeA", 4, "bk0", "Maze", "0", 0, "Labyrinth Shop", "迷宮の店"),
	STAGE_ENTRY("Maze", "MazeB", 1, "bkBlue", "Maze", "0", 0, "Labyrinth B", "迷宮Ｂ"),
	STAGE_ENTRY("Maze", "MazeS", 2, "bkGray", "Maze", "Bllg", 0, "Boulder Chamber", "大石の塞ぐ所"),
	STAGE_ENTRY("Maze", "MazeM", 1, "bkRed", "Maze", "0", 0, "Labyrinth M", "迷宮Ｍ"),
	STAGE_ENTRY("Cave", "Drain", 3, "bkWater", "Cemet", "0", 0, "Dark Place", "暗い所"),
	STAGE_ENTRY("Almond", "Almond", 3, "bkWater", "Cemet", "Almo1", 4, "Core", "コア"),
	STAGE_ENTRY("River", "River", 2, "bkGreen", "Weed", "0", 0, "Waterway", "水路"),
	STAGE_ENTRY("Eggs", "Eggs2", 1, "bkGreen", "Eggs2", "0", 0, "Egg Corridor?", "タマゴ回廊？"),
	STAGE_ENTRY("Store", "Cthu2", 4, "bk0", "Eggs1", "0", 0, "Cthulhu's Abode?", "クトゥルーの住処？"),
	STAGE_ENTRY("Store", "EggR2", 4, "bk0", "Eggs1", "TwinD", 6, "Egg Observation Room?", "タマゴ監視室？"),
	STAGE_ENTRY("EggX", "EggX2", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "タマゴ No.00"),
	STAGE_ENTRY("Oside", "Oside", 6, "bkMoon", "Moon", "0", 0, "Outer Wall", "外壁"),
	STAGE_ENTRY("Store", "EgEnd2", 4, "bk0", "Eggs1", "0", 0, "Side Room", "タマゴ回廊の個室"),
	STAGE_ENTRY("Store", "Itoh", 2, "bkBlue", "Guest", "0", 0, "Storehouse", "倉庫"),
	STAGE_ENTRY("Cent", "Cent", 1, "bkGreen", "Guest", "Cent", 0, "Plantation", "大農園"),
	STAGE_ENTRY("Jail", "Jail1", 4, "bk0", "Guest", "Cent", 0, "Jail No. 1", "第１牢"),
	STAGE_ENTRY("Jail", "Momo", 4, "bk0", "Guest", "0", 0, "Hideout", "カクレガ"),
	STAGE_ENTRY("Jail", "Lounge", 4, "bk0", "Guest", "0", 0, "Rest Area", "休憩所"),
	STAGE_ENTRY("Store", "CentW", 4, "bk0", "Guest", "Cent", 0, "Teleporter", "転送室"),
	STAGE_ENTRY("Store", "Jail2", 4, "bk0", "Guest", "Cent", 0, "Jail No. 2", "第２牢"),
	STAGE_ENTRY("White", "Blcny1", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "バルコニー"),
	STAGE_ENTRY("Jail", "Priso1", 4, "bkGray", "Red", "0", 0, "Final Cave", "最後の洞窟"),
	STAGE_ENTRY("White", "Ring1", 7, "bkFog", "Guest", "Miza", 0, "Throne Room", "王の玉座"),
	STAGE_ENTRY("White", "Ring2", 7, "bkFog", "Guest", "Dr", 0, "The King's Table", "王の食卓"),
	STAGE_ENTRY("Pens", "Prefa1", 4, "bk0", "0", "0", 0, "Prefab Building", "プレハブ"),
	STAGE_ENTRY("Jail", "Priso2", 4, "bkGray", "Red", "0", 0, "Last Cave (Hidden)", "最後の洞窟・裏"),
	STAGE_ENTRY("White", "Ring3", 4, "bk0", "Miza", "Almo2", 7, "Black Space", "黒い広間"),
	STAGE_ENTRY("Pens", "Little", 2, "bkBlue", "Guest", "0", 0, "Little House", "リトル家"),
	STAGE_ENTRY("White", "Blcny2", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "バルコニー"),
	STAGE_ENTRY("Fall", "Fall", 1, "bkFall", "Guest", "Heri", 0, "Fall", "落下"),
	STAGE_ENTRY("White", "Kings", 4, "bk0", "Kings", "0", 0, "u", "u"),
	STAGE_ENTRY("Pens", "Pixel", 1, "bkBlue", "Guest", "0", 0, "Waterway Cabin", "水路の小部屋"),
	STAGE_ENTRY("Maze", "e_Maze", 1, "bkMaze", "Guest", "Maze", 3, "", ""),
	STAGE_ENTRY("Barr", "e_Jenk", 4, "bk0", "Sand", "Bllg", 0, "", ""),
	STAGE_ENTRY("Barr", "e_Malc", 1, "bkBlue", "Weed", "Bllg", 0, "", ""),
	STAGE_ENTRY("Mimi", "e_Ceme", 4, "bk0", "Plant", "0", 0, "", ""),
	STAGE_ENTRY("Fall", "e_Sky", 1, "bkFall", "Guest", "Heri", 0, "", ""),
	STAGE_ENTRY("Pens", "Prefa2", 4, "bk0", "0", "0", 0, "Prefab House", "プレハブ"),
	STAGE_ENTRY("Hell", "Hell1", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B1", "聖域地下１階"),
	STAGE_ENTRY("Hell", "Hell2", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B2", "聖域地下２階"),
	STAGE_ENTRY("Hell", "Hell3", 1, "bkRed", "Hell", "Press", 8, "Sacred Ground - B3", "聖域地下３階"),
	STAGE_ENTRY("Cave", "Mapi", 2, "bk0", "Cemet", "0", 0, "Storage", "物置"),
	STAGE_ENTRY("Hell", "Hell4", 4, "bk0", "Hell", "0", 0, "Passage?", "通路？"),
	STAGE_ENTRY("Hell", "Hell42", 4, "bk0", "Hell", "Press", 8, "Passage?", "通路？"),
	STAGE_ENTRY("Hell", "Statue", 1, "bkBlue", "0", "Cent", 0, "Statue Chamber", "石像の間"),
	STAGE_ENTRY("Hell", "Ballo1", 2, "bkBlue", "Priest", "Ballos", 9, "Seal Chamber", "封印の間"),
	STAGE_ENTRY("White", "Ostep", 7, "bkFog", "0", "0", 0, "Corridor", "わたり廊下"),
	STAGE_ENTRY("Labo", "e_Labo", 4, "bk0", "Guest", "0", 0, "", ""),
	STAGE_ENTRY("Cave", "Pole", 4, "bk0", "Guest", "0", 0, "Hermit Gunsmith", "はぐれ銃鍛冶"),
	STAGE_ENTRY("0", "Island", 4, "bk0", "Island", "0", 0, "", ""),
	STAGE_ENTRY("Hell", "Ballo2", 2, "bkBlue", "Priest", "Bllg", 9, "Seal Chamber", "封印の間"),
	STAGE_ENTRY("White", "e_Blcn", 7, "bkFog", "Miza", "0", 9, "", ""),
	STAGE_ENTRY("Oside", "Clock", 6, "bkMoon", "Moon", "0", 0, "Clock Room", "時計屋"),
};

BOOL TransferStage(int no, int w, int x, int y)
{
	FILE *errorLog = fopen("sd:/cse2/error.log", "wb");
	char errorMsg[0x400];
	
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
	{
		sprintf(errorMsg, "Couldn't open tileset %s\n", gTMT[no].parts);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}

	sprintf(path, "%s/%s.pxa", path_dir, gTMT[no].parts);
	if (!LoadAttributeData(path))
	{
		sprintf(errorMsg, "Couldn't open tileset attributes %s\n", gTMT[no].parts);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}
	
	//Load tilemap
	sprintf(path, "%s/%s.pxm", path_dir, gTMT[no].map);
	if (!LoadMapData2(path))
	{
		sprintf(errorMsg, "Couldn't open map data %s\n", gTMT[no].map);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}

	//Load NPCs
	sprintf(path, "%s/%s.pxe", path_dir, gTMT[no].map);
	if (!LoadEvent(path))
	{
		sprintf(errorMsg, "Couldn't open events %s\n", gTMT[no].map);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}

	//Load script
	sprintf(path, "%s/%s.tsc", path_dir, gTMT[no].map);
	if (!LoadTextScript_Stage(path))
	{
		sprintf(errorMsg, "Couldn't open text script %s\n", gTMT[no].map);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}
	
	//Load background
	strcpy(path, gTMT[no].back);
	if (!InitBack(path, gTMT[no].bkType))
	{
		sprintf(errorMsg, "Couldn't open background %s\n", gTMT[no].back);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}
	
	//Get path
	strcpy(path_dir, "Npc");
	
	//Load NPC sprite sheets
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].npc);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_1))
	{
		sprintf(errorMsg, "Couldn't open npc sprite sheet %s\n", gTMT[no].npc);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}
	
	sprintf(path, "%s/Npc%s", path_dir, gTMT[no].boss);
	if (!ReloadBitmap_File(path, SURFACE_ID_LEVEL_SPRITESET_2))
	{
		sprintf(errorMsg, "Couldn't open boss sprite sheet %s\n", gTMT[no].npc);
		fwrite(errorMsg, strlen(errorMsg), 1, errorLog);
		bError = TRUE;
	}
	
	fclose(errorLog);
	
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
