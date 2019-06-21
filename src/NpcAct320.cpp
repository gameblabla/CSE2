#include "NpcAct.h"

#include <stddef.h>

#include "WindowsWrapper.h"

#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "KeyControl.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"

// Curly (carried, shooting)
void ActNpc320(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{16, 96, 32, 112},
		{48, 96, 64, 112},
		{96, 96, 112, 112},
	};

	RECT rcRight[3] = {
		{16, 112, 32, 128},
		{48, 112, 64, 128},
		{96, 112, 112, 128},
	};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;
		npc->x = gMC.x;
		npc->y = gMC.y;
		SetNpChar(321, 0, 0, 0, 0, 0, npc, 0x100);
	}

	if (gMC.flag & 8)
	{
		if (gMC.up)
		{
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y - 0x1400;
			npc->ani_no = 1;
		}
		else
		{
			npc->ani_no = 0;

			if (gMC.direct == 0)
			{
				npc->tgt_x = gMC.x + 0xE00;
				npc->tgt_y = gMC.y - 0x600;
			}
			else
			{
				npc->tgt_x = gMC.x - 0xE00;
				npc->tgt_y = gMC.y - 0x600;
			}
		}
	}
	else
	{
		if (gMC.up)
		{
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y + 0x1000;
			npc->ani_no = 2;
		}
		else if (gMC.down)
		{
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y - 0x1000;
			npc->ani_no = 1;
		}
		else
		{
			npc->ani_no = 0;

			if (gMC.direct == 0)
			{
				npc->tgt_x = gMC.x + 0xE00;
				npc->tgt_y = gMC.y - 0x600;
			}
			else
			{
				npc->tgt_x = gMC.x - 0xE00;
				npc->tgt_y = gMC.y - 0x600;
			}
		}
	}

	npc->x += (npc->tgt_x - npc->x) / 2;
	npc->y += (npc->tgt_y - npc->y) / 2;

	if (gMC.ani_no % 2)
		npc->y -= 0x200;

	if (gMC.direct == 0)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

// Curly's Nemesis
void ActNpc321(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{136, 152, 152, 168},
		{152, 152, 168, 168},
		{168, 152, 184, 168},
	};

	RECT rcRight[3] = {
		{136, 168, 152, 184},
		{152, 168, 168, 184},
		{168, 168, 184, 184},
	};

	int direct;

	if (npc->pNpc == NULL)
		return;

	switch (npc->pNpc->ani_no)
	{
		case 0:
			if (gMC.direct == 0)
			{
				npc->x = npc->pNpc->x + 0x1000;
				direct = 2;
			}
			else
			{
				npc->x = npc->pNpc->x - 0x1000;
				direct = 0;
			}

			npc->y = npc->pNpc->y;
			break;

		case 1:
			if (gMC.direct == 0)
				npc->x = npc->pNpc->x;
			else
				npc->x = npc->pNpc->x;	// Duplicate derp

			direct = 1;
			npc->y = npc->pNpc->y - 0x1400;
			break;

		case 2:
			if (gMC.direct == 0)
				npc->x = npc->pNpc->x;
			else
				npc->x = npc->pNpc->x;	// Duplicate derp

			direct = 3;
			npc->y = npc->pNpc->y + 0x1400;
			break;
	}

	npc->ani_no = npc->pNpc->ani_no;

	if (g_GameFlags & 2 && CountBulletNum(43) < 2 && gKeyTrg & gKeyShot)
	{
		SetBullet(43, npc->pNpc->x, npc->pNpc->y, direct);
		SetCaret(npc->pNpc->x, npc->pNpc->y, 3, 0);
		PlaySoundObject(117, 1);
	}

	if (gMC.direct == 0)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

// Deleet
void ActNpc322(NPCHAR *npc)
{
	RECT rc[3] = {
		{272, 216, 296, 240},
		{296, 216, 320, 240},
		{160, 216, 184, 240},
	};

	if (npc->act_no < 2 && npc->life <= 968)
	{
		npc->act_no = 2;
		npc->act_wait = 0;
		npc->bits &= ~0x20;
		npc->bits |= 4;
		PlaySoundObject(22, 1);
	}

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 0)
				npc->y += 0x1000;
			else
				npc->x += 0x1000;
			// Fallthrough
		case 1:
			if (npc->shock)
				++npc->count1;
			else
				npc->count1 = 0;

			npc->rect = rc[npc->count1 / 2 % 2];
			break;

		case 2:
			npc->ani_no = 2;

			switch (npc->act_wait)
			{
				case 0:
					SetNpChar(207, npc->x + 0x800, npc->y, 0, 0, 0, 0, 0x180);
					break;

				case 50:
					SetNpChar(207, npc->x + 0x800, npc->y, 0, 0, 1, 0, 0x180);
					break;

				case 100:
					SetNpChar(207, npc->x + 0x800, npc->y, 0, 0, 2, 0, 0x180);
					break;

				case 150:
					SetNpChar(207, npc->x + 0x800, npc->y, 0, 0, 3, 0, 0x180);
					break;

				case 200:
					SetNpChar(207, npc->x + 0x800, npc->y, 0, 0, 4, 0, 0x180);
					break;

				case 250:
					npc->hit.back = 0x6000;
					npc->hit.front = 0x6000;
					npc->hit.top = 0x6000;
					npc->hit.bottom = 0x6000;
					npc->damage = 12;
					PlaySoundObject(26, 1);
					SetDestroyNpChar(npc->x, npc->y, 0x6000, 40);
					SetQuake(10);

					if (npc->direct == 0)
					{
						DeleteMapParts(npc->x / 0x200 / 0x10, (npc->y - 0x1000) / 0x200 / 0x10);
						DeleteMapParts(npc->x / 0x200 / 0x10, (npc->y + 0x1000) / 0x200 / 0x10);
					}
					else
					{
						DeleteMapParts((npc->x - 0x1000) / 0x200 / 0x10, npc->y / 0x200 / 0x10);
						DeleteMapParts((npc->x + 0x1000) / 0x200 / 0x10, npc->y / 0x200 / 0x10);
					}

					npc->cond |= 8;
					break;
			}

			++npc->act_wait;
			npc->rect = rc[2];
			break;
	}
}

// Bute (spinning)
void ActNpc323(NPCHAR *npc)
{
	RECT rc[4] = {
		{216, 32, 232, 56},
		{232, 32, 248, 56},
		{216, 56, 232, 80},
		{232, 56, 248, 80},
	};

	if (++npc->ani_wait > 3)
	{
		npc->ani_wait = 0;

		if (++npc->ani_no > 3)
			npc->ani_no = 0;
	}

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			switch (npc->direct)
			{
				case 0:
					npc->xm = -0x600;
					break;

				case 2:
					npc->xm = 0x600;
					break;

				case 1:
					npc->ym = -0x600;
					break;

				case 3:
					npc->ym = 0x600;
					break;
			}
			// Fallthrough
		case 1:
			if (++npc->act_wait == 16)
				npc->bits &= ~8;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->flag & 0xFF)
				npc->act_no = 10;

			if (npc->act_wait > 20)
			{
				switch (npc->direct)
				{
					case 0:
						if (npc->x <= gMC.x + 0x4000)
							npc->act_no = 10;
						break;

					case 2:
						if (npc->x >= gMC.x - 0x4000)
							npc->act_no = 10;
						break;

					case 1:
						if (npc->y <= gMC.y + 0x4000)
							npc->act_no = 10;
						break;

					case 3:
						if (npc->y >= gMC.y - 0x4000)
							npc->act_no = 10;
						break;
				}
			}

			break;
	}

	if (npc->act_no == 10)
	{
		npc->code_char = 309;
		npc->ani_no = 0;
		npc->act_no = 11;
		npc->bits |= 0x20;
		npc->bits &= ~8;
		npc->damage = 5;
		npc->view.top = 0x1000;
	}

	npc->rect = rc[npc->ani_no];
}

// Bute generator
void ActNpc324(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			// Fallthrough
		case 11:
			if (++npc->act_wait % 50 == 1)
				SetNpChar(323, npc->x, npc->y, 0, 0, npc->direct, 0, 0x100);

			if (npc->act_wait > 351)
				npc->act_no = 0;

			break;
	}
}

// Heavy Press lightning
void ActNpc325(NPCHAR *npc)
{
	RECT rc[7] = {
		{240, 96, 272, 128},
		{272, 96, 304, 128},
		{240, 128, 272, 160},
		{240, 0, 256, 96},
		{256, 0, 272, 96},
		{272, 0, 288, 96},
		{288, 0, 304, 96},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			PlaySoundObject(29, 1);
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 0)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
				npc->ani_no = 0;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 10;
				npc->ani_wait = 0;
				npc->ani_no = 3;
				npc->damage = 10;
				npc->view.front = 0x1000;
				npc->view.top = 0x1800;
				PlaySoundObject(101, 1);
				SetDestroyNpChar(npc->x, npc->y + 0xA800, 0, 3);
			}

			break;

		case 10:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 6)
			{
				npc->cond = 0;
				return;
			}

			break;
	}

	npc->rect = rc[npc->ani_no];
}

// Sue/Itoh becoming humans
void ActNpc326(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			npc->x += 0x2000;
			npc->ani_no = 0;
			// Fallthrough
		case 1:
			if (++npc->act_wait > 80)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
				break;
			}
			else
			{
				if (npc->direct == 0)
				{
					if (npc->act_wait == 30)
						npc->ani_no = 1;
					if (npc->act_wait == 40)
						npc->ani_no = 0;
				}
				else
				{
					if (npc->act_wait == 50)
						npc->ani_no = 1;
					if (npc->act_wait == 60)
						npc->ani_no = 0;
				}
			}
			break;

		case 10:
			if (++npc->act_wait > 50)
			{
				npc->act_no = 15;
				npc->ani_no = 4;

				if (npc->direct == 0)
					npc->act_wait = 0;
				else
					npc->act_wait = -20;

				break;
			}
			else
			{
				if (npc->act_wait / 2 % 2)
					npc->ani_no = 2;
				else
					npc->ani_no = 3;
			}
			break;

		case 15:
			if (++npc->act_wait > 40)
			{
				npc->act_wait = 0;
				npc->act_no = 20;
			}

			break;

		case 20:
			npc->ym += 0x40;
			if (npc->ym > 0x5FF)
				npc->ym = 0x5FF;

			npc->y += npc->ym;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 30;
				npc->act_wait = 0;
				npc->ani_no = 6;

				if (npc->direct == 0)
					SetNpChar(327, npc->x, npc->y - 0x2000, 0, 0, 0, npc, 0x100);
				else
					SetNpChar(327, npc->x, npc->y - 0x1000, 0, 0, 0, npc, 0x100);

				break;
			}

			break;

		case 30:
			if (++npc->act_wait == 30)
				npc->ani_no = 7;

			if (npc->act_wait == 40)
				npc->act_no = 40;

			break;

		case 40:
			npc->act_no = 41;
			npc->act_wait = 0;
			npc->ani_no = 0;
			// Fallthrough
		case 41:
			if (++npc->act_wait == 30)
				npc->ani_no = 1;

			if (npc->act_wait == 40)
				npc->ani_no = 0;

			break;
	}

	RECT rcItoh[8] = {
		{0, 128, 16, 152},
		{16, 128, 32, 152},
		{32, 128, 48, 152},
		{48, 128, 64, 152},
		{64, 128, 80, 152},
		{80, 128, 96, 152},
		{96, 128, 112, 152},
		{112, 128, 128, 152},
	};

	RECT rcSu[8] = {
		{128, 128, 144, 152},
		{144, 128, 160, 152},
		{160, 128, 176, 152},
		{176, 128, 192, 152},
		{192, 128, 208, 152},
		{208, 128, 224, 152},
		{224, 128, 240, 152},
		{32, 152, 48, 176},
	};

	if (npc->direct == 0)
		npc->rect = rcItoh[npc->ani_no];
	else
		npc->rect = rcSu[npc->ani_no];
}

// Sneeze
void ActNpc327(NPCHAR *npc)
{
	RECT rc[2] = {
		{240, 80, 256, 96},
		{256, 80, 272, 96},
	};

	++npc->act_wait;

	switch (npc->act_no)
	{
		case 0:
			if (npc->act_wait < 4)
				npc->y -= 0x400;

			if (npc->pNpc->ani_no == 7)
			{
				npc->ani_no = 1;
				npc->act_no = 1;
				npc->tgt_x = npc->x;
				npc->tgt_y = npc->y;
			}
			break;

		case 1:
			if (npc->act_wait < 48)
			{
				npc->x = npc->tgt_x + (Random(-1, 1) * 0x200);
				npc->y = npc->tgt_y + (Random(-1, 1) * 0x200);
			}
			else
			{
				npc->x = npc->tgt_x;
				npc->y = npc->tgt_y;
			}
			break;
	}

	if (npc->act_wait > 70)
		npc->cond = 0;

	npc->rect = rc[npc->ani_no];
}

// Thingy that turns Sue and Itoh into humans for 4 seconds
void ActNpc328(NPCHAR *npc)
{
	RECT rc = {96, 0, 128, 48};
	npc->rect = rc;
}

// Laboratory fan
void ActNpc329(NPCHAR *npc)
{
	RECT rc[2] = {
		{48, 0, 64, 16},
		{64, 0, 80, 16},
	};

	if (++npc->ani_wait / 2 % 2)
		npc->rect = rc[0];
	else
		npc->rect = rc[1];
}

// Rolling
void ActNpc330(NPCHAR *npc)
{
	RECT rc[3] = {
		{144, 136, 160, 152},
		{160, 136, 176, 152},
		{176, 136, 192, 152},
	};

	switch (npc->act_no)
	{
		case 0:
			ChangeMapParts(npc->x / 0x200 / 0x10, npc->y / 0x200 / 0x10, 0);

			if (npc->direct == 0)
				npc->act_no = 10;
			else
				npc->act_no = 30;

			break;

		case 10:
			npc->xm -= 0x40;
			npc->ym = 0;

			if (npc->flag & 1)
				npc->act_no = 20;

			break;

		case 20:
			npc->xm = 0;
			npc->ym -= 0x40;

			if (npc->flag & 2)
				npc->act_no = 30;

			break;

		case 30:
			npc->xm += 0x40;
			npc->ym = 0;

			if (npc->flag & 4)
				npc->act_no = 40;

			break;

		case 40:
			npc->xm = 0;
			npc->ym += 0x40;

			if (npc->flag & 8)
				npc->act_no = 10;

			break;
	}

	if (npc->xm < -0x400)
		npc->xm = -0x400;
	if (npc->xm > 0x400)
		npc->xm = 0x400;

	if (npc->ym < -0x400)
		npc->ym = -0x400;
	if (npc->ym > 0x400)
		npc->ym = 0x400;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	npc->rect = rc[npc->ani_no];
}

// Ballos bone projectile
void ActNpc331(NPCHAR *npc)
{
	RECT rc[4] = {
		{288, 80, 304, 96},
		{304, 80, 320, 96},
		{288, 96, 304, 112},
		{304, 96, 320, 112},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (npc->flag & 8)
			{
				npc->ym = -0x200;
				npc->act_no = 10;
			}

			break;

		case 10:
			if (npc->flag & 8)
			{
				npc->cond = 0;
				SetCaret(npc->x, npc->y, 2, 0);
			}

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;
	npc->x += npc->xm;

	if (++npc->ani_wait > 3)
	{
		npc->ani_wait = 0;

		if (npc->direct == 0)
			++npc->ani_no;
		else
			--npc->ani_no;

		if (npc->ani_no < 0)
			npc->ani_no += 4;
		if (npc->ani_no > 3)
			npc->ani_no -= 4;
	}

	npc->rect = rc[npc->ani_no];
}

// Ballos shockwave
void ActNpc332(NPCHAR *npc)
{
	int xm;

	RECT rc[3] = {
		{144, 96, 168, 120},
		{168, 96, 192, 120},
		{192, 96, 216, 120},
	};

	switch (npc->act_no)
	{
		case 0:
			PlaySoundObject(44, 1);
			npc->act_no = 1;

			if (npc->direct == 0)
				npc->xm = -0x400;
			else
				npc->xm = 0x400;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;

				if (++npc->ani_no > 2)
					npc->ani_no = 0;
			}

			if (++npc->act_wait % 6 == 1)
			{
				if (npc->direct == 0)
					xm = (Random(-0x10, -4) * 0x200) / 8;
				else
					xm = (Random(4, 0x10) * 0x200) / 8;

				SetNpChar(331, npc->x, npc->y, xm, -0x400, 0, 0, 0x100);

				PlaySoundObject(12, 1);
			}

			break;
	}

	if (npc->flag & 1)
		npc->cond = 0;
	if (npc->flag & 4)
		npc->cond = 0;

	npc->x += npc->xm;

	npc->rect = rc[npc->ani_no];
}

// Ballos lightning
void ActNpc333(NPCHAR *npc)
{
	RECT rc[2] = {
		{80, 120, 104, 144},
		{104, 120, 128, 144},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			PlaySoundObject(103, 1);
			npc->y = gMC.y;
			// Fallthrough
		case 1:
			if (++npc->act_wait / 2 % 2)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (npc->direct == 0 && npc->act_wait == 20)
				SetNpChar(146, npc->tgt_x, npc->tgt_y, 0, 0, 0, 0, 0x100);

			if (npc->act_wait > 40)
				npc->cond = 0;

			break;

	}

	npc->rect = rc[npc->ani_no];
}

// Sweat
void ActNpc334(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{160, 184, 168, 200},
		{168, 184, 176, 200},
	};

	RECT rcRight[2] = {
		{176, 184, 184, 200},
		{184, 184, 192, 200},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;

			if (npc->direct == 0)
			{
				npc->x += 0x1400;
				npc->y -= 0x2400;
			}
			else
			{
				npc->x = gMC.x - 0x1400;
				npc->y = gMC.y - 0x400;
			}
			// Fallthrough
		case 10:
			if (++npc->act_wait / 8 % 2)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (npc->act_wait >= 64)
				npc->cond = 0;

			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Ikachan
void ActNpc335(NPCHAR *npc)
{
	RECT rc[3] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{32, 16, 48, 32},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(3, 20);
			// Fallthrough
		case 1:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 2;
				npc->act_wait = Random(10, 50);
				npc->ani_no = 1;
				npc->xm = 0x600;
			}

			break;

		case 2:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 3;
				npc->act_wait = Random(40, 50);
				npc->ani_no = 2;
				npc->ym = Random(-0x100, 0x100);
			}

			break;

		case 3:
			if (--npc->act_wait <= 0)
			{
				npc->act_no = 1;
				npc->act_wait = 0;
				npc->ani_no = 0;
			}

			break;
	}

	npc->xm -= 0x10;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rc[npc->ani_no];
}

// Ikachan generator
void ActNpc336(NPCHAR *npc)
{
	int y;

	switch (npc->act_no)
	{
		case 0:
			if (gMC.shock)
				npc->cond = 0;

			break;

		case 10:
			if (++npc->act_wait % 4 == 1)
			{
				y = npc->y + (Random(0, 13) * 0x200 * 0x10);
				SetNpChar(335, npc->x, y, 0, 0, 0, 0, 0);
			}

			break;
	}
}

// Numhachi
void ActNpc337(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{256, 112, 288, 152},
		{288, 112, 320, 152},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			// Fallthrough
		case 1:
			npc->xm = 0;
			npc->act_no = 2;
			npc->ani_no = 0;
			// Fallthrough
		case 2:
			if (++npc->ani_wait > 50)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rcLeft[npc->ani_no];
}

// Green Devil
void ActNpc338(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{288, 0, 304, 16},
		{304, 0, 320, 16},
	};

	RECT rcRight[2] = {
		{288, 16, 304, 32},
		{304, 16, 320, 32},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->view.top = 0x1000;
			npc->view.bottom = 0x1000;
			npc->damage = 3;
			npc->bits |= 0x20;
			npc->tgt_y = npc->y;
			npc->ym = (Random(-10, 10) * 0x200) / 2;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->y < npc->tgt_y)
				npc->ym += 0x80;
			else
				npc->ym -= 0x80;

			if (npc->direct == 0)
				npc->xm -= 0x20;
			else
				npc->xm += 0x20;

			if (npc->xm > 0x400)
				npc->xm = 0x400;
			if (npc->xm < -0x400)
				npc->xm = -0x400;

			if (npc->x < 0 || npc->y < 0 || npc->x > gMap.width * 0x200 * 0x10 || npc->y > gMap.length * 0x200 * 0x10)
			{
				VanishNpChar(npc);
				return;
			}

			break;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Green Devil generator
void ActNpc339(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(0, 40);
			// Fallthrough
		case 1:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->act_no = 0;
				SetNpChar(338, npc->x, npc->y + (Random(-0x10, 0x10) * 0x200), 0, 0, npc->direct, 0, 0x100);
			}

			break;
	}
}
