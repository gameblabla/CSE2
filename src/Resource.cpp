#include "Resource.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SDL_rwops.h>

#include "WindowsWrapper.h"

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
#include "Resource/WAVE/Wave.h"
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

const unsigned char* GetResource(const char *name, size_t *size)
{
	//ORG
	if (!strcmp(name, "ACCESS"))
	{
		*size = sizeof(rAccess);
		return rAccess;
	}
	if (!strcmp(name, "ANZEN"))
	{
		*size = sizeof(rAnzen);
		return rAnzen;
	}
	if (!strcmp(name, "BALCONY"))
	{
		*size = sizeof(rBalcony);
		return rBalcony;
	}
	if (!strcmp(name, "BALLOS"))
	{
		*size = sizeof(rBallos);
		return rBallos;
	}
	if (!strcmp(name, "BDOWN"))
	{
		*size = sizeof(rBreakDown);
		return rBreakDown;
	}
	if (!strcmp(name, "CEMETERY"))
	{
		*size = sizeof(rCemetery);
		return rCemetery;
	}
	if (!strcmp(name, "CURLY"))
	{
		*size = sizeof(rCurly);
		return rCurly;
	}
	if (!strcmp(name, "DR"))
	{
		*size = sizeof(rDr);
		return rDr;
	}
	if (!strcmp(name, "ENDING"))
	{
		*size = sizeof(rEnding);
		return rEnding;
	}
	if (!strcmp(name, "ESCAPE"))
	{
		*size = sizeof(rEscape);
		return rEscape;
	}
	if (!strcmp(name, "FANFALE1"))
	{
		*size = sizeof(rFanfale1);
		return rFanfale1;
	}
	if (!strcmp(name, "FANFALE2"))
	{
		*size = sizeof(rFanfale2);
		return rFanfale2;
	}
	if (!strcmp(name, "FANFALE3"))
	{
		*size = sizeof(rFanfale3);
		return rFanfale3;
	}
	if (!strcmp(name, "FIREEYE"))
	{
		*size = sizeof(rFireEye);
		return rFireEye;
	}
	if (!strcmp(name, "GAMEOVER"))
	{
		*size = sizeof(rGameover);
		return rGameover;
	}
	if (!strcmp(name, "GINSUKE"))
	{
		*size = sizeof(rGinsuke);
		return rGinsuke;
	}
	if (!strcmp(name, "GRAND"))
	{
		*size = sizeof(rGrand);
		return rGrand;
	}
	if (!strcmp(name, "GRAVITY"))
	{
		*size = sizeof(rGravity);
		return rGravity;
	}
	if (!strcmp(name, "HELL"))
	{
		*size = sizeof(rHell);
		return rHell;
	}
	if (!strcmp(name, "IRONH"))
	{
		*size = sizeof(rironH);
		return rironH;
	}
	if (!strcmp(name, "JENKA"))
	{
		*size = sizeof(rJenka);
		return rJenka;
	}
	if (!strcmp(name, "JENKA2"))
	{
		*size = sizeof(rJenka2);
		return rJenka2;
	}
	if (!strcmp(name, "KODOU"))
	{
		*size = sizeof(rKodou);
		return rKodou;
	}
	if (!strcmp(name, "LASTBT3"))
	{
		*size = sizeof(rLastBtl3);
		return rLastBtl3;
	}
	if (!strcmp(name, "LASTBTL"))
	{
		*size = sizeof(rLastBtl);
		return rLastBtl;
	}
	if (!strcmp(name, "LASTCAVE"))
	{
		*size = sizeof(rLastCave);
		return rLastCave;
	}
	if (!strcmp(name, "MARINE"))
	{
		*size = sizeof(rMarine);
		return rMarine;
	}
	if (!strcmp(name, "MAZE"))
	{
		*size = sizeof(rMaze);
		return rMaze;
	}
	if (!strcmp(name, "MDOWN2"))
	{
		*size = sizeof(rMDown2);
		return rMDown2;
	}
	if (!strcmp(name, "MURA"))
	{
		*size = sizeof(rMura);
		return rMura;
	}
	if (!strcmp(name, "OSIDE"))
	{
		*size = sizeof(rOside);
		return rOside;
	}
	if (!strcmp(name, "PLANT"))
	{
		*size = sizeof(rPlant);
		return rPlant;
	}
	if (!strcmp(name, "QUIET"))
	{
		*size = sizeof(rquiet);
		return rquiet;
	}
	if (!strcmp(name, "REQUIEM"))
	{
		*size = sizeof(rRequiem);
		return rRequiem;
	}
	if (!strcmp(name, "TOROKO"))
	{
		*size = sizeof(rToroko);
		return rToroko;
	}
	if (!strcmp(name, "VIVI"))
	{
		*size = sizeof(rVivi);
		return rVivi;
	}
	if (!strcmp(name, "WANPAK2"))
	{
		*size = sizeof(rWanpak2);
		return rWanpak2;
	}
	if (!strcmp(name, "WANPAKU"))
	{
		*size = sizeof(rWanpaku);
		return rWanpaku;
	}
	if (!strcmp(name, "WEED"))
	{
		*size = sizeof(rWeed);
		return rWeed;
	}
	if (!strcmp(name, "WHITE"))
	{
		*size = sizeof(rWhite);
		return rWhite;
	}
	if (!strcmp(name, "XXXX"))
	{
		*size = sizeof(rXXXX);
		return rXXXX;
	}
	if (!strcmp(name, "ZONBIE"))
	{
		*size = sizeof(rZonbie);
		return rZonbie;
	}
	
	//WAVE
	if (!strcmp(name, "WAVE100"))
	{
		*size = sizeof(rWave);
		return rWave;
	}
	
	//Bitmap
	if (!strcmp(name, "CREDIT01"))
	{
		*size = sizeof(rCredit01);
		return rCredit01;
	}
	if (!strcmp(name, "CREDIT02"))
	{
		*size = sizeof(rCredit02);
		return rCredit02;
	}
	if (!strcmp(name, "CREDIT03"))
	{
		*size = sizeof(rCredit03);
		return rCredit03;
	}
	if (!strcmp(name, "CREDIT04"))
	{
		*size = sizeof(rCredit04);
		return rCredit04;
	}
	if (!strcmp(name, "CREDIT05"))
	{
		*size = sizeof(rCredit05);
		return rCredit05;
	}
	if (!strcmp(name, "CREDIT06"))
	{
		*size = sizeof(rCredit06);
		return rCredit06;
	}
	if (!strcmp(name, "CREDIT07"))
	{
		*size = sizeof(rCredit07);
		return rCredit07;
	}
	if (!strcmp(name, "CREDIT08"))
	{
		*size = sizeof(rCredit08);
		return rCredit08;
	}
	if (!strcmp(name, "CREDIT09"))
	{
		*size = sizeof(rCredit09);
		return rCredit09;
	}
	if (!strcmp(name, "CREDIT10"))
	{
		*size = sizeof(rCredit10);
		return rCredit10;
	}
	if (!strcmp(name, "CREDIT11"))
	{
		*size = sizeof(rCredit11);
		return rCredit11;
	}
	if (!strcmp(name, "CREDIT12"))
	{
		*size = sizeof(rCredit12);
		return rCredit12;
	}
	if (!strcmp(name, "CREDIT14"))
	{
		*size = sizeof(rCredit14);
		return rCredit14;
	}
	if (!strcmp(name, "CREDIT15"))
	{
		*size = sizeof(rCredit15);
		return rCredit15;
	}
	if (!strcmp(name, "CREDIT16"))
	{
		*size = sizeof(rCredit16);
		return rCredit16;
	}
	if (!strcmp(name, "CREDIT17"))
	{
		*size = sizeof(rCredit17);
		return rCredit17;
	}
	if (!strcmp(name, "CREDIT18"))
	{
		*size = sizeof(rCredit18);
		return rCredit18;
	}
	if (!strcmp(name, "PIXEL"))
	{
#ifdef JAPANESE
		*size = sizeof(rpixel_jp);
		return rpixel_jp;
#else
		*size = sizeof(rpixel);
		return rpixel;
#endif
	}
	
#ifndef WINDOWS
	//ICON
	if (!strcmp(name, "ICON_MINI"))
	{
		*size = sizeof(rICON_MINI);
		return rICON_MINI;
	}
#endif

	//CURSOR
	if (!strcmp(name, "CURSOR_NORMAL"))
	{
		*size = sizeof(rCURSOR_NORMAL);
		return rCURSOR_NORMAL;
	}
	if (!strcmp(name, "CURSOR_IKA"))
	{
		*size = sizeof(rCURSOR_IKA);
		return rCURSOR_IKA;
	}
	return NULL;
}

SDL_RWops* FindResource(const char *name)
{
	size_t resSize;
	const unsigned char* resource = GetResource(name, &resSize);
	
	if (!resource)
		return NULL;
	
	SDL_RWops *fp = SDL_RWFromConstMem(resource, (int)resSize);
	
	if (!fp)
	{
		printf("Couldn't open resource %s\nSDL Error: %s\n", name, SDL_GetError());
		return NULL;
	}
	
	return fp;
}
