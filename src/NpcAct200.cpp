#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Spikes (small)
void ActNpc211(NPCHAR *npc)
{
	RECT rects[4];

	rects[0] = {256, 200, 272, 216};
	rects[1] = {272, 200, 288, 216};
	rects[2] = {288, 200, 304, 216};
	rects[3] = {304, 200, 320, 216};

	npc->rect = rects[npc->code_event];
}
