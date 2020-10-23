// Released under the MIT licence.
// See LICENCE.txt for details.

#include "Resource.h"

#include <stddef.h>
#include <string.h>

static const unsigned char rCredit01[] = {
	#include "Resource/BITMAP/Credit01.bmp.h"
};

static const unsigned char rCredit02[] = {
	#include "Resource/BITMAP/Credit02.bmp.h"
};

static const unsigned char rCredit03[] = {
	#include "Resource/BITMAP/Credit03.bmp.h"
};

static const unsigned char rCredit04[] = {
	#include "Resource/BITMAP/Credit04.bmp.h"
};

static const unsigned char rCredit05[] = {
	#include "Resource/BITMAP/Credit05.bmp.h"
};

static const unsigned char rCredit06[] = {
	#include "Resource/BITMAP/Credit06.bmp.h"
};

static const unsigned char rCredit07[] = {
	#include "Resource/BITMAP/Credit07.bmp.h"
};

static const unsigned char rCredit08[] = {
	#include "Resource/BITMAP/Credit08.bmp.h"
};

static const unsigned char rCredit09[] = {
	#include "Resource/BITMAP/Credit09.bmp.h"
};

static const unsigned char rCredit10[] = {
	#include "Resource/BITMAP/Credit10.bmp.h"
};

static const unsigned char rCredit11[] = {
	#include "Resource/BITMAP/Credit11.bmp.h"
};

static const unsigned char rCredit12[] = {
	#include "Resource/BITMAP/Credit12.bmp.h"
};

static const unsigned char rCredit14[] = {
	#include "Resource/BITMAP/Credit14.bmp.h"
};

static const unsigned char rCredit15[] = {
	#include "Resource/BITMAP/Credit15.bmp.h"
};

static const unsigned char rCredit16[] = {
	#include "Resource/BITMAP/Credit16.bmp.h"
};

static const unsigned char rCredit17[] = {
	#include "Resource/BITMAP/Credit17.bmp.h"
};

static const unsigned char rCredit18[] = {
	#include "Resource/BITMAP/Credit18.bmp.h"
};

static const unsigned char rpixel[] = {
#ifdef JAPANESE
	#include "Resource/BITMAP/pixel_jp.bmp.h"
#else
	#include "Resource/BITMAP/pixel.bmp.h"
#endif
};

static const unsigned char rCURSOR_IKA[] = {
	#include "Resource/CURSOR/CURSOR_IKA.png.h"
};

static const unsigned char rCURSOR_NORMAL[] = {
	#include "Resource/CURSOR/CURSOR_NORMAL.png.h"
};

#ifndef _WIN32
static const unsigned char rICON_MINI[] = {
	#include "Resource/ICON/ICON_MINI.png.h"
};
#endif

static const unsigned char rAccess[] = {
	#include "Resource/ORG/Access.org.h"
};

static const unsigned char rAnzen[] = {
	#include "Resource/ORG/Anzen.org.h"
};

static const unsigned char rBalcony[] = {
	#include "Resource/ORG/Balcony.org.h"
};

static const unsigned char rBallos[] = {
	#include "Resource/ORG/Ballos.org.h"
};

static const unsigned char rBreakDown[] = {
	#include "Resource/ORG/BreakDown.org.h"
};

static const unsigned char rCemetery[] = {
	#include "Resource/ORG/Cemetery.org.h"
};

static const unsigned char rCurly[] = {
	#include "Resource/ORG/Curly.org.h"
};

static const unsigned char rDr[] = {
	#include "Resource/ORG/Dr.org.h"
};

static const unsigned char rEnding[] = {
	#include "Resource/ORG/Ending.org.h"
};

static const unsigned char rEscape[] = {
	#include "Resource/ORG/Escape.org.h"
};

static const unsigned char rFanfale1[] = {
	#include "Resource/ORG/Fanfale1.org.h"
};

static const unsigned char rFanfale2[] = {
	#include "Resource/ORG/Fanfale2.org.h"
};

static const unsigned char rFanfale3[] = {
	#include "Resource/ORG/Fanfale3.org.h"
};

static const unsigned char rFireEye[] = {
	#include "Resource/ORG/FireEye.org.h"
};

static const unsigned char rGameover[] = {
	#include "Resource/ORG/Gameover.org.h"
};

static const unsigned char rGinsuke[] = {
	#include "Resource/ORG/Ginsuke.org.h"
};

static const unsigned char rGrand[] = {
	#include "Resource/ORG/Grand.org.h"
};

static const unsigned char rGravity[] = {
	#include "Resource/ORG/Gravity.org.h"
};

static const unsigned char rHell[] = {
	#include "Resource/ORG/Hell.org.h"
};

static const unsigned char rironH[] = {
	#include "Resource/ORG/ironH.org.h"
};

static const unsigned char rJenka[] = {
	#include "Resource/ORG/Jenka.org.h"
};

static const unsigned char rJenka2[] = {
	#include "Resource/ORG/Jenka2.org.h"
};

static const unsigned char rKodou[] = {
	#include "Resource/ORG/Kodou.org.h"
};

static const unsigned char rLastBtl3[] = {
	#include "Resource/ORG/LastBtl3.org.h"
};

static const unsigned char rLastBtl[] = {
	#include "Resource/ORG/LastBtl.org.h"
};

static const unsigned char rLastCave[] = {
	#include "Resource/ORG/LastCave.org.h"
};

static const unsigned char rMarine[] = {
	#include "Resource/ORG/Marine.org.h"
};

static const unsigned char rMaze[] = {
	#include "Resource/ORG/Maze.org.h"
};

static const unsigned char rMDown2[] = {
	#include "Resource/ORG/MDown2.org.h"
};

static const unsigned char rMura[] = {
	#include "Resource/ORG/Mura.org.h"
};

static const unsigned char rOside[] = {
	#include "Resource/ORG/Oside.org.h"
};

static const unsigned char rPlant[] = {
	#include "Resource/ORG/Plant.org.h"
};

static const unsigned char rquiet[] = {
	#include "Resource/ORG/quiet.org.h"
};

static const unsigned char rRequiem[] = {
	#include "Resource/ORG/Requiem.org.h"
};

static const unsigned char rToroko[] = {
	#include "Resource/ORG/Toroko.org.h"
};

static const unsigned char rVivi[] = {
	#include "Resource/ORG/Vivi.org.h"
};

static const unsigned char rWanpak2[] = {
	#include "Resource/ORG/Wanpak2.org.h"
};

static const unsigned char rWanpaku[] = {
	#include "Resource/ORG/Wanpaku.org.h"
};

static const unsigned char rWeed[] = {
	#include "Resource/ORG/Weed.org.h"
};

static const unsigned char rWhite[] = {
	#include "Resource/ORG/White.org.h"
};

static const unsigned char rXXXX[] = {
	#include "Resource/ORG/XXXX.org.h"
};

static const unsigned char rZonbie[] = {
	#include "Resource/ORG/Zonbie.org.h"
};

static const unsigned char rWave[] = {
	#include "Resource/WAVE/Wave.dat.h"
};

static const struct
{
	const char *type;
	const char *name;
	const unsigned char *data;
	size_t size;
} resources[] = {
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
	{"BITMAP", "PIXEL", rpixel, sizeof(rpixel)},
	{"CURSOR", "CURSOR_IKA", rCURSOR_IKA, sizeof(rCURSOR_IKA)},
	{"CURSOR", "CURSOR_NORMAL", rCURSOR_NORMAL, sizeof(rCURSOR_NORMAL)},
#ifndef _WIN32
	{"ICON", "ICON_MINI", rICON_MINI, sizeof(rICON_MINI)},
#endif
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
	{"DUMMY", "DUMMY", NULL, 0} // Just here to prevent errors in the event that the array is otherwise empty
};

const unsigned char* FindResource(const char *name, const char *type, size_t *size)
{
	for (size_t i = 0; i < sizeof(resources) / sizeof(resources[0]); ++i)
	{
		if (!strcmp(name, resources[i].name) && !strcmp(type, resources[i].type))
		{
			if (size != NULL)
				*size = resources[i].size;

			return resources[i].data;
		}
	}

	return NULL;
}
