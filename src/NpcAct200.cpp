#include "NpcAct.h"

#include "WindowsWrapper.h"

#include "Back.h"
#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

//Dragon Zombie
void ActNpc200(NPCHAR *npc)
{
	unsigned char deg;
	int ym;
	int xm;

	RECT rcLeft[6] = {
		{0, 0, 40, 40},
		{40, 0, 80, 40},
		{80, 0, 120, 40},
		{120, 0, 160, 40},
		{160, 0, 200, 40},
		{200, 0, 240, 40},
	};

	RECT rcRight[6] = {
		{0, 40, 40, 80},
		{40, 40, 80, 80},
		{80, 40, 120, 80},
		{120, 40, 160, 80},
		{160, 40, 200, 80},
		{200, 40, 240, 80},
	};

	if (npc->act_no < 100 && npc->life < 950)
	{
		PlaySoundObject(72, 1);
		SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
		SetExpObjects(npc->x, npc->y, npc->exp);
		npc->act_no = 100;
		npc->bits &= ~0x20;
		npc->damage = 0;
	}

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 10;
			npc->count1 = 0;
			// Fallthrough
		case 10:
			if (++npc->ani_wait > 30)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->count1)
				--npc->count1;

			if (npc->count1 == 0 && gMC.x > npc->x - 0xE000 && gMC.x < npc->x + 0xE000)
				npc->act_no = 20;

			break;

		case 20:
			npc->act_no = 21;
			npc->act_wait = 0;
			// Fallthrough
		case 21:
			if (++npc->act_wait / 2 % 2)
				npc->ani_no = 2;
			else
				npc->ani_no = 3;

			if (npc->act_wait > 30)
				npc->act_no = 30;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;

		case 30:
			npc->act_no = 31;
			npc->act_wait = 0;
			npc->ani_no = 4;
			npc->tgt_x = gMC.x;
			npc->tgt_y = gMC.y;
			// Fallthrough
		case 31:
			if (++npc->act_wait < 40 && npc->act_wait % 8 == 1)
			{
				if (npc->direct == 0)
					deg = GetArktan(npc->x - 0x1C00 - npc->tgt_x, npc->y - npc->tgt_y);
				else
					deg = GetArktan(npc->x + 0x1C00 - npc->tgt_x, npc->y - npc->tgt_y);

				deg += (unsigned char)Random(-6, 6);

				ym = 3 * GetSin(deg);
				xm = 3 * GetCos(deg);

				if (npc->direct == 0)
					SetNpChar(202, npc->x - 0x1C00, npc->y, xm, ym, 0, 0, 0x100);
				else
					SetNpChar(202, npc->x + 0x1C00, npc->y, xm, ym, 0, 0, 0x100);

				if ((gMC.cond & 2) == 0)
					PlaySoundObject(33, 1);
			}

			if (npc->act_wait > 60)
			{
				npc->act_no = 10;
				npc->count1 = Random(100, 200);
				npc->ani_wait = 0;
			}

			break;

		case 100:
			npc->ani_no = 5;
			break;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Dragon Zombie (dead)
void ActNpc201(NPCHAR *npc)
{
	RECT rcLeft[1] = {200, 0, 240, 40};
	RECT rcRight[1] = {200, 40, 240, 80};

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Dragon Zombie projectile
void ActNpc202(NPCHAR *npc)
{
	if (npc->flag & 0xFF)
	{
		npc->cond = 0;
		SetCaret(npc->x, npc->y, 2, 0);
	}

	npc->y += npc->ym;
	npc->x += npc->xm;

	RECT rect_left[3] = {
		{184, 216, 200, 240},
		{200, 216, 216, 240},
		{216, 216, 232, 240},
	};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if ( npc->ani_no > 2 )
		npc->ani_no = 0;

	npc->rect = rect_left[npc->ani_no];

	if (++npc->count1 > 300)
	{
		npc->cond = 0;
		SetCaret(npc->x, npc->y, 2, 0);
	}
}

//Critter (destroyed Egg Corridor)
void ActNpc203(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{0, 80, 16, 96},
		{16, 80, 32, 96},
		{32, 80, 48, 96},
	};

	RECT rcRight[3] = {
		{0, 96, 16, 112},
		{16, 96, 32, 112},
		{32, 96, 48, 112},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->y += 0x600;
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (npc->x > gMC.x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (npc->act_wait >= 8 && npc->x - 0xE000 < gMC.x && npc->x + 0xE000 > gMC.x && npc->y - 0xA000 < gMC.y && npc->y + 0xA000 > gMC.y)
			{
				npc->ani_no = 1;
			}
			else
			{
				if (npc->act_wait < 8)
					++npc->act_wait;

				npc->ani_no = 0;
			}

			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			if (npc->act_wait >= 8 && npc->x - 0x6000 < gMC.x && npc->x + 0x6000 > gMC.x && npc->y - 0xA000 < gMC.y && npc->y + 0x6000 > gMC.y)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 3;
				npc->ani_no = 2;
				npc->ym = -0x5FF;

				if ((gMC.cond & 2) == 0)
					PlaySoundObject(30, 1);

				if (npc->direct == 0)
					npc->xm = -0x100;
				else
					npc->xm = 0x100;
			}

			break;

		case 3:
			if (npc->flag & 8)
			{
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;

				if ((gMC.cond & 2) == 0)
					PlaySoundObject(23, 1);
			}

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Falling spike (small)
void ActNpc204(NPCHAR *npc)
{
	RECT rc[2] = {
		{240, 80, 256, 96},
		{240, 144, 256, 160},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x - 0x1800 && gMC.x < npc->x + 0x1800 && gMC.y > npc->y)
				npc->act_no = 2;
			break;

		case 2:
			if (++npc->act_wait / 6 % 2)
				npc->x = npc->tgt_x - 0x200;
			else
				npc->x = npc->tgt_x;

			if (npc->act_wait > 30)
			{
				npc->act_no = 3;
				npc->ani_no = 1;
			}
			break;

		case 3:
			npc->ym += 0x20;

			if (npc->flag & 0xFF)
			{
				if ((gMC.cond & 2) == 0)
					PlaySoundObject(12, 1);

				SetDestroyNpChar(npc->x, npc->y, npc->view.back, 4);
				npc->cond = 0;
				return;
			}

			break;
	}

	if (npc->ym > 0xC00)
		npc->ym = 0xC00;

	npc->y += npc->ym;

	npc->rect = rc[npc->ani_no];
}

//Falling spike (large)
void ActNpc205(NPCHAR *npc)
{
	RECT rc[2] = {
		{112, 80, 128, 112},
		{128, 80, 144, 112},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			npc->y += 0x800;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x - 0x1800 && gMC.x < npc->x + 0x1800 && gMC.y > npc->y)
				npc->act_no = 2;

			break;

		case 2:
			if (++npc->act_wait / 6 % 2)
				npc->x = npc->tgt_x - 0x200;
			else
				npc->x = npc->tgt_x;

			if (npc->act_wait > 30)
			{
				npc->act_no = 3;
				npc->ani_no = 1;
				npc->act_wait = 0;
			}

			break;

		case 3:
			npc->ym += 0x20;

			if (gMC.y > npc->y)
			{
				npc->bits &= ~0x40;
				npc->damage = 0x7F;
			}
			else
			{
				npc->bits |= 0x40;
				npc->damage = 0;
			}

			if (++npc->act_wait > 8 && npc->flag & 0xFF)
			{
				npc->bits |= 0x40;
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->ym = 0;
				npc->damage = 0;
				PlaySoundObject(12, 1);
				SetDestroyNpChar(npc->x, npc->y, npc->view.back, 4);
				SetBullet(24, npc->x, npc->y, 0);
				return;
			}

			break;

		case 4:
			if (++npc->act_wait > 4)
			{
				npc->act_no = 5;
				npc->bits |= 0x20;
			}

			break;
	}

	if (npc->ym > 0xC00)
		npc->ym = 0xC00;

	npc->y += npc->ym;

	npc->rect = rc[npc->ani_no];
}

//Counter Bomb
void ActNpc206(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			npc->count1 = 120;
			npc->act_wait = Random(0, 50);
			// Fallthrough
		case 1:
			if (++npc->act_wait < 50)
				break;

			npc->act_wait = 0;
			npc->act_no = 2;
			npc->ym = 0x300;
			break;

		case 2:
			if (gMC.x > npc->x - 0xA000 && gMC.x < npc->x + 0xA000)
			{
				npc->act_wait = 0;
				npc->act_no = 3;
			}

			if (npc->shock)
			{
				npc->act_wait = 0;
				npc->act_no = 3;
			}

			break;

		case 3:
			switch (npc->act_wait)
			{
				case 60 * 0:
					SetNpChar(207, npc->x + 0x2000, npc->y + 0x800, 0, 0, 0, 0, 0x100);
					break;

				case 60 * 1:
					SetNpChar(207, npc->x + 0x2000, npc->y + 0x800, 0, 0, 1, 0, 0x100);
					break;

				case 60 * 2:
					SetNpChar(207, npc->x + 0x2000, npc->y + 0x800, 0, 0, 2, 0, 0x100);
					break;

				case 60 * 3:
					SetNpChar(207, npc->x + 0x2000, npc->y + 0x800, 0, 0, 3, 0, 0x100);
					break;

				case 60 * 4:
					SetNpChar(207, npc->x + 0x2000, npc->y + 0x800, 0, 0, 4, 0, 0x100);
					break;

				case 60 * 5:
					npc->hit.back = 0x10000;
					npc->hit.front = 0x10000;
					npc->hit.top = 0xC800;
					npc->hit.bottom = 0xC800;
					npc->damage = 30;
					PlaySoundObject(35, 1);
					SetDestroyNpChar(npc->x, npc->y, 0x10000, 100);
					SetQuake(20);
					npc->cond |= 8;
					break;
			}

			++npc->act_wait;
			break;
	}

	if (npc->act_no > 1)
	{
		if (npc->tgt_y < npc->y)
			npc->ym -= 0x10;
		if (npc->tgt_y > npc->y)
			npc->ym += 0x10;

		if (npc->ym > 0x100)
			npc->ym = 0x100;
		if (npc->ym < -0x100)
			npc->ym = -0x100;
	}

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[3] = {
		{80, 80, 120, 120},
		{120, 80, 160, 120},
		{160, 80, 200, 120},
	};

	if (++npc->ani_wait > 4)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	npc->rect = rect_left[npc->ani_no];
}

//Counter Bomb's countdown
void ActNpc207(NPCHAR *npc)
{
	RECT rc[5] = {
		{0, 144, 16, 160},
		{16, 144, 32, 160},
		{32, 144, 48, 160},
		{48, 144, 64, 160},
		{64, 144, 80, 160},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = npc->direct;
			PlaySoundObject(43, 1);
			// Fallthrough
		case 1:
			npc->x += 0x200;

			if (++npc->act_wait > 8)
			{
				npc->act_wait = 0;
				npc->act_no = 2;
			}

			break;

		case 2:
			if (++npc->act_wait > 30)
			{
				npc->cond = 0;
				return;
			}

			break;
	}

	npc->rect = rc[npc->ani_no];
}

//Basu (destroyed Egg Corridor)
void ActNpc208(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{248, 80, 272, 104},
		{272, 80, 296, 104},
		{296, 80, 320, 104},
	};

	RECT rcRight[3] = {
		{248, 104, 272, 128},
		{272, 104, 296, 128},
		{296, 104, 320, 128},
	};

	switch (npc->act_no)
	{
		case 0:
			if (gMC.x < npc->x + 0x2000 && gMC.x > npc->x - 0x2000)
			{
				npc->bits |= 0x20;
				npc->ym = -0x200;
				npc->tgt_x = npc->x;
				npc->tgt_y = npc->y;
				npc->act_no = 1;
				npc->act_wait = 0;
				npc->count1 = npc->direct;
				npc->count2 = 0;
				npc->damage = 6;

				if (npc->direct == 0)
				{
					npc->x = gMC.x + 0x20000;
					npc->xm = -0x2FF;
				}
				else
				{
					npc->x = gMC.x - 0x20000;
					npc->xm = 0x2FF;
				}
			}
			else
			{
				npc->rect.right = 0;
				npc->damage = 0;
				npc->xm = 0;
				npc->ym = 0;
				npc->bits &= ~0x20;
			}

			return;

		case 1:
			if (npc->x > gMC.x)
			{
				npc->direct = 0;
				npc->xm -= 0x10;
			}
			else
			{
				npc->direct = 2;
				npc->xm += 0x10;
			}

			if (npc->flag & 1)
				npc->xm = 0x200;
			if (npc->flag & 4)
				npc->xm = -0x200;

			if (npc->y < npc->tgt_y)
				npc->ym += 8;
			else
				npc->ym -= 8;

			if (npc->xm > 0x2FF)
				npc->xm = 0x2FF;
			if (npc->xm < -0x2FF)
				npc->xm = -0x2FF;

			if (npc->ym > 0x200)
				npc->ym = 0x200;
			if (npc->ym < -0x200)
				npc->ym = -0x200;

			if (npc->shock)
			{
				npc->x += npc->xm / 2;
				npc->y += npc->ym / 2;
			}
			else
			{
				npc->x += npc->xm;
				npc->y += npc->ym;
			}

			if (gMC.x > npc->x + 0x32000 || gMC.x < npc->x - 0x32000)
			{
				npc->act_no = 0;
				npc->xm = 0;
				npc->direct = npc->count1;
				npc->x = npc->tgt_x;
				npc->rect.right = 0;
				npc->damage = 0;
				return;
			}

			break;
	}

	if (npc->act_no)
	{
		if (npc->act_wait < 150)
			++npc->act_wait;

		if (npc->act_wait == 150)
		{
			if (++npc->count2 % 8 == 0 && npc->x < gMC.x + 0x14000 && npc->x > gMC.x - 0x14000)
			{
				unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
				deg += (unsigned char)Random(-6, 6);
				const int ym = 3 * GetSin(deg);
				const int xm = 3 * GetCos(deg);
				SetNpChar(209, npc->x, npc->y, xm, ym, 0, 0, 0x100);
				PlaySoundObject(39, 1);
			}

			if (npc->count2 > 16)
			{
				npc->act_wait = 0;
				npc->count2 = 0;
			}
		}
	}

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 1)
		npc->ani_no = 0;

	if (npc->act_wait > 120 && npc->act_wait / 2 % 2 == 1 && npc->ani_no == 1)
		npc->ani_no = 2;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Basu projectile (destroyed Egg Corridor)
void ActNpc209(NPCHAR *npc)
{
	if (npc->flag & 0xFF)
	{
		npc->cond = 0;
		SetCaret(npc->x, npc->y, 2, 0);
	}

	npc->y += npc->ym;
	npc->x += npc->xm;

	RECT rect_left[4] = {
		{232, 96, 248, 112},
		{200, 112, 216, 128},
		{216, 112, 232, 128},
		{232, 112, 248, 128},
	};

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 3)
		npc->ani_no = 0;

	npc->rect = rect_left[npc->ani_no];

	if (++npc->count1 > 300)
	{
		npc->cond = 0;
		SetCaret(npc->x, npc->y, 2, 0);
	}
}

//Beetle (destroyed Egg Corridor)
void ActNpc210(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{0, 112, 16, 128},
		{16, 112, 32, 128},
	};

	RECT rcRight[2] = {
		{32, 112, 48, 128},
		{48, 112, 64, 128},
	};

	switch (npc->act_no)
	{
		case 0:
			if (gMC.x < npc->x + 0x2000 && gMC.x > npc->x - 0x2000)
			{
				npc->bits |= 0x20;
				npc->ym = -0x200;
				npc->tgt_y = npc->y;
				npc->act_no = 1;
				npc->damage = 2;

				if (npc->direct == 0)
				{
					npc->x = gMC.x + 0x20000;
					npc->xm = -0x2FF;
				}
				else
				{
					npc->x = gMC.x - 0x20000;
					npc->xm = 0x2FF;
				}
			}
			else
			{
				npc->bits &= ~0x20;
				npc->rect.right = 0;
				npc->damage = 0;
				npc->xm = 0;
				npc->ym = 0;
				return;
			}

			break;

		case 1:
			if (npc->x > gMC.x)
			{
				npc->direct = 0;
				npc->xm -= 0x10;
			}
			else
			{
				npc->direct = 2;
				npc->xm += 0x10;
			}

			if (npc->xm > 0x2FF)
				npc->xm = 0x2FF;
			if (npc->xm < -0x2FF)
				npc->xm = -0x2FF;

			if (npc->y < npc->tgt_y)
				npc->ym += 8;
			else
				npc->ym -= 8;

			if (npc->ym > 0x200)
				npc->ym = 0x200;
			if (npc->ym < -0x200)
				npc->ym = -0x200;

			if (npc->shock)
			{
				npc->x += npc->xm / 2;
				npc->y += npc->ym / 2;
			}
			else
			{
				npc->x += npc->xm;
				npc->y += npc->ym;
			}

			break;
	}

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 1)
		npc->ani_no = 0;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Spikes (small)
void ActNpc211(NPCHAR *npc)
{
	RECT rects[4] = {
		{256, 200, 272, 216},
		{272, 200, 288, 216},
		{288, 200, 304, 216},
		{304, 200, 320, 216},
	};

	npc->rect = rects[npc->code_event];
}

//Sky Dragon
void ActNpc212(NPCHAR *npc)
{
	RECT rcRight[4] = {
		{160, 152, 200, 192},
		{200, 152, 240, 192},
		{240, 112, 280, 152},
		{280, 112, 320, 152},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x800;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 30)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 2;
			npc->ani_wait = 0;
			npc->tgt_y = npc->y - 0x2000;
			npc->tgt_x = npc->x - 0xC00;
			npc->ym = 0;
			npc->bits |= 8;
			// Fallthrough
		case 11:
			if (npc->x < npc->tgt_x)
				npc->xm += 8;
			else
				npc->xm -= 8;

			if (npc->y < npc->tgt_y)
				npc->ym += 8;
			else
				npc->ym -= 8;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (++npc->ani_wait > 5)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 2;

			break;

		case 20:
			npc->act_no = 21;
			npc->bits |= 8;
			// Fallthrough
		case 21:
			if (npc->y < npc->tgt_y)
				npc->ym += 0x10;
			else
				npc->ym -= 0x10;

			npc->xm += 0x20;

			if (npc->xm > 0x600)
				npc->xm = 0x600;
			if (npc->xm < -0x600)
				npc->xm = -0x600;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 2;

			break;

		case 30:
			npc->act_no = 31;
			SetNpChar(297, 0, 0, 0, 0, 0, npc, 0x100);
			break;
	}

	npc->rect = rcRight[npc->ani_no];

	if (gMC.equip & 0x40)
	{
		if (npc->ani_no > 1)
		{
			npc->rect.top += 40;
			npc->rect.bottom += 40;
		}
	}
}

//Night Spirit
void ActNpc213(NPCHAR *npc)
{
	RECT rect[10] = {
		{0, 0, 0, 0},
		{0, 0, 48, 48},
		{48, 0, 96, 48},
		{96, 0, 144, 48},
		{144, 0, 192, 48},
		{192, 0, 240, 48},
		{240, 0, 288, 48},
		{0, 48, 48, 96},
		{48, 48, 96, 96},
		{96, 48, 144, 96},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->ani_no = 0;
			npc->tgt_x = npc->x;
			npc->tgt_y = npc->y;
			// Fallthrough
		case 1:
			if (gMC.y > npc->y - 0x1000 && gMC.y < npc->y + 0x1000)
			{
				if (npc->direct == 0)
					npc->y -= 0x1E000;
				else
					npc->y += 0x1E000;

				npc->act_no = 10;
				npc->act_wait = 0;
				npc->ani_no = 1;
				npc->ym = 0;
				npc->bits |= 0x20;
			}

			break;

		case 10:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 1;

			if (++npc->act_wait > 200)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
				npc->ani_no = 4;
			}

			break;

		case 20:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 6)
				npc->ani_no = 4;

			if (++npc->act_wait > 50)
			{
				npc->act_no = 30;
				npc->act_wait = 0;
				npc->ani_no = 7;
			}

			break;

		case 30:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 9)
				npc->ani_no = 7;

			if (++npc->act_wait % 5 == 1)
			{
				SetNpChar(214, npc->x, npc->y, (Random(2, 12) * 0x200) / 4, Random(-0x200, 0x200), 0, 0, 0x100);
				PlaySoundObject(21, 1);
			}

			if (npc->act_wait > 50)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 40:
			if (npc->y < npc->tgt_y)
				npc->ym += 0x40;
			else
				npc->ym -= 0x40;

			if (npc->ym < -0x400)
				npc->ym = -0x400;
			if (npc->ym > 0x400)
				npc->ym = 0x400;

			if (npc->shock)
				npc->y += npc->ym / 2;
			else
				npc->y += npc->ym;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 6)
				npc->ani_no = 4;

			if (gMC.y < npc->tgt_y + 0x1E000 && gMC.y > npc->tgt_y - 0x1E000)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
				npc->ani_no = 4;
			}
			break;
	}

	if (npc->act_no >= 10 && npc->act_no <= 30)
	{
		if (npc->y < gMC.y)
			npc->ym += 25;
		else
			npc->ym -= 25;

		if (npc->ym < -0x400)
			npc->ym = -0x400;
		if (npc->ym > 0x400)
			npc->ym = 0x400;

		if (npc->flag & 2)
			npc->ym = 0x200;
		if (npc->flag & 8)
			npc->ym = -0x200;

		if (npc->shock)
			npc->y += npc->ym / 2;
		else
			npc->y += npc->ym;

		if ( gMC.y > npc->tgt_y + 0x1E000 || gMC.y < npc->tgt_y - 0x1E000 )
			npc->act_no = 40;
	}

	npc->rect = rect[npc->ani_no];
}

//Night Spirit projectile
void ActNpc214(NPCHAR *npc)
{
	RECT rect[3] = {
		{144, 48, 176, 64},
		{176, 48, 208, 64},
		{208, 48, 240, 64},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->bits |= 8;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
				npc->ani_no = 0;

			npc->xm -= 25;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->xm < 0)
				npc->bits &= ~8;

			if (npc->flag & 0xFF)
			{
				SetDestroyNpChar(npc->x, npc->y, npc->view.back, 4);
				PlaySoundObject(28, 1);
				npc->cond = 0;
			}

			break;
	}

	npc->rect = rect[npc->ani_no];
}

//Sandcroc (Outer Wall)
void ActNpc215(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->ani_no = 0;
			npc->act_no = 1;
			npc->act_wait = 0;
			npc->tgt_y = npc->y;
			npc->bits &= ~0x20;
			npc->bits &= ~4;
			npc->bits &= ~1;
			npc->bits &= ~8;
			// Fallthrough
		case 1:
			if (gMC.x > npc->x - 0x1800 && gMC.x < npc->x + 0x1800 && gMC.y > npc->y && gMC.y < npc->y + 0x1000)
			{
				npc->act_no = 15;
				npc->act_wait = 0;
			}

			break;

		case 15:
			if (++npc->act_wait > 10)
			{
				PlaySoundObject(102, 1);
				npc->act_no = 20;
			}

			break;

		case 20:
			if (++npc->ani_wait > 3)
			{
				++npc->ani_no;
				npc->ani_wait = 0;
			}

			if (npc->ani_no == 3)
				npc->damage = 15;

			if (npc->ani_no == 4)
			{
				npc->bits |= 0x20;
				npc->act_no = 30;
				npc->act_wait = 0;
			}

			break;

		case 30:
			npc->bits |= 1;
			npc->damage = 0;
			++npc->act_wait;

			if (npc->shock)
			{
				npc->act_no = 40;
				npc->act_wait = 0;
			}

			break;

		case 40:
			npc->bits |= 8;
			npc->y += 0x200;

			if (++npc->act_wait == 32)
			{
				npc->bits &= ~1;
				npc->bits &= ~0x20;
				npc->act_no = 50;
				npc->act_wait = 0;
			}

			break;

		case 50:
			if (npc->act_wait < 100)
			{
				++npc->act_wait;
			}
			else
			{
				npc->y = npc->tgt_y;
				npc->ani_no = 0;
				npc->act_no = 0;
			}

			break;
	}

	RECT rect[5] = {
		{0, 0, 0, 0},
		{0, 96, 48, 128},
		{48, 96, 96, 128},
		{96, 96, 144, 128},
		{144, 96, 192, 128},
	};

	npc->rect = rect[npc->ani_no];
}

//Debug Cat
void ActNpc216(NPCHAR *npc)
{
	RECT rect = {256, 192, 272, 216};

	npc->rect = rect;
}

//Itoh
void ActNpc217(NPCHAR *npc)
{
	RECT rect[8] = {
		{144, 64, 160, 80},
		{160, 64, 176, 80},
		{176, 64, 192, 80},
		{192, 64, 208, 80},
		{144, 80, 160, 96},
		{160, 80, 176, 96},
		{144, 80, 160, 96},
		{176, 80, 192, 96},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 10:
			npc->ani_no = 2;
			npc->xm = 0;
			break;

		case 20:
			npc->act_no = 21;
			npc->ani_no = 2;
			npc->xm += 0x200;
			npc->ym -= 0x400;
			break;

		case 21:
			if (npc->flag & 8)
			{
				npc->ani_no = 3;
				npc->act_no = 30;
				npc->act_wait = 0;
				npc->xm = 0;
				npc->tgt_x = npc->x;
			}

			break;

		case 30:
			npc->ani_no = 3;

			if (++npc->act_wait / 2 % 2)
				npc->x = npc->tgt_x + 0x200;
			else
				npc->x = npc->tgt_x;

			break;

		case 40:
			npc->act_no = 41;
			npc->ym = -0x200;
			npc->ani_no = 2;
			// Fallthrough
		case 41:
			if (npc->flag & 8)
			{
				npc->act_no = 42;
				npc->ani_no = 4;
			}

			break;

		case 42:
			npc->xm = 0;
			npc->ani_no = 4;
			break;

		case 50:
			npc->act_no = 51;
			npc->act_wait = 0;
			// Fallthrough
		case 51:
			if (++npc->act_wait > 32)
				npc->act_no = 42;

			npc->xm = 0x200;

			if (++npc->ani_wait > 3)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 7)
				npc->ani_no = 4;

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rect[npc->ani_no];
}

// Core giant energy ball projectile
void ActNpc218(NPCHAR *npc)
{
	RECT rc[2] = {
		{256, 120, 288, 152},
		{288, 120, 320, 152},
	};

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
	RECT rc = {0, 0, 0, 0};

	if (npc->direct == 0)
	{
		if (Random(0, 40) == 1)
			SetNpChar(4, npc->x + (Random(-20, 20) * 0x200), npc->y, 0, -0x200, 0, 0, 0x100);
	}
	else
	{
		SetNpChar(199, npc->x + (Random(-0xA0, 0xA0) * 0x200), npc->y + (Random(-0x80, 0x80) * 0x200), 0, 0, 2, 0, 0x100);
	}

	npc->rect = rc;
}
