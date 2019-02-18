#include "NpcAct.h"

#include "WindowsWrapper.h"

#include "Back.h"
#include "Bullet.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "KeyControl.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

//Curly (carried, shooting)
void ActNpc320(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {16, 96, 32, 112};
	rcLeft[1] = {48, 96, 64, 112};
	rcLeft[2] = {96, 96, 112, 112};

	rcRight[0] = {16, 112, 32, 128};
	rcRight[1] = {48, 112, 64, 128};
	rcRight[2] = {96, 112, 112, 128};

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

	if (gMC.ani_no & 1)
		npc->y -= 0x200;

	if (gMC.direct == 0)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

//Curly's Nemesis
void ActNpc321(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {136, 152, 152, 168};
	rcLeft[1] = {152, 152, 168, 168};
	rcLeft[2] = {168, 152, 184, 168};

	rcRight[0] = {136, 168, 152, 184};
	rcRight[1] = {152, 168, 168, 184};
	rcRight[2] = {168, 168, 184, 184};

	char direct;

	if (npc->pNpc)
	{
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
				npc->x = npc->pNpc->x;
				direct = 1;
				npc->y = npc->pNpc->y - 0x1400;
				break;

			case 2:
				npc->x = npc->pNpc->x;
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
}

//Deleet
void ActNpc322(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {272, 216, 296, 240};
	rc[1] = {296, 216, 320, 240};
	rc[2] = {160, 216, 184, 240};

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

			npc->rect = rc[npc->count1 / 2 & 1];
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
						DeleteMapParts(npc->x / 0x2000, (npc->y - 0x1000) / 0x2000);
						DeleteMapParts(npc->x / 0x2000, (npc->y + 0x1000) / 0x2000);
					}
					else
					{
						DeleteMapParts((npc->x - 0x1000) / 0x2000, npc->y / 0x2000);
						DeleteMapParts((npc->x + 0x1000) / 0x2000, npc->y / 0x2000);
					}

					npc->cond |= 8;
					break;
			}

			++npc->act_wait;
			npc->rect = rc[2];
			break;
	}
}

//Bute (spinning)
void ActNpc323(NPCHAR *npc)
{
	RECT rc[4];

	rc[0] = {216, 32, 232, 56};
	rc[1] = {232, 32, 248, 56};
	rc[2] = {216, 56, 232, 80};
	rc[3] = {232, 56, 248, 80};

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

//Bute generator
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

//Heavy Press lightning
void ActNpc325(NPCHAR *npc)
{
	RECT rc[7];

	rc[0] = {240, 96, 272, 128};
	rc[1] = {272, 96, 304, 128};
	rc[2] = {240, 128, 272, 160};
	rc[3] = {240, 0, 256, 96};
	rc[4] = {256, 0, 272, 96};
	rc[5] = {272, 0, 288, 96};
	rc[6] = {288, 0, 304, 96};

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

//Sue/Itoh becoming humans
void ActNpc326(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			npc->x += 0x2000;
			npc->ani_no = 0;
			//Fallthrough
		case 1:
			if (++npc->act_wait > 80)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
			}
			else
			{
				if (npc->direct)
				{
					if (npc->act_wait == 50)
						npc->ani_no = 1;
					if (npc->act_wait == 60)
						npc->ani_no = 0;
				}
				else
				{
					if (npc->act_wait == 30)
						npc->ani_no = 1;
					if (npc->act_wait == 40)
						npc->ani_no = 0;
				}
			}
			break;
		case 10:
			if (++npc->act_wait > 50)
			{
				npc->act_no = 15;
				npc->ani_no = 4;
				if ( npc->direct )
					npc->act_wait = -20;
				else
					npc->act_wait = 0;
			}
			else
			{
				if (npc->act_wait / 2 & 1)
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
				
				if (npc->direct)
					SetNpChar(327, npc->x, npc->y - 0x1000, 0, 0, 0, npc, 0x100);
				else
					SetNpChar(327, npc->x, npc->y - 0x2000, 0, 0, 0, npc, 0x100);
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
			//Fallthrough
		case 41:
			if (++npc->act_wait == 30)
				npc->ani_no = 1;
			if (npc->act_wait == 40)
				npc->ani_no = 0;
			break;
		default:
			break;
	}
	
	RECT rcSu[8];
	RECT rcItoh[8];
	rcItoh[0] = {0, 128, 16, 152};
	rcItoh[1] = {16, 128, 32, 152};
	rcItoh[2] = {32, 128, 48, 152};
	rcItoh[3] = {48, 128, 64, 152};
	rcItoh[4] = {64, 128, 80, 152};
	rcItoh[5] = {80, 128, 96, 152};
	rcItoh[6] = {96, 128, 112, 152};
	rcItoh[7] = {112, 128, 128, 152};
	rcSu[0] = {128, 128, 144, 152};
	rcSu[1] = {144, 128, 160, 152};
	rcSu[2] = {160, 128, 176, 152};
	rcSu[3] = {176, 128, 192, 152};
	rcSu[4] = {192, 128, 208, 152};
	rcSu[5] = {208, 128, 224, 152};
	rcSu[6] = {224, 128, 240, 152};
	rcSu[7] = {32, 152, 48, 176};
	
	if (npc->direct)
		npc->rect = rcSu[npc->ani_no];
	else
		npc->rect = rcItoh[npc->ani_no];
}

//Sneeze
void ActNpc327(NPCHAR *npc)
{
	RECT rc[2];
	rc[0] = {240, 80, 256, 96};
	rc[1] = {256, 80, 272, 96};
	
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
			if (npc->act_wait >= 48)
			{
				npc->x = npc->tgt_x;
				npc->y = npc->tgt_y;
			}
			else
			{
				npc->x = npc->tgt_x + (Random(-1, 1) << 9);
				npc->y = npc->tgt_y + (Random(-1, 1) << 9);
			}
			break;
	}
	
	if (npc->act_wait > 70)
		npc->cond = 0;
	
	npc->rect = rc[npc->ani_no];
}

//Thingy that turns Sue and Itoh into humans for 4 seconds
void ActNpc328(NPCHAR *npc)
{
	npc->rect = {96, 0, 128, 48};
}

//Laboratory fan
void ActNpc329(NPCHAR *npc)
{
	if (++npc->ani_wait / 2 & 1)
		npc->rect = {48, 0, 64, 16};
	else
		npc->rect = {64, 0, 80, 16};
}

//Rolling
void ActNpc330(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {144, 136, 160, 152};
	rc[1] = {160, 136, 176, 152};
	rc[2] = {176, 136, 192, 152};

	switch (npc->act_no)
	{
		case 0:
			ChangeMapParts(npc->x / 0x2000, npc->y / 0x2000, 0);

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

//Ballos bone projectile
void ActNpc331(NPCHAR *npc)
{
	RECT rc[4];

	rc[0] = {288, 80, 304, 96};
	rc[1] = {304, 80, 320, 96};
	rc[2] = {288, 96, 304, 112};
	rc[3] = {304, 96, 320, 112};

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

//Ballos shockwave
void ActNpc332(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {144, 96, 168, 120};
	rc[1] = {168, 96, 192, 120};
	rc[2] = {192, 96, 216, 120};

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
				{
					const int xm = (Random(-0x10, -4) * 0x200) / 8;
					SetNpChar(331, npc->x, npc->y, xm, -0x400, 0, 0, 0x100);
				}
				else
				{
					const int xm = (Random(4, 0x10) * 0x200) / 8;
					SetNpChar(331, npc->x, npc->y, xm, -0x400, 0, 0, 0x100);
				}

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

//Ballos lightning
void ActNpc333(NPCHAR *npc)
{
	RECT rc[2];

	rc[0] = {80, 120, 104, 144};
	rc[1] = {104, 120, 128, 144};

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
			if (++npc->act_wait / 2 & 1)
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

//Sweat
void ActNpc334(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {160, 184, 168, 200};
	rcLeft[1] = {168, 184, 176, 200};

	rcRight[0] = {176, 184, 184, 200};
	rcRight[1] = {184, 184, 192, 200};

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
			if (++npc->act_wait / 8 & 1)
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

//Ikachan
void ActNpc335(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {0, 16, 16, 32};
	rc[1] = {16, 16, 32, 32};
	rc[2] = {32, 16, 48, 32};

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

//Ikachan generator
void ActNpc336(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			if (gMC.shock)
				npc->cond = 0;

			break;

		case 10:
			if (++npc->act_wait % 4 == 1 )
				SetNpChar(335, npc->x, npc->y + (Random(0, 13) * 0x2000), 0, 0, 0, 0, 0);

			break;
	}
}

//Numhachi
void ActNpc337(NPCHAR *npc)
{
	RECT rcLeft[2];

	rcLeft[0] = {256, 112, 288, 152};
	rcLeft[1] = {288, 112, 320, 152};

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

//Green Devil
void ActNpc338(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {288, 0, 304, 16};
	rcLeft[1] = {304, 0, 320, 16};

	rcRight[0] = {288, 16, 304, 32};
	rcRight[1] = {304, 16, 320, 32};

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

			if (npc->tgt_y > npc->y)
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

			if (npc->x < 0 || npc->y < 0 || npc->x > gMap.width * 0x2000 || npc->y > gMap.length * 0x2000)
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

//Green Devil generator
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
