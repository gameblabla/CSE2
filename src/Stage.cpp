#include "Stage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "WindowsWrapper.h"

#include "Back.h"
#include "Boss.h"
#include "Bullet.h"
#include "Caret.h"
#include "Draw.h"
#include "File.h"
#include "Flash.h"
#include "Frame.h"
#include "Generic.h"
#include "Main.h"
#include "Map.h"
#include "MapName.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Organya.h"
#ifdef EXTRA_SOUND_FORMATS
#include "ExtraSoundFormats.h"
#endif
#include "TextScr.h"
#include "ValueView.h"

#ifdef JAPANESE
#define STAGE_ENTRY(parts, map, bkType, back, npc, boss, boss_no, name_en, name_jp) {parts, map, bkType, back, npc, boss, boss_no, name_jp}
#else
#define STAGE_ENTRY(parts, map, bkType, back, npc, boss, boss_no, name_en, name_jp) {parts, map, bkType, back, npc, boss, boss_no, name_en}
#endif

int gStageNo;
MusicID gMusicNo;
unsigned int gOldPos;
MusicID gOldNo;
int gSoundtrack;

const STAGE_TABLE gTMTDefault[95] = {
	STAGE_ENTRY("0", "0", 4, "bk0", "Guest", "0", 0, "Null", "\x96\xB3"),	/* 無 */
	STAGE_ENTRY("Pens", "Pens1", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "\x83\x41\x81\x5B\x83\x54\x81\x5B\x82\xCC\x89\xC6"),	/* アーサーの家 */
	STAGE_ENTRY("Eggs", "Eggs", 1, "bkGreen", "Eggs1", "Ravil", 0, "Egg Corridor", "\x83\x5E\x83\x7D\x83\x53\x89\xF1\x98\x4C"),	/* タマゴ回廊 */
	STAGE_ENTRY("EggX", "EggX", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "\x83\x5E\x83\x7D\x83\x53\x20\x4E\x6F\x2E\x30\x30"),	/* タマゴ No.00 */
	STAGE_ENTRY("EggIn", "Egg6", 4, "bk0", "Eggs1", "0", 0, "Egg No. 06", "\x83\x5E\x83\x7D\x83\x53\x20\x4E\x6F\x2E\x30\x36"),	/* タマゴ No.06 */
	STAGE_ENTRY("Store", "EggR", 4, "bk0", "Eggs1", "0", 0, "Egg Observation Room", "\x83\x5E\x83\x7D\x83\x53\x8A\xC4\x8E\x8B\x8E\xBA"),	/* タマゴ監視室 */
	STAGE_ENTRY("Weed", "Weed", 1, "bkBlue", "Weed", "0", 0, "Grasstown", "\x83\x4E\x83\x54\x83\x80\x83\x89"),	/* クサムラ */
	STAGE_ENTRY("Barr", "Santa", 4, "bk0", "Weed", "0", 0, "Santa's House", "\x83\x54\x83\x93\x83\x5E\x82\xCC\x89\xC6"),	/* サンタの家 */
	STAGE_ENTRY("Barr", "Chako", 1, "bkBlue", "Guest", "0", 0, "Chaco's House", "\x83\x60\x83\x83\x83\x52\x82\xCC\x89\xC6"),	/* チャコの家 */
	STAGE_ENTRY("Maze", "MazeI", 4, "bk0", "Maze", "0", 0, "Labyrinth I", "\x96\xC0\x8B\x7B\x82\x68"),	/* 迷宮Ｉ */
	STAGE_ENTRY("Sand", "Sand", 1, "bkGreen", "Sand", "Omg", 1, "Sand Zone", "\x8D\xBB\x8B\xE6"),	/* 砂区 */
	STAGE_ENTRY("Mimi", "Mimi", 1, "bkBlue", "Guest", "0", 0, "Mimiga Village", "\x83\x7E\x83\x7E\x83\x4B\x81\x5B\x82\xCC\x91\xBA"),	/* ミミガーの村 */
	STAGE_ENTRY("Cave", "Cave", 4, "bk0", "Cemet", "0", 0, "First Cave", "\x8D\xC5\x8F\x89\x82\xCC\x93\xB4\x8C\x41"),	/* 最初の洞窟 */
	STAGE_ENTRY("Cave", "Start", 4, "bk0", "Cemet", "0", 0, "Start Point", "\x83\x58\x83\x5E\x81\x5B\x83\x67\x92\x6E\x93\x5F"),	/* スタート地点 */
	STAGE_ENTRY("Mimi", "Barr", 4, "bk0", "Cemet", "Bllg", 0, "Shack", "\x83\x6F\x83\x89\x83\x62\x83\x4E\x8F\xAC\x89\xAE"),	/* バラック小屋 */
	STAGE_ENTRY("Mimi", "Pool", 1, "bkBlue", "Guest", "0", 0, "Reservoir", "\x92\x99\x90\x85\x92\x72"),	/* 貯水池 */
	STAGE_ENTRY("Mimi", "Cemet", 4, "bk0", "Cemet", "0", 0, "Graveyard", "\x82\xCD\x82\xA9\x82\xCE"),	/* はかば */
	STAGE_ENTRY("Mimi", "Plant", 1, "bkGreen", "Plant", "0", 0, "Yamashita Farm", "\x8E\x52\x89\xBA\x94\x5F\x89\x80"),	/* 山下農園 */
	STAGE_ENTRY("Store", "Shelt", 4, "bk0", "Eggs1", "0", 0, "Shelter", "\x83\x56\x83\x46\x83\x8B\x83\x5E\x81\x5B"),	/* シェルター */
	STAGE_ENTRY("Pens", "Comu", 1, "bkBlue", "Guest", "0", 0, "Assembly Hall", "\x8F\x57\x89\xEF\x8F\xEA"),	/* 集会場 */
	STAGE_ENTRY("Mimi", "MiBox", 4, "bk0", "0", "0", 0, "Save Point", "\x83\x5A\x81\x5B\x83\x75\x83\x7C\x83\x43\x83\x93\x83\x67"),	/* セーブポイント */
	STAGE_ENTRY("Store", "EgEnd1", 4, "bk0", "0", "0", 0, "Side Room", "\x83\x5E\x83\x7D\x83\x53\x89\xF1\x98\x4C\x82\xCC\x8C\xC2\x8E\xBA"),	/* タマゴ回廊の個室 */
	STAGE_ENTRY("Store", "Cthu", 4, "bk0", "0", "0", 0, "Cthulhu's Abode", "\x83\x4E\x83\x67\x83\x44\x83\x8B\x81\x5B\x82\xCC\x8F\x5A\x8F\x88"),	/* クトゥルーの住処 */
	STAGE_ENTRY("EggIn", "Egg1", 4, "bk0", "Eggs1", "0", 0, "Egg No. 01", "\x83\x5E\x83\x7D\x83\x53\x20\x4E\x6F\x2E\x30\x31"),	/* タマゴ No.01 */
	STAGE_ENTRY("Pens", "Pens2", 1, "bkBlue", "Guest", "0", 0, "Arthur's House", "\x83\x41\x81\x5B\x83\x54\x81\x5B\x82\xCC\x89\xC6"),	/* アーサーの家 */
	STAGE_ENTRY("Barr", "Malco", 1, "bkBlue", "Weed", "Bllg", 0, "Power Room", "\x93\x64\x8C\xB9\x8E\xBA"),	/* 電源室 */
	STAGE_ENTRY("Barr", "WeedS", 1, "bkBlue", "0", "0", 0, "Save Point", "\x83\x5A\x81\x5B\x83\x75\x83\x7C\x83\x43\x83\x93\x83\x67"),	/* セーブポイント */
	STAGE_ENTRY("Store", "WeedD", 1, "bkBlue", "0", "0", 0, "Execution Chamber", "\x8F\x88\x8C\x59\x8E\xBA"),	/* 処刑室 */
	STAGE_ENTRY("Weed", "Frog", 2, "bkGreen", "Weed", "Frog", 2, "Gum", "\x83\x4B\x83\x80"),	/* ガム */
	STAGE_ENTRY("Sand", "Curly", 4, "bk0", "Sand", "Curly", 0, "Sand Zone Residence", "\x8D\xBB\x8B\xE6\x92\x93\x8D\xDD\x8F\x8A"),	/* 砂区駐在所 */
	STAGE_ENTRY("Pens", "WeedB", 1, "bkBlue", "Ravil", "0", 0, "Grasstown Hut", "\x83\x4E\x83\x54\x83\x80\x83\x89\x82\xCC\x8F\xAC\x89\xAE"),	/* クサムラの小屋 */
	STAGE_ENTRY("River", "Stream", 5, "bkBlue", "Stream", "IronH", 5, "Main Artery", "\x91\xE5\x93\xAE\x96\xAC"),	/* 大動脈 */
	STAGE_ENTRY("Pens", "CurlyS", 4, "bk0", "Sand", "Curly", 0, "Small Room", "\x8F\xAC\x95\x94\x89\xAE"),	/* 小部屋 */
	STAGE_ENTRY("Barr", "Jenka1", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "\x83\x57\x83\x46\x83\x93\x83\x4A\x82\xCC\x89\xC6"),	/* ジェンカの家 */
	STAGE_ENTRY("Sand", "Dark", 1, "bkBlack", "Sand", "0", 0, "Deserted House", "\x94\x70\x89\xAE"),	/* 廃屋 */
	STAGE_ENTRY("Gard", "Gard", 1, "bkGard", "Toro", "Bllg", 0, "Sand Zone Storehouse", "\x8D\xBB\x8B\xE6\x91\x71\x8C\xC9"),	/* 砂区倉庫 */
	STAGE_ENTRY("Barr", "Jenka2", 4, "bk0", "Sand", "Bllg", 0, "Jenka's House", "\x83\x57\x83\x46\x83\x93\x83\x4A\x82\xCC\x89\xC6"),	/* ジェンカの家 */
	STAGE_ENTRY("Sand", "SandE", 1, "bkGreen", "Sand", "Bllg", 0, "Sand Zone", "\x8D\xBB\x8B\xE6"),	/* 砂区 */
	STAGE_ENTRY("Maze", "MazeH", 4, "bk0", "Maze", "0", 0, "Labyrinth H", "\x96\xC0\x8B\x7B\x82\x67"),	/* 迷宮Ｈ */
	STAGE_ENTRY("Maze", "MazeW", 1, "bkMaze", "Maze", "X", 3, "Labyrinth W", "\x96\xC0\x8B\x7B\x82\x76"),	/* 迷宮Ｗ */
	STAGE_ENTRY("Maze", "MazeO", 4, "bk0", "Guest", "0", 0, "Camp", "\x83\x4C\x83\x83\x83\x93\x83\x76"),	/* キャンプ */
	STAGE_ENTRY("Maze", "MazeD", 4, "bk0", "Guest", "Dark", 0, "Clinic Ruins", "\x90\x66\x97\xC3\x8F\x8A\x90\xD5"),	/* 診療所跡 */
	STAGE_ENTRY("Store", "MazeA", 4, "bk0", "Maze", "0", 0, "Labyrinth Shop", "\x96\xC0\x8B\x7B\x82\xCC\x93\x58"),	/* 迷宮の店 */
	STAGE_ENTRY("Maze", "MazeB", 1, "bkBlue", "Maze", "0", 0, "Labyrinth B", "\x96\xC0\x8B\x7B\x82\x61"),	/* 迷宮Ｂ */
	STAGE_ENTRY("Maze", "MazeS", 2, "bkGray", "Maze", "Bllg", 0, "Boulder Chamber", "\x91\xE5\x90\xCE\x82\xCC\x8D\xC7\x82\xAE\x8F\x8A"),	/* 大石の塞ぐ所 */
	STAGE_ENTRY("Maze", "MazeM", 1, "bkRed", "Maze", "0", 0, "Labyrinth M", "\x96\xC0\x8B\x7B\x82\x6C"),	/* 迷宮Ｍ */
	STAGE_ENTRY("Cave", "Drain", 3, "bkWater", "Cemet", "0", 0, "Dark Place", "\x88\xC3\x82\xA2\x8F\x8A"),	/* 暗い所 */
	STAGE_ENTRY("Almond", "Almond", 3, "bkWater", "Cemet", "Almo1", 4, "Core", "\x83\x52\x83\x41"),	/* コア */
	STAGE_ENTRY("River", "River", 2, "bkGreen", "Weed", "0", 0, "Waterway", "\x90\x85\x98\x48"),	/* 水路 */
	STAGE_ENTRY("Eggs", "Eggs2", 1, "bkGreen", "Eggs2", "0", 0, "Egg Corridor?", "\x83\x5E\x83\x7D\x83\x53\x89\xF1\x98\x4C\x81\x48"),	/* タマゴ回廊？ */
	STAGE_ENTRY("Store", "Cthu2", 4, "bk0", "Eggs1", "0", 0, "Cthulhu's Abode?", "\x83\x4E\x83\x67\x83\x44\x83\x8B\x81\x5B\x82\xCC\x8F\x5A\x8F\x88\x81\x48"),	/* クトゥルーの住処？ */
	STAGE_ENTRY("Store", "EggR2", 4, "bk0", "Eggs1", "TwinD", 6, "Egg Observation Room?", "\x83\x5E\x83\x7D\x83\x53\x8A\xC4\x8E\x8B\x8E\xBA\x81\x48"),	/* タマゴ監視室？ */
	STAGE_ENTRY("EggX", "EggX2", 4, "bk0", "Eggs1", "0", 0, "Egg No. 00", "\x83\x5E\x83\x7D\x83\x53\x20\x4E\x6F\x2E\x30\x30"),	/* タマゴ No.00 */
	STAGE_ENTRY("Oside", "Oside", 6, "bkMoon", "Moon", "0", 0, "Outer Wall", "\x8A\x4F\x95\xC7"),	/* 外壁 */
	STAGE_ENTRY("Store", "EgEnd2", 4, "bk0", "Eggs1", "0", 0, "Side Room", "\x83\x5E\x83\x7D\x83\x53\x89\xF1\x98\x4C\x82\xCC\x8C\xC2\x8E\xBA"),	/* タマゴ回廊の個室 */
	STAGE_ENTRY("Store", "Itoh", 2, "bkBlue", "Guest", "0", 0, "Storehouse", "\x91\x71\x8C\xC9"),	/* 倉庫 */
	STAGE_ENTRY("Cent", "Cent", 1, "bkGreen", "Guest", "Cent", 0, "Plantation", "\x91\xE5\x94\x5F\x89\x80"),	/* 大農園 */
	STAGE_ENTRY("Jail", "Jail1", 4, "bk0", "Guest", "Cent", 0, "Jail No. 1", "\x91\xE6\x82\x50\x98\x53"),	/* 第１牢 */
	STAGE_ENTRY("Jail", "Momo", 4, "bk0", "Guest", "0", 0, "Hideout", "\x83\x4A\x83\x4E\x83\x8C\x83\x4B"),	/* カクレガ */
	STAGE_ENTRY("Jail", "Lounge", 4, "bk0", "Guest", "0", 0, "Rest Area", "\x8B\x78\x8C\x65\x8F\x8A"),	/* 休憩所 */
	STAGE_ENTRY("Store", "CentW", 4, "bk0", "Guest", "Cent", 0, "Teleporter", "\x93\x5D\x91\x97\x8E\xBA"),	/* 転送室 */
	STAGE_ENTRY("Store", "Jail2", 4, "bk0", "Guest", "Cent", 0, "Jail No. 2", "\x91\xE6\x82\x51\x98\x53"),	/* 第２牢 */
	STAGE_ENTRY("White", "Blcny1", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "\x83\x6F\x83\x8B\x83\x52\x83\x6A\x81\x5B"),	/* バルコニー */
	STAGE_ENTRY("Jail", "Priso1", 4, "bkGray", "Red", "0", 0, "Final Cave", "\x8D\xC5\x8C\xE3\x82\xCC\x93\xB4\x8C\x41"),	/* 最後の洞窟 */
	STAGE_ENTRY("White", "Ring1", 7, "bkFog", "Guest", "Miza", 0, "Throne Room", "\x89\xA4\x82\xCC\x8B\xCA\x8D\xC0"),	/* 王の玉座 */
	STAGE_ENTRY("White", "Ring2", 7, "bkFog", "Guest", "Dr", 0, "The King's Table", "\x89\xA4\x82\xCC\x90\x48\x91\xEC"),	/* 王の食卓 */
	STAGE_ENTRY("Pens", "Prefa1", 4, "bk0", "0", "0", 0, "Prefab Building", "\x83\x76\x83\x8C\x83\x6E\x83\x75"),	/* プレハブ */
	STAGE_ENTRY("Jail", "Priso2", 4, "bkGray", "Red", "0", 0, "Last Cave (Hidden)", "\x8D\xC5\x8C\xE3\x82\xCC\x93\xB4\x8C\x41\x81\x45\x97\xA0"),	/* 最後の洞窟・裏 */
	STAGE_ENTRY("White", "Ring3", 4, "bk0", "Miza", "Almo2", 7, "Black Space", "\x8D\x95\x82\xA2\x8D\x4C\x8A\xD4"),	/* 黒い広間 */
	STAGE_ENTRY("Pens", "Little", 2, "bkBlue", "Guest", "0", 0, "Little House", "\x83\x8A\x83\x67\x83\x8B\x89\xC6"),	/* リトル家 */
	STAGE_ENTRY("White", "Blcny2", 7, "bkFog", "Ravil", "Heri", 0, "Balcony", "\x83\x6F\x83\x8B\x83\x52\x83\x6A\x81\x5B"),	/* バルコニー */
	STAGE_ENTRY("Fall", "Fall", 1, "bkFall", "Guest", "Heri", 0, "Fall", "\x97\x8E\x89\xBA"),	/* 落下 */
	STAGE_ENTRY("White", "Kings", 4, "bk0", "Kings", "0", 0, "u", "\x75"),	/* u */
	STAGE_ENTRY("Pens", "Pixel", 1, "bkBlue", "Guest", "0", 0, "Waterway Cabin", "\x90\x85\x98\x48\x82\xCC\x8F\xAC\x95\x94\x89\xAE"),	/* 水路の小部屋 */
	STAGE_ENTRY("Maze", "e_Maze", 1, "bkMaze", "Guest", "Maze", 3, "", ""),
	STAGE_ENTRY("Barr", "e_Jenk", 4, "bk0", "Sand", "Bllg", 0, "", ""),
	STAGE_ENTRY("Barr", "e_Malc", 1, "bkBlue", "Weed", "Bllg", 0, "", ""),
	STAGE_ENTRY("Mimi", "e_Ceme", 4, "bk0", "Plant", "0", 0, "", ""),
	STAGE_ENTRY("Fall", "e_Sky", 1, "bkFall", "Guest", "Heri", 0, "", ""),
	STAGE_ENTRY("Pens", "Prefa2", 4, "bk0", "0", "0", 0, "Prefab House", "\x83\x76\x83\x8C\x83\x6E\x83\x75"),	/* プレハブ */
	STAGE_ENTRY("Hell", "Hell1", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B1", "\x90\xB9\x88\xE6\x92\x6E\x89\xBA\x82\x50\x8A\x4B"),	/* 聖域地下１階 */
	STAGE_ENTRY("Hell", "Hell2", 2, "bkRed", "Hell", "0", 0, "Sacred Ground - B2", "\x90\xB9\x88\xE6\x92\x6E\x89\xBA\x82\x51\x8A\x4B"),	/* 聖域地下２階 */
	STAGE_ENTRY("Hell", "Hell3", 1, "bkRed", "Hell", "Press", 8, "Sacred Ground - B3", "\x90\xB9\x88\xE6\x92\x6E\x89\xBA\x82\x52\x8A\x4B"),	/* 聖域地下３階 */
	STAGE_ENTRY("Cave", "Mapi", 2, "bk0", "Cemet", "0", 0, "Storage", "\x95\xA8\x92\x75"),	/* 物置 */
	STAGE_ENTRY("Hell", "Hell4", 4, "bk0", "Hell", "0", 0, "Passage?", "\x92\xCA\x98\x48\x81\x48"),	/* 通路？ */
	STAGE_ENTRY("Hell", "Hell42", 4, "bk0", "Hell", "Press", 8, "Passage?", "\x92\xCA\x98\x48\x81\x48"),	/* 通路？ */
	STAGE_ENTRY("Hell", "Statue", 1, "bkBlue", "0", "Cent", 0, "Statue Chamber", "\x90\xCE\x91\x9C\x82\xCC\x8A\xD4"),	/* 石像の間 */
	STAGE_ENTRY("Hell", "Ballo1", 2, "bkBlue", "Priest", "Ballos", 9, "Seal Chamber", "\x95\x95\x88\xF3\x82\xCC\x8A\xD4"),	/* 封印の間 */
	STAGE_ENTRY("White", "Ostep", 7, "bkFog", "0", "0", 0, "Corridor", "\x82\xED\x82\xBD\x82\xE8\x98\x4C\x89\xBA"),	/* わたり廊下 */
	STAGE_ENTRY("Labo", "e_Labo", 4, "bk0", "Guest", "0", 0, "", ""),
	STAGE_ENTRY("Cave", "Pole", 4, "bk0", "Guest", "0", 0, "Hermit Gunsmith", "\x82\xCD\x82\xAE\x82\xEA\x8F\x65\x92\x62\x96\xE8"),	/* はぐれ銃鍛冶 */
	STAGE_ENTRY("0", "Island", 4, "bk0", "Island", "0", 0, "", ""),
	STAGE_ENTRY("Hell", "Ballo2", 2, "bkBlue", "Priest", "Bllg", 9, "Seal Chamber", "\x95\x95\x88\xF3\x82\xCC\x8A\xD4"),	/* 封印の間 */
	STAGE_ENTRY("White", "e_Blcn", 7, "bkFog", "Miza", "0", 9, "", ""),
	STAGE_ENTRY("Oside", "Clock", 6, "bkMoon", "Moon", "0", 0, "Clock Room", "\x8E\x9E\x8C\x76\x89\xAE"),	/* 時計屋 */
};

const STAGE_TABLE *gTMT = gTMTDefault;

BOOL LoadStageTable(void)
{
	std::string path;

	unsigned char *file_buffer;
	size_t file_size;

	// Try to load stage.tbl
	path = gDataPath + "/stage.tbl";
	file_buffer = LoadFileToMemory(path.c_str(), &file_size);

	if (file_buffer != NULL)
	{
		const unsigned long entry_count = file_size / 0xE5;

		STAGE_TABLE *pTMT = (STAGE_TABLE*)malloc(entry_count * sizeof(STAGE_TABLE));

		if (pTMT != NULL)
		{
			for (unsigned long i = 0; i < entry_count; ++i)
			{
				unsigned char *entry = file_buffer + i * 0xE5;

				memcpy(pTMT[i].parts, entry, 0x20);
				memcpy(pTMT[i].map, entry + 0x20, 0x20);
				pTMT[i].bkType = (entry[0x40 + 3] << 24) | (entry[0x40 + 2] << 16) | (entry[0x40 + 1] << 8) | entry[0x40];
				memcpy(pTMT[i].back, entry + 0x44, 0x20);
				memcpy(pTMT[i].npc, entry + 0x64, 0x20);
				memcpy(pTMT[i].boss, entry + 0x84, 0x20);
				pTMT[i].boss_no = entry[0xA4];
#ifdef JAPANESE
				memcpy(pTMT[i].name, entry + 0xA5, 0x20);
#else
				memcpy(pTMT[i].name, entry + 0xC5, 0x20);
#endif
			}

			gTMT = pTMT;
			free(file_buffer);
			return TRUE;
		}

		free(file_buffer);
	}

	// Try to load mrmap.bin
	path = gDataPath + "/mrmap.bin";
	file_buffer = LoadFileToMemory(path.c_str(), &file_size);

	if (file_buffer != NULL)
	{
		const unsigned long entry_count = file_buffer[0] | (file_buffer[1] << 8) | (file_buffer[2] << 16) | (file_buffer[3] << 24);

		STAGE_TABLE *pTMT = (STAGE_TABLE*)malloc(entry_count * sizeof(STAGE_TABLE));

		if (pTMT != NULL)
		{
			for (unsigned long i = 0; i < entry_count; ++i)
			{
				// For compatibility with Booster's Lab, we store our stage table in "MOD_MR" format.
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
			free(file_buffer);
			return TRUE;
		}

		free(file_buffer);
	}

	printf("Failed to load stage.tbl/mrmap.bin\n");
	return FALSE;
}

BOOL TransferStage(int no, int w, int x, int y)
{
	std::string path;
	std::string path_dir;
	BOOL bError;

	// Move character
	SetMyCharPosition(x * 0x10 * 0x200, y * 0x10 * 0x200);

	bError = FALSE;

	// Get path
	path_dir = "Stage";

	// Load tileset
	path = path_dir + "/Prt" + gTMT[no].parts;
	if (!ReloadBitmap_File(path.c_str(), SURFACE_ID_LEVEL_TILESET))
		bError = TRUE;

	path = path_dir + '/' + gTMT[no].parts + ".pxa";
	if (!LoadAttributeData(path.c_str()))
		bError = TRUE;

	// Load tilemap
	path = path_dir + '/' + gTMT[no].map + ".pxm";
	if (!LoadMapData2(path.c_str()))
		bError = TRUE;

	// Load NPCs
	path = path_dir + '/' + gTMT[no].map + ".pxe";
	if (!LoadEvent(path.c_str()))
		bError = TRUE;

	// Load script
	path = path_dir + '/' + gTMT[no].map + ".tsc";
	if (!LoadTextScript_Stage(path.c_str()))
		bError = TRUE;

	// Load background
	path = gTMT[no].back;
	if (!InitBack(path.c_str(), gTMT[no].bkType))
		bError = TRUE;

	// Get path
	path_dir = "Npc";

	// Load NPC sprite sheets
	path = path_dir + "/Npc" + gTMT[no].npc;
	if (!ReloadBitmap_File(path.c_str(), SURFACE_ID_LEVEL_SPRITESET_1))
		bError = TRUE;

	path = path_dir + "/Npc" + gTMT[no].boss;
	if (!ReloadBitmap_File(path.c_str(), SURFACE_ID_LEVEL_SPRITESET_2))
		bError = TRUE;

	if (bError)
		return FALSE;

	// Load map name
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

//Music

enum
{
	MUSIC_TYPE_ORGANYA,
	MUSIC_TYPE_OTHER
};

typedef struct MusicListEntry
{
	const char *intro_file_path;
	const char *loop_file_path;
	int type;
	bool loop;	// Only applicable to non-Organya songs
} MusicListEntry;

static const MusicListEntry music_table_organya[42] = {
	{"Resource/ORG/XXXX.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Wanpaku.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Anzen.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Gameover.org", NULL, MUSIC_TYPE_ORGANYA, false},
	{"Resource/ORG/Gravity.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Weed.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/MDown2.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/FireEye.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Vivi.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Mura.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Fanfale1.org", NULL, MUSIC_TYPE_ORGANYA, false},
	{"Resource/ORG/Ginsuke.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Cemetery.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Plant.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Kodou.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Fanfale3.org", NULL, MUSIC_TYPE_ORGANYA, false},
	{"Resource/ORG/Fanfale2.org", NULL, MUSIC_TYPE_ORGANYA, false},
	{"Resource/ORG/Dr.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Escape.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Jenka.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Maze.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Access.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/ironH.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Grand.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Curly.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Oside.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Requiem.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Wanpak2.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/quiet.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/LastCave.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Balcony.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/LastBtl.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/LastBtl3.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Ending.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Zonbie.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/BreakDown.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Hell.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Jenka2.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Marine.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Ballos.org", NULL, MUSIC_TYPE_ORGANYA, true},
	{"Resource/ORG/Toroko.org", NULL, MUSIC_TYPE_ORGANYA, false},
	{"Resource/ORG/White.org", NULL, MUSIC_TYPE_ORGANYA, true}
};

static const MusicListEntry music_table_new[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/wanpaku.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/anzen.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/gameover.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/New/gravity.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/weed.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/mdown2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/fireeye.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/vivi.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/mura.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/fanfale1.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/New/ginsuke.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/cemetery.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/plant.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/kodou.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/fanfale3.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/New/fanfale2.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/New/dr.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/escape.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/jenka.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/maze.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/access.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/ironh.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/grand.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/curly.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/oside.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/requiem.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/wanpak2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/quiet.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/lastcave.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/balcony.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/lastbt3.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/ending.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/zonbie.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/bdown.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/hell.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/jenka2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/marine.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/ballos.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/New/toroko.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/New/white.ogg", NULL, MUSIC_TYPE_OTHER, true}
};

static const MusicListEntry music_table_remastered[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/wanpaku_intro.ogg", "Soundtracks/Remastered/wanpaku_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/anzen_intro.ogg", "Soundtracks/Remastered/anzen_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/gameover_intro.ogg", "Soundtracks/Remastered/gameover_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Remastered/gravity_intro.ogg", "Soundtracks/Remastered/gravity_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/weed_intro.ogg", "Soundtracks/Remastered/weed_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/mdown2_intro.ogg", "Soundtracks/Remastered/mdown2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/fireeye_intro.ogg", "Soundtracks/Remastered/fireeye_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/vivi_intro.ogg", "Soundtracks/Remastered/vivi_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/mura_intro.ogg", "Soundtracks/Remastered/mura_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/fanfale1_intro.ogg", "Soundtracks/Remastered/fanfale1_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Remastered/ginsuke_intro.ogg", "Soundtracks/Remastered/ginsuke_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/cemetery_intro.ogg", "Soundtracks/Remastered/cemetery_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/plant_intro.ogg", "Soundtracks/Remastered/plant_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/kodou_intro.ogg", "Soundtracks/Remastered/kodou_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/fanfale3_intro.ogg", "Soundtracks/Remastered/fanfale3_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Remastered/fanfale2_intro.ogg", "Soundtracks/Remastered/fanfale2_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Remastered/dr_intro.ogg", "Soundtracks/Remastered/dr_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/escape_intro.ogg", "Soundtracks/Remastered/escape_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/jenka_intro.ogg", "Soundtracks/Remastered/jenka_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/maze_intro.ogg", "Soundtracks/Remastered/maze_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/access_intro.ogg", "Soundtracks/Remastered/access_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/ironh_intro.ogg", "Soundtracks/Remastered/ironh_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/grand_intro.ogg", "Soundtracks/Remastered/grand_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/curly_intro.ogg", "Soundtracks/Remastered/curly_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/oside_intro.ogg", "Soundtracks/Remastered/oside_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/requiem_intro.ogg", "Soundtracks/Remastered/requiem_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/wanpak2_intro.ogg", "Soundtracks/Remastered/wanpak2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/quiet_intro.ogg", "Soundtracks/Remastered/quiet_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/lastcave_intro.ogg", "Soundtracks/Remastered/lastcave_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/balcony_intro.ogg", "Soundtracks/Remastered/balcony_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/lastbt3_intro.ogg", "Soundtracks/Remastered/lastbt3_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/ending_intro.ogg", "Soundtracks/Remastered/ending_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/zonbie_intro.ogg", "Soundtracks/Remastered/zonbie_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/bdown_intro.ogg", "Soundtracks/Remastered/bdown_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/hell_intro.ogg", "Soundtracks/Remastered/hell_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/jenka2_intro.ogg", "Soundtracks/Remastered/jenka2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/marine_intro.ogg", "Soundtracks/Remastered/marine_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/ballos_intro.ogg", "Soundtracks/Remastered/ballos_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Remastered/toroko_intro.ogg", "Soundtracks/Remastered/toroko_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Remastered/white_intro.ogg", "Soundtracks/Remastered/white_loop.ogg", MUSIC_TYPE_OTHER, true}
};

static const MusicListEntry music_table_famitracks[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/wanpaku_intro.ogg", "Soundtracks/Famitracks/wanpaku_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/anzen_intro.ogg", "Soundtracks/Famitracks/anzen_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/gameover.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Famitracks/gravity_intro.ogg", "Soundtracks/Famitracks/gravity_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/weed_intro.ogg", "Soundtracks/Famitracks/weed_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/mdown2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/fireeye_intro.ogg", "Soundtracks/Famitracks/fireeye_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/vivi_intro.ogg", "Soundtracks/Famitracks/vivi_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/mura.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/fanfale1.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Famitracks/ginsuke.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/cemetery_intro.ogg", "Soundtracks/Famitracks/cemetery_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/plant_intro.ogg", "Soundtracks/Famitracks/plant_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/kodou_intro.ogg", "Soundtracks/Famitracks/kodou_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/fanfale3.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Famitracks/fanfale2.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Famitracks/dr.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/escape.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/jenka_intro.ogg", "Soundtracks/Famitracks/jenka_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/maze.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/access.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/ironh_intro.ogg", "Soundtracks/Famitracks/ironh_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/grand_intro.ogg", "Soundtracks/Famitracks/grand_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/curly_intro.ogg", "Soundtracks/Famitracks/curly_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/oside_intro.ogg", "Soundtracks/Famitracks/oside_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/requiem_intro.ogg", "Soundtracks/Famitracks/requiem_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/wanpak2_intro.ogg", "Soundtracks/Famitracks/wanpak2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/quiet_intro.ogg", "Soundtracks/Famitracks/quiet_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/lastcave_intro.ogg", "Soundtracks/Famitracks/lastcave_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/balcony_intro.ogg", "Soundtracks/Famitracks/balcony_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/lastbt3_intro.ogg", "Soundtracks/Famitracks/lastbt3_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/ending_intro.ogg", "Soundtracks/Famitracks/ending_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/zonbie_intro.ogg", "Soundtracks/Famitracks/zonbie_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/bdown_intro.ogg", "Soundtracks/Famitracks/bdown_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/hell_intro.ogg", "Soundtracks/Famitracks/hell_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/jenka2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/marine.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/ballos_intro.ogg", "Soundtracks/Famitracks/ballos_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Famitracks/toroko.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Famitracks/white_intro.ogg", "Soundtracks/Famitracks/white_loop.ogg", MUSIC_TYPE_OTHER, true}
};

static const MusicListEntry music_table_ridiculon[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/wanpaku.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/anzen.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/gameover.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Ridiculon/gravity.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/weed.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/mdown2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/fireeye.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/vivi.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/mura.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/fanfale1.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Ridiculon/ginsuke.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/cemetery.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/plant.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/kodou.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/fanfale3.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Ridiculon/fanfale2.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Ridiculon/dr.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/escape.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/jenka.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/maze.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/access.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/ironh.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/grand.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/curly.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/oside.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/requiem.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/wanpak2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/quiet.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/lastcave.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/balcony.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/lastbt3.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/ending.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/zonbie.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/bdown.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/hell.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/jenka2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/marine.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/ballos.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Ridiculon/toroko.ogg", NULL, MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Ridiculon/white.ogg", NULL, MUSIC_TYPE_OTHER, true}
};

static const MusicListEntry music_table_snes[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/wanpaku_intro.ogg", "Soundtracks/SNES/wanpaku_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/anzen_intro.ogg", "Soundtracks/SNES/anzen_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/gameover_intro.ogg", "Soundtracks/SNES/gameover_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/SNES/gravity_intro.ogg", "Soundtracks/SNES/gravity_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/weed_intro.ogg", "Soundtracks/SNES/weed_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/mdown2_intro.ogg", "Soundtracks/SNES/mdown2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/fireeye_intro.ogg", "Soundtracks/SNES/fireeye_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/vivi_intro.ogg", "Soundtracks/SNES/vivi_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/mura_intro.ogg", "Soundtracks/SNES/mura_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/fanfale1_intro.ogg", "Soundtracks/SNES/fanfale1_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/SNES/ginsuke_intro.ogg", "Soundtracks/SNES/ginsuke_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/cemetery_intro.ogg", "Soundtracks/SNES/cemetery_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/plant_intro.ogg", "Soundtracks/SNES/plant_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/kodou_intro.ogg", "Soundtracks/SNES/kodou_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/fanfale3_intro.ogg", "Soundtracks/SNES/fanfale3_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/SNES/fanfale2_intro.ogg", "Soundtracks/SNES/fanfale2_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/SNES/dr_intro.ogg", "Soundtracks/SNES/dr_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/escape_intro.ogg", "Soundtracks/SNES/escape_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/jenka_intro.ogg", "Soundtracks/SNES/jenka_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/maze_intro.ogg", "Soundtracks/SNES/maze_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/access_intro.ogg", "Soundtracks/SNES/access_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/ironh_intro.ogg", "Soundtracks/SNES/ironh_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/grand_intro.ogg", "Soundtracks/SNES/grand_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/curly_intro.ogg", "Soundtracks/SNES/curly_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/oside_intro.ogg", "Soundtracks/SNES/oside_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/requiem_intro.ogg", "Soundtracks/SNES/requiem_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/wanpak2_intro.ogg", "Soundtracks/SNES/wanpak2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/quiet_intro.ogg", "Soundtracks/SNES/quiet_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/lastcave_intro.ogg", "Soundtracks/SNES/lastcave_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/balcony_intro.ogg", "Soundtracks/SNES/balcony_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/lastbt3_intro.ogg", "Soundtracks/SNES/lastbt3_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/ending_intro.ogg", "Soundtracks/SNES/ending_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/zonbie_intro.ogg", "Soundtracks/SNES/zonbie_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/bdown_intro.ogg", "Soundtracks/SNES/bdown_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/hell_intro.ogg", "Soundtracks/SNES/hell_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/jenka2_intro.ogg", "Soundtracks/SNES/jenka2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/marine_intro.ogg", "Soundtracks/SNES/marine_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/ballos_intro.ogg", "Soundtracks/SNES/ballos_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/SNES/toroko_intro.ogg", "Soundtracks/SNES/toroko_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/SNES/white_intro.ogg", "Soundtracks/SNES/white_loop.ogg", MUSIC_TYPE_OTHER, true}
};

static const MusicListEntry music_table_arranged[42] = {
	{NULL, NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/wanpaku_intro.ogg", "Soundtracks/Arranged/wanpaku_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/anzen_intro.ogg", "Soundtracks/Arranged/anzen_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/gameover_intro.ogg", "Soundtracks/Arranged/gameover_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Arranged/gravity_intro.ogg", "Soundtracks/Arranged/gravity_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/weed_intro.ogg", "Soundtracks/Arranged/weed_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/mdown2.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/fireeye_intro.ogg", "Soundtracks/Arranged/fireeye_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/vivi_intro.ogg", "Soundtracks/Arranged/vivi_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/mura_intro.ogg", "Soundtracks/Arranged/mura_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/fanfale1_intro.ogg", "Soundtracks/Arranged/fanfale1_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Arranged/ginsuke_intro.ogg", "Soundtracks/Arranged/ginsuke_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/cemetery_intro.ogg", "Soundtracks/Arranged/cemetery_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/plant_intro.ogg", "Soundtracks/Arranged/plant_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/kodou_intro.ogg", "Soundtracks/Arranged/kodou_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/fanfale3_intro.ogg", "Soundtracks/Arranged/fanfale3_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Arranged/fanfale2_intro.ogg", "Soundtracks/Arranged/fanfale2_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Arranged/dr_intro.ogg", "Soundtracks/Arranged/dr_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/escape_intro.ogg", "Soundtracks/Arranged/escape_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/jenka_intro.ogg", "Soundtracks/Arranged/jenka_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/maze_intro.ogg", "Soundtracks/Arranged/maze_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/access_intro.ogg", "Soundtracks/Arranged/access_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/ironh_intro.ogg", "Soundtracks/Arranged/ironh_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/grand_intro.ogg", "Soundtracks/Arranged/grand_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/curly_intro.ogg", "Soundtracks/Arranged/curly_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/oside_intro.ogg", "Soundtracks/Arranged/oside_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/requiem_intro.ogg", "Soundtracks/Arranged/requiem_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/wanpak2_intro.ogg", "Soundtracks/Arranged/wanpak2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/quiet_intro.ogg", "Soundtracks/Arranged/quiet_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/lastcave_intro.ogg", "Soundtracks/Arranged/lastcave_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/balcony_intro.ogg", "Soundtracks/Arranged/balcony_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/lastbtl.ogg", NULL, MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/lastbt3_intro.ogg", "Soundtracks/Arranged/lastbt3_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/ending_intro.ogg", "Soundtracks/Arranged/ending_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/zonbie_intro.ogg", "Soundtracks/Arranged/zonbie_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/bdown_intro.ogg", "Soundtracks/Arranged/bdown_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/hell_intro.ogg", "Soundtracks/Arranged/hell_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/jenka2_intro.ogg", "Soundtracks/Arranged/jenka2_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/marine_intro.ogg", "Soundtracks/Arranged/marine_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/ballos_intro.ogg", "Soundtracks/Arranged/ballos_loop.ogg", MUSIC_TYPE_OTHER, true},
	{"Soundtracks/Arranged/toroko_intro.ogg", "Soundtracks/Arranged/toroko_loop.ogg", MUSIC_TYPE_OTHER, false},
	{"Soundtracks/Arranged/white_intro.ogg", "Soundtracks/Arranged/white_loop.ogg", MUSIC_TYPE_OTHER, true}
};

static const struct
{
	const MusicListEntry *music_table;
	unsigned short volume;	// Logarithmic - 0 is silent, 0x80 is half-volume, 0x100 is full-volume (this only affects non-Organya music)
} soundtracks[7] = {
	{music_table_organya, 0x100},
	{music_table_new, 0xC0},
	{music_table_remastered, 0xC0},
	{music_table_famitracks, 0xE0},
	{music_table_ridiculon, 0xE0},
	{music_table_snes, 0x100},
	{music_table_arranged, 0x140}
};

void ChangeMusic(MusicID no)
{
	if (no != MUS_SILENCE && no == gMusicNo)
		return;

	// Stop and keep track of old song
	gOldPos = GetOrganyaPosition();
	gOldNo = gMusicNo;
	StopOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_PauseMusic();
#endif

	const MusicListEntry *music_table = soundtracks[gSoundtrack].music_table;

	std::string intro_file_path;
	if (music_table[no].intro_file_path != NULL)
		intro_file_path = gDataPath + '/' + music_table[no].intro_file_path;

	std::string loop_file_path;
	if (music_table[no].loop_file_path != NULL)
		loop_file_path = gDataPath + '/' + music_table[no].loop_file_path;

	switch (music_table[no].type)
	{
		case MUSIC_TYPE_ORGANYA:
			// Load .org
			LoadOrganya(intro_file_path.c_str());

			// Reset position, volume, and then play the song
			ChangeOrganyaVolume(100);
			SetOrganyaPosition(0);
			PlayOrganyaMusic();

#ifdef EXTRA_SOUND_FORMATS
			ExtraSound_LoadMusic(NULL, NULL, false);	// Play a null song so any current song gets pushed back to the backup slot
#endif
			break;

#ifdef EXTRA_SOUND_FORMATS
		case MUSIC_TYPE_OTHER:
			ExtraSound_LoadMusic(music_table[no].intro_file_path != NULL ? intro_file_path.c_str() : NULL, music_table[no].loop_file_path != NULL ? loop_file_path.c_str() : NULL, music_table[no].loop);
			ExtraSound_SetMusicVolume(soundtracks[gSoundtrack].volume);
			ExtraSound_UnpauseMusic();

			// Play a null Organya song so focussing and refocussing the window doesn't cause the old Organya song to start playing again
			intro_file_path = gDataPath + '/' + music_table_organya[0].intro_file_path;
			LoadOrganya(intro_file_path.c_str());

			ChangeOrganyaVolume(100);
			SetOrganyaPosition(0);
			PlayOrganyaMusic();
			break;
#endif
	}

	gMusicNo = no;
}

void ReCallMusic(void)
{
	std::string path;

	// Stop old song
	StopOrganyaMusic();
#ifdef EXTRA_SOUND_FORMATS
	ExtraSound_PauseMusic();
#endif

	const MusicListEntry *music_table = soundtracks[gSoundtrack].music_table;

	switch (music_table[gOldNo].type)
	{
		case MUSIC_TYPE_ORGANYA:
			// Load .org that was playing before
			path = gDataPath + '/' + music_table[gOldNo].intro_file_path;
			LoadOrganya(path.c_str());

			// Reset position, volume, and then play the song
			SetOrganyaPosition(gOldPos);
			ChangeOrganyaVolume(100);
			PlayOrganyaMusic();
			break;

#ifdef EXTRA_SOUND_FORMATS
		case MUSIC_TYPE_OTHER:
			ExtraSound_LoadPreviousMusic();
			ExtraSound_UnpauseMusic();
			break;
#endif
	}

	gMusicNo = gOldNo;
}

BOOL CheckSoundtrackExists(int soundtrack)
{
	if (soundtrack >= sizeof(soundtracks) / sizeof(soundtracks[0]))
		return FALSE;

	// Just check if the first file exists
	std::string path = gDataPath + '/' + soundtracks[soundtrack].music_table[1].intro_file_path;

	FILE *file = fopen(path.c_str(), "rb");

	if (file == NULL)
		return FALSE;

	fclose(file);
	return TRUE;
}
