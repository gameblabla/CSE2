#include <string>
#include <stdlib.h>

#include "Tags.h"
#include "NpcTbl.h"
#include "GenericLoad.h"
#include "TextScr.h"
#include "Flags.h"
#include "Map.h"
#include "Draw.h"
#include "Ending.h"

int g_GameFlags;
int gCounter;

int Random(int min, int max)
{
	return min + rand() % (max - min + 1);
}

bool Game()
{
	if (LoadGenericData())
	{
		char path[PATH_LENGTH];
		sprintf(path, "%s/npc.tbl", gDataPath);
		if (LoadNpcTable(path))
		{
			InitTextScript2();
			InitSkipFlags();
			InitMapData2();
			InitCreditScript();
			
			StartCreditScript();
			
			while (Flip_SystemTask())
			{
				ActionCredit();
				ActionIllust();
				ActionStripper();
				CortBox(&grcFull, 0x000020);
				PutIllust();
				PutStripper();
			}
			
			/*
			int mode = 0; //1;
			while (mode)
			{
				//if ( mode == 1 )
				//	mode = ModeOpening();
				//if ( mode == 2 )
				//	mode = ModeTitle();
				//if ( mode == 3 )
				//	mode = ModeAction();
			}
			*/
			
			//EndMapData();
			//EndTextScript();
			//ReleaseNpcTable();
			//ReleaseCreditScript();
		}
		else
		{
			return false;
		}
	}
	
	return true;
}