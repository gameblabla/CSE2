#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Red petals
void ActNpc116(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {272, 184, 320, 200};

	npc->rect = rc[0];
}

//Table and chair
void ActNpc119(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {248, 184, 272, 200};

	npc->rect = rc[0];
}
