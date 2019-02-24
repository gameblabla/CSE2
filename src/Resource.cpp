#include "Resource.h"

#include <stdint.h>
#include <cstring>

#include "WindowsWrapper.h"

#include "Resource/ORG/ACCESS.org.h"
#include "Resource/ORG/ANZEN.org.h"
#include "Resource/ORG/BALCONY.org.h"
#include "Resource/ORG/BALLOS.org.h"
#include "Resource/ORG/BDOWN.org.h"
#include "Resource/ORG/CEMETERY.org.h"
#include "Resource/ORG/CURLY.org.h"
#include "Resource/ORG/DR.org.h"
#include "Resource/ORG/ENDING.org.h"
#include "Resource/ORG/ESCAPE.org.h"
#include "Resource/ORG/FANFALE1.org.h"
#include "Resource/ORG/FANFALE2.org.h"
#include "Resource/ORG/FANFALE3.org.h"
#include "Resource/ORG/FIREEYE.org.h"
#include "Resource/ORG/GAMEOVER.org.h"
#include "Resource/ORG/GINSUKE.org.h"
#include "Resource/ORG/GRAND.org.h"
#include "Resource/ORG/GRAVITY.org.h"
#include "Resource/ORG/HELL.org.h"
#include "Resource/ORG/IRONH.org.h"
#include "Resource/ORG/JENKA.org.h"
#include "Resource/ORG/JENKA2.org.h"
#include "Resource/ORG/KODOU.org.h"
#include "Resource/ORG/LASTBT3.org.h"
#include "Resource/ORG/LASTBTL.org.h"
#include "Resource/ORG/LASTCAVE.org.h"
#include "Resource/ORG/MARINE.org.h"
#include "Resource/ORG/MAZE.org.h"
#include "Resource/ORG/MDOWN2.org.h"
#include "Resource/ORG/MURA.org.h"
#include "Resource/ORG/OSIDE.org.h"
#include "Resource/ORG/PLANT.org.h"
#include "Resource/ORG/QUIET.org.h"
#include "Resource/ORG/REQUIEM.org.h"
#include "Resource/ORG/TOROKO.org.h"
#include "Resource/ORG/VIVI.org.h"
#include "Resource/ORG/WANPAK2.org.h"
#include "Resource/ORG/WANPAKU.org.h"
#include "Resource/ORG/WEED.org.h"
#include "Resource/ORG/WHITE.org.h"
#include "Resource/ORG/XXXX.org.h"
#include "Resource/ORG/ZONBIE.org.h"
#include "Resource/WAVE/WAVE100.h"
#include "Resource/BITMAP/CREDIT01.bmp.h"
#include "Resource/BITMAP/CREDIT02.bmp.h"
#include "Resource/BITMAP/CREDIT03.bmp.h"
#include "Resource/BITMAP/CREDIT04.bmp.h"
#include "Resource/BITMAP/CREDIT05.bmp.h"
#include "Resource/BITMAP/CREDIT06.bmp.h"
#include "Resource/BITMAP/CREDIT07.bmp.h"
#include "Resource/BITMAP/CREDIT08.bmp.h"
#include "Resource/BITMAP/CREDIT09.bmp.h"
#include "Resource/BITMAP/CREDIT10.bmp.h"
#include "Resource/BITMAP/CREDIT11.bmp.h"
#include "Resource/BITMAP/CREDIT12.bmp.h"
#include "Resource/BITMAP/CREDIT14.bmp.h"
#include "Resource/BITMAP/CREDIT15.bmp.h"
#include "Resource/BITMAP/CREDIT16.bmp.h"
#include "Resource/BITMAP/CREDIT17.bmp.h"
#include "Resource/BITMAP/CREDIT18.bmp.h"
#ifdef JAPANESE
#include "Resource/BITMAP/PIXEL_JP.bmp.h"
#else
#include "Resource/BITMAP/PIXEL.bmp.h"
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
		*size = sizeof(rACCESS);
		return rACCESS;
	}
	if (!strcmp(name, "ANZEN"))
	{
		*size = sizeof(rANZEN);
		return rANZEN;
	}
	if (!strcmp(name, "BALCONY"))
	{
		*size = sizeof(rBALCONY);
		return rBALCONY;
	}
	if (!strcmp(name, "BALLOS"))
	{
		*size = sizeof(rBALLOS);
		return rBALLOS;
	}
	if (!strcmp(name, "BDOWN"))
	{
		*size = sizeof(rBDOWN);
		return rBDOWN;
	}
	if (!strcmp(name, "CEMETERY"))
	{
		*size = sizeof(rCEMETERY);
		return rCEMETERY;
	}
	if (!strcmp(name, "CURLY"))
	{
		*size = sizeof(rCURLY);
		return rCURLY;
	}
	if (!strcmp(name, "DR"))
	{
		*size = sizeof(rDR);
		return rDR;
	}
	if (!strcmp(name, "ENDING"))
	{
		*size = sizeof(rENDING);
		return rENDING;
	}
	if (!strcmp(name, "ESCAPE"))
	{
		*size = sizeof(rESCAPE);
		return rESCAPE;
	}
	if (!strcmp(name, "FANFALE1"))
	{
		*size = sizeof(rFANFALE1);
		return rFANFALE1;
	}
	if (!strcmp(name, "FANFALE2"))
	{
		*size = sizeof(rFANFALE2);
		return rFANFALE2;
	}
	if (!strcmp(name, "FANFALE3"))
	{
		*size = sizeof(rFANFALE3);
		return rFANFALE3;
	}
	if (!strcmp(name, "FIREEYE"))
	{
		*size = sizeof(rFIREEYE);
		return rFIREEYE;
	}
	if (!strcmp(name, "GAMEOVER"))
	{
		*size = sizeof(rGAMEOVER);
		return rGAMEOVER;
	}
	if (!strcmp(name, "GINSUKE"))
	{
		*size = sizeof(rGINSUKE);
		return rGINSUKE;
	}
	if (!strcmp(name, "GRAND"))
	{
		*size = sizeof(rGRAND);
		return rGRAND;
	}
	if (!strcmp(name, "GRAVITY"))
	{
		*size = sizeof(rGRAVITY);
		return rGRAVITY;
	}
	if (!strcmp(name, "HELL"))
	{
		*size = sizeof(rHELL);
		return rHELL;
	}
	if (!strcmp(name, "IRONH"))
	{
		*size = sizeof(rIRONH);
		return rIRONH;
	}
	if (!strcmp(name, "JENKA"))
	{
		*size = sizeof(rJENKA);
		return rJENKA;
	}
	if (!strcmp(name, "JENKA2"))
	{
		*size = sizeof(rJENKA2);
		return rJENKA2;
	}
	if (!strcmp(name, "KODOU"))
	{
		*size = sizeof(rKODOU);
		return rKODOU;
	}
	if (!strcmp(name, "LASTBT3"))
	{
		*size = sizeof(rLASTBT3);
		return rLASTBT3;
	}
	if (!strcmp(name, "LASTBTL"))
	{
		*size = sizeof(rLASTBTL);
		return rLASTBTL;
	}
	if (!strcmp(name, "LASTCAVE"))
	{
		*size = sizeof(rLASTCAVE);
		return rLASTCAVE;
	}
	if (!strcmp(name, "MARINE"))
	{
		*size = sizeof(rMARINE);
		return rMARINE;
	}
	if (!strcmp(name, "MAZE"))
	{
		*size = sizeof(rMAZE);
		return rMAZE;
	}
	if (!strcmp(name, "MDOWN2"))
	{
		*size = sizeof(rMDOWN2);
		return rMDOWN2;
	}
	if (!strcmp(name, "MURA"))
	{
		*size = sizeof(rMURA);
		return rMURA;
	}
	if (!strcmp(name, "OSIDE"))
	{
		*size = sizeof(rOSIDE);
		return rOSIDE;
	}
	if (!strcmp(name, "PLANT"))
	{
		*size = sizeof(rPLANT);
		return rPLANT;
	}
	if (!strcmp(name, "QUIET"))
	{
		*size = sizeof(rQUIET);
		return rQUIET;
	}
	if (!strcmp(name, "REQUIEM"))
	{
		*size = sizeof(rREQUIEM);
		return rREQUIEM;
	}
	if (!strcmp(name, "TOROKO"))
	{
		*size = sizeof(rTOROKO);
		return rTOROKO;
	}
	if (!strcmp(name, "VIVI"))
	{
		*size = sizeof(rVIVI);
		return rVIVI;
	}
	if (!strcmp(name, "WANPAK2"))
	{
		*size = sizeof(rWANPAK2);
		return rWANPAK2;
	}
	if (!strcmp(name, "WANPAKU"))
	{
		*size = sizeof(rWANPAKU);
		return rWANPAKU;
	}
	if (!strcmp(name, "WEED"))
	{
		*size = sizeof(rWEED);
		return rWEED;
	}
	if (!strcmp(name, "WHITE"))
	{
		*size = sizeof(rWHITE);
		return rWHITE;
	}
	if (!strcmp(name, "XXXX"))
	{
		*size = sizeof(rXXXX);
		return rXXXX;
	}
	if (!strcmp(name, "ZONBIE"))
	{
		*size = sizeof(rZONBIE);
		return rZONBIE;
	}
	
	//WAVE
	if (!strcmp(name, "WAVE100"))
	{
		*size = sizeof(rWAVE100);
		return rWAVE100;
	}
	
	//Bitmap
	if (!strcmp(name, "CREDIT01"))
	{
		*size = sizeof(rCREDIT01);
		return rCREDIT01;
	}
	if (!strcmp(name, "CREDIT02"))
	{
		*size = sizeof(rCREDIT02);
		return rCREDIT02;
	}
	if (!strcmp(name, "CREDIT03"))
	{
		*size = sizeof(rCREDIT03);
		return rCREDIT03;
	}
	if (!strcmp(name, "CREDIT04"))
	{
		*size = sizeof(rCREDIT04);
		return rCREDIT04;
	}
	if (!strcmp(name, "CREDIT05"))
	{
		*size = sizeof(rCREDIT05);
		return rCREDIT05;
	}
	if (!strcmp(name, "CREDIT06"))
	{
		*size = sizeof(rCREDIT06);
		return rCREDIT06;
	}
	if (!strcmp(name, "CREDIT07"))
	{
		*size = sizeof(rCREDIT07);
		return rCREDIT07;
	}
	if (!strcmp(name, "CREDIT08"))
	{
		*size = sizeof(rCREDIT08);
		return rCREDIT08;
	}
	if (!strcmp(name, "CREDIT09"))
	{
		*size = sizeof(rCREDIT09);
		return rCREDIT09;
	}
	if (!strcmp(name, "CREDIT10"))
	{
		*size = sizeof(rCREDIT10);
		return rCREDIT10;
	}
	if (!strcmp(name, "CREDIT11"))
	{
		*size = sizeof(rCREDIT11);
		return rCREDIT11;
	}
	if (!strcmp(name, "CREDIT12"))
	{
		*size = sizeof(rCREDIT12);
		return rCREDIT12;
	}
	if (!strcmp(name, "CREDIT14"))
	{
		*size = sizeof(rCREDIT14);
		return rCREDIT14;
	}
	if (!strcmp(name, "CREDIT15"))
	{
		*size = sizeof(rCREDIT15);
		return rCREDIT15;
	}
	if (!strcmp(name, "CREDIT16"))
	{
		*size = sizeof(rCREDIT16);
		return rCREDIT16;
	}
	if (!strcmp(name, "CREDIT17"))
	{
		*size = sizeof(rCREDIT17);
		return rCREDIT17;
	}
	if (!strcmp(name, "CREDIT18"))
	{
		*size = sizeof(rCREDIT18);
		return rCREDIT18;
	}
	if (!strcmp(name, "PIXEL"))
	{
#ifdef JAPANESE
		*size = sizeof(rPIXEL_JP);
		return rPIXEL_JP;
#else
		*size = sizeof(rPIXEL);
		return rPIXEL;
#endif
	}

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

const unsigned char* FindResource(const char *name, unsigned int *size)
{
	unsigned int resSize;
	const unsigned char* resource = GetResource(name, &resSize);
	
	if (!resource)
		return FALSE;
	
	if (size != NULL)
		*size = resSize;
	return resource;
}
