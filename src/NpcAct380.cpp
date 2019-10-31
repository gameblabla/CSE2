#include "NpcAct.h"
#include "NpChar.h"
#include "MyChar.h"

/// "Follow" a coordinate by getting closer to it
/// Returns currentX + (1/32) * (targetX - currentX), thusly getting currentX 1/32th closer to targetX
inline int followCoordinate(int currentX, int targetX)
{
	return (targetX + 31 * currentX) >> 5;
}

/// Creepy Curly
/// NPC that follows the player or an NPC
void ActNpc381(NPCHAR *npc)
{
	npc->direct = 0;
	if (npc->act_no > 200)
	{
		// Follow the first NPC for which act_no == this->act_no - 1
		for (size_t i = 0; i < NPC_MAX; ++i)
		{
			NPCHAR *checkedNpc = &gNPC[i];
			if ((checkedNpc->cond & 0x80) && checkedNpc->act_no == npc->act_no - 1)
			{
				NPCHAR *matchedNpc = checkedNpc;
				if (npc->x <= matchedNpc->x)
					npc->direct = 2;

				npc->x = followCoordinate(npc->x, matchedNpc->x);
				npc->y = followCoordinate(npc->y, matchedNpc->y);
			}
		}
	}
	else
	{
		// Follow the player
		if (npc->x <= gMC.x)
			npc->direct = 2;
		npc->x = followCoordinate(npc->x, gMC.x);
		npc->y = followCoordinate(npc->y, gMC.y);
	}

	int animationIterator = ++npc->act_wait / 3 & 3;

	if (animationIterator == 2)
		animationIterator = 0;

	if (animationIterator == 3)
		animationIterator = 2;

	npc->rect.left = animationIterator * 16;
	npc->rect.right = npc->rect.left + 16;
	npc->rect.top = npc->direct * 8 + 96;
	npc->rect.bottom = npc->rect.top + 16;
}
