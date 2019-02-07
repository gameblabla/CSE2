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

// Core giant energy ball projectile
void ActNpc218(NPCHAR *npc)
{
	RECT rc[2];

	rc[0] = {256, 120, 288, 152};
	rc[1] = {288, 120, 320, 152};

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (++npc->act_wait > 200)
		npc->cond = 0;

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 1)
		npc->ani_no = 0;

	npc->rect = rc[npc->ani_no];
}

//Smoke generator
void ActNpc219(NPCHAR *npc)
{
	RECT rc[1];

	rc[0] = {0, 0, 0, 0};

	if ( npc->direct )
		SetNpChar(199, npc->x + (Random(-0xA0, 0xA0) * 0x200), npc->y + (Random(-0x80, 0x80) * 0x200), 0, 0, 2, 0, 0x100);
	else if (Random(0, 40) == 1)
		SetNpChar(4, npc->x + (Random(-20, 20) * 0x200), npc->y, 0, -0x200, 0, 0, 0x100);

	npc->rect = rc[0];
}
