#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "CommonDefines.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Water droplet generator
void ActNpc359(NPCHAR *npc)
{
	if (gMC.x < npc->x + (((WINDOW_WIDTH / 2) + 160) * 0x200) && gMC.x > npc->x - (((WINDOW_WIDTH / 2) + 160) * 0x200) && gMC.y < npc->y + (((WINDOW_HEIGHT / 2) + 200) * 0x200) && gMC.y > npc->y - (((WINDOW_HEIGHT / 2) + 40) * 0x200) && Random(0, 100) == 2)
		SetNpChar(73, npc->x + (Random(-6, 6) * 0x200), npc->y - 0xE00, 0, 0, 0, 0, 0);
}
