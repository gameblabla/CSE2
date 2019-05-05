#include "Resource.h"

#include <stddef.h>
#include <string.h>

#include "Resource/ORG/Access.org.h"
#include "Resource/ORG/Anzen.org.h"
#include "Resource/ORG/Balcony.org.h"
#include "Resource/ORG/Ballos.org.h"
#include "Resource/ORG/BreakDown.org.h"
#include "Resource/ORG/Cemetery.org.h"
#include "Resource/ORG/Curly.org.h"
#include "Resource/ORG/Dr.org.h"
#include "Resource/ORG/Ending.org.h"
#include "Resource/ORG/Escape.org.h"
#include "Resource/ORG/Fanfale1.org.h"
#include "Resource/ORG/Fanfale2.org.h"
#include "Resource/ORG/Fanfale3.org.h"
#include "Resource/ORG/FireEye.org.h"
#include "Resource/ORG/Gameover.org.h"
#include "Resource/ORG/Ginsuke.org.h"
#include "Resource/ORG/Grand.org.h"
#include "Resource/ORG/Gravity.org.h"
#include "Resource/ORG/Hell.org.h"
#include "Resource/ORG/ironH.org.h"
#include "Resource/ORG/Jenka.org.h"
#include "Resource/ORG/Jenka2.org.h"
#include "Resource/ORG/Kodou.org.h"
#include "Resource/ORG/LastBtl3.org.h"
#include "Resource/ORG/LastBtl.org.h"
#include "Resource/ORG/LastCave.org.h"
#include "Resource/ORG/Marine.org.h"
#include "Resource/ORG/Maze.org.h"
#include "Resource/ORG/MDown2.org.h"
#include "Resource/ORG/Mura.org.h"
#include "Resource/ORG/Oside.org.h"
#include "Resource/ORG/Plant.org.h"
#include "Resource/ORG/quiet.org.h"
#include "Resource/ORG/Requiem.org.h"
#include "Resource/ORG/Toroko.org.h"
#include "Resource/ORG/Vivi.org.h"
#include "Resource/ORG/Wanpak2.org.h"
#include "Resource/ORG/Wanpaku.org.h"
#include "Resource/ORG/Weed.org.h"
#include "Resource/ORG/White.org.h"
#include "Resource/ORG/XXXX.org.h"
#include "Resource/ORG/Zonbie.org.h"
#include "Resource/WAVE/Wave.dat.h"
#include "Resource/BITMAP/Credit01.bmp.h"
#include "Resource/BITMAP/Credit02.bmp.h"
#include "Resource/BITMAP/Credit03.bmp.h"
#include "Resource/BITMAP/Credit04.bmp.h"
#include "Resource/BITMAP/Credit05.bmp.h"
#include "Resource/BITMAP/Credit06.bmp.h"
#include "Resource/BITMAP/Credit07.bmp.h"
#include "Resource/BITMAP/Credit08.bmp.h"
#include "Resource/BITMAP/Credit09.bmp.h"
#include "Resource/BITMAP/Credit10.bmp.h"
#include "Resource/BITMAP/Credit11.bmp.h"
#include "Resource/BITMAP/Credit12.bmp.h"
#include "Resource/BITMAP/Credit14.bmp.h"
#include "Resource/BITMAP/Credit15.bmp.h"
#include "Resource/BITMAP/Credit16.bmp.h"
#include "Resource/BITMAP/Credit17.bmp.h"
#include "Resource/BITMAP/Credit18.bmp.h"
#ifdef JAPANESE
#include "Resource/BITMAP/pixel_jp.bmp.h"
#else
#include "Resource/BITMAP/pixel.bmp.h"
#endif
#ifndef WINDOWS
#include "Resource/ICON/ICON_MINI.bmp.h"
#endif
#include "Resource/CURSOR/CURSOR_IKA.bmp.h"
#include "Resource/CURSOR/CURSOR_NORMAL.bmp.h"

#ifdef JAPANESE
#include "Resource/FONT/msgothic.ttc.h"
#else
#ifndef WINDOWS
#include "Resource/FONT/cour.ttf.h"
#endif
#endif

static const struct
{
	const char *type;
	const char *name;
	const unsigned char *data;
	size_t size;
} resources[] = {
	{"ORG", "ACCESS", rAccess, sizeof(rAccess)},
	{"ORG", "ANZEN", rAnzen, sizeof(rAnzen)},
	{"ORG", "BALCONY", rBalcony, sizeof(rBalcony)},
	{"ORG", "BALLOS", rBallos, sizeof(rBallos)},
	{"ORG", "BDOWN", rBreakDown, sizeof(rBreakDown)},
	{"ORG", "CEMETERY", rCemetery, sizeof(rCemetery)},
	{"ORG", "CURLY", rCurly, sizeof(rCurly)},
	{"ORG", "DR", rDr, sizeof(rDr)},
	{"ORG", "ENDING", rEnding, sizeof(rEnding)},
	{"ORG", "ESCAPE", rEscape, sizeof(rEscape)},
	{"ORG", "FANFALE1", rFanfale1, sizeof(rFanfale1)},
	{"ORG", "FANFALE2", rFanfale2, sizeof(rFanfale2)},
	{"ORG", "FANFALE3", rFanfale3, sizeof(rFanfale3)},
	{"ORG", "FIREEYE", rFireEye, sizeof(rFireEye)},
	{"ORG", "GAMEOVER", rGameover, sizeof(rGameover)},
	{"ORG", "GINSUKE", rGinsuke, sizeof(rGinsuke)},
	{"ORG", "GRAND", rGrand, sizeof(rGrand)},
	{"ORG", "GRAVITY", rGravity, sizeof(rGravity)},
	{"ORG", "HELL", rHell, sizeof(rHell)},
	{"ORG", "IRONH", rironH, sizeof(rironH)},
	{"ORG", "JENKA", rJenka, sizeof(rJenka)},
	{"ORG", "JENKA2", rJenka2, sizeof(rJenka2)},
	{"ORG", "KODOU", rKodou, sizeof(rKodou)},
	{"ORG", "LASTBT3", rLastBtl3, sizeof(rLastBtl3)},
	{"ORG", "LASTBTL", rLastBtl, sizeof(rLastBtl)},
	{"ORG", "LASTCAVE", rLastCave, sizeof(rLastCave)},
	{"ORG", "MARINE", rMarine, sizeof(rMarine)},
	{"ORG", "MAZE", rMaze, sizeof(rMaze)},
	{"ORG", "MDOWN2", rMDown2, sizeof(rMDown2)},
	{"ORG", "MURA", rMura, sizeof(rMura)},
	{"ORG", "OSIDE", rOside, sizeof(rOside)},
	{"ORG", "PLANT", rPlant, sizeof(rPlant)},
	{"ORG", "QUIET", rquiet, sizeof(rquiet)},
	{"ORG", "REQUIEM", rRequiem, sizeof(rRequiem)},
	{"ORG", "TOROKO", rToroko, sizeof(rToroko)},
	{"ORG", "VIVI", rVivi, sizeof(rVivi)},
	{"ORG", "WANPAK2", rWanpak2, sizeof(rWanpak2)},
	{"ORG", "WANPAKU", rWanpaku, sizeof(rWanpaku)},
	{"ORG", "WEED", rWeed, sizeof(rWeed)},
	{"ORG", "WHITE", rWhite, sizeof(rWhite)},
	{"ORG", "XXXX", rXXXX, sizeof(rXXXX)},
	{"ORG", "ZONBIE", rZonbie, sizeof(rZonbie)},

	{"WAVE", "WAVE100", rWave, sizeof(rWave)},

	{"BITMAP", "CREDIT01", rCredit01, sizeof(rCredit01)},
	{"BITMAP", "CREDIT02", rCredit02, sizeof(rCredit02)},
	{"BITMAP", "CREDIT03", rCredit03, sizeof(rCredit03)},
	{"BITMAP", "CREDIT04", rCredit04, sizeof(rCredit04)},
	{"BITMAP", "CREDIT05", rCredit05, sizeof(rCredit05)},
	{"BITMAP", "CREDIT06", rCredit06, sizeof(rCredit06)},
	{"BITMAP", "CREDIT07", rCredit07, sizeof(rCredit07)},
	{"BITMAP", "CREDIT08", rCredit08, sizeof(rCredit08)},
	{"BITMAP", "CREDIT09", rCredit09, sizeof(rCredit09)},
	{"BITMAP", "CREDIT10", rCredit10, sizeof(rCredit10)},
	{"BITMAP", "CREDIT11", rCredit11, sizeof(rCredit11)},
	{"BITMAP", "CREDIT12", rCredit12, sizeof(rCredit12)},
	{"BITMAP", "CREDIT14", rCredit14, sizeof(rCredit14)},
	{"BITMAP", "CREDIT15", rCredit15, sizeof(rCredit15)},
	{"BITMAP", "CREDIT16", rCredit16, sizeof(rCredit16)},
	{"BITMAP", "CREDIT17", rCredit17, sizeof(rCredit17)},
	{"BITMAP", "CREDIT18", rCredit18, sizeof(rCredit18)},
#ifdef JAPANESE
	{"BITMAP", "PIXEL", rpixel_jp, sizeof(rpixel_jp)},
#else
	{"BITMAP", "PIXEL", rpixel, sizeof(rpixel)},
#endif

#ifndef WINDOWS
	{"ICON", "ICON_MINI", rICON_MINI, sizeof(rICON_MINI)},
#endif

	{"CURSOR", "CURSOR_NORMAL", rCURSOR_NORMAL, sizeof(rCURSOR_NORMAL)},
	{"CURSOR", "CURSOR_IKA", rCURSOR_IKA, sizeof(rCURSOR_IKA)},

#ifdef JAPANESE
	{"FONT", "DEFAULT_FONT", rmsgothic, sizeof(rmsgothic)},
#else
#ifndef WINDOWS
	{"FONT", "DEFAULT_FONT", rcour, sizeof(rcour)},
#endif
#endif
};

const unsigned char* FindResource(const char *name, const char *type, size_t *size)
{
	for (unsigned int i = 0; i < sizeof(resources) / sizeof(resources[0]); ++i)
	{
		if (!strcmp(name, resources[i].name) && !strcmp(type, resources[i].type))
		{
			if (size)
				*size = resources[i].size;

			return resources[i].data;
		}
	}

	return NULL;
}
