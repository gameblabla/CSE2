#include "BossTwinD.h"

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Flash.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

static void ActBossCharT_DragonBody(NPCHAR *npc)
{
	unsigned char deg;

	RECT rcLeft[3] = {
		{0, 0, 40, 40},
		{40, 0, 80, 40},
		{80, 0, 120, 40},
	};

	RECT rcRight[3] = {
		{0, 40, 40, 80},
		{40, 40, 80, 80},
		{80, 40, 120, 80},
	};

	switch (npc->act_no)
	{
		case 0:
			deg = ((npc->pNpc->count1 / 4) + npc->count1) % 0x100;
			npc->act_no = 10;
			npc->x += npc->pNpc->x + GetCos(deg) * npc->pNpc->tgt_x;
			npc->y += npc->pNpc->y + GetSin(deg) * npc->pNpc->tgt_y;
			// Fallthrough
		case 10:
			if (npc->x > gMC.x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;

		case 100:
			deg = ((npc->pNpc->count1 / 4) + npc->count1) % 0x100;
			npc->tgt_x = npc->pNpc->x + GetCos(deg) * npc->pNpc->tgt_x;
			npc->tgt_y = npc->pNpc->y + GetSin(deg) * npc->pNpc->tgt_y;
			npc->x += (npc->tgt_x - npc->x) / 8;
			npc->y += (npc->tgt_y - npc->y) / 8;

			if (npc->x > gMC.x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;

		case 1000:
			npc->act_no = 1001;
			npc->bits &= ~0x20;
			// Fallthrough
		case 1001:
			deg = ((npc->pNpc->count1 / 4) + npc->count1) % 0x100;
			npc->tgt_x = npc->pNpc->x + GetCos(deg) * npc->pNpc->tgt_x;
			npc->tgt_y = npc->pNpc->y + GetSin(deg) * npc->pNpc->tgt_y;
			npc->x += (npc->tgt_x - npc->x) / 8;
			npc->y += (npc->tgt_y - npc->y) / 8;

			if (npc->x > npc->pNpc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;
	}

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

static void ActBossCharT_DragonHead(NPCHAR *npc)
{
	unsigned char deg;
	int ym;
	int xm;

	RECT rcLeft[4] = {
		{0, 80, 40, 112},
		{40, 80, 80, 112},
		{80, 80, 120, 112},
		{120, 80, 160, 112},
	};

	RECT rcRight[4] = {
		{0, 112, 40, 144},
		{40, 112, 80, 144},
		{80, 112, 120, 144},
		{120, 112, 160, 144},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			break;

		case 100:
			npc->act_no = 200;
			// Fallthrough
		case 200:
			npc->bits &= ~0x20;
			npc->ani_no = 0;
			npc->hit.front = 0x2000;
			npc->act_no = 201;
			npc->count1 = Random(100, 200);
			// Fallthrough
		case 201:
			if (npc->count1)
			{
				--npc->count1;
			}
			else
			{
				npc->act_no = 210;
				npc->act_wait = 0;
				npc->count2 = 0;
			}

			break;

		case 210:
			if (++npc->act_wait == 3)
				npc->ani_no = 1;

			if (npc->act_wait == 6)
			{
				npc->ani_no = 2;
				npc->hit.front = 0x1000;
				npc->bits |= 0x20;
				npc->count2 = 0;
			}

			if (npc->act_wait > 150)
			{
				npc->act_no = 220;
				npc->act_wait = 0;
			}

			if (npc->shock)
				++npc->count2;

			if (npc->count2 > 10)
			{
				PlaySoundObject(51, 1);
				SetDestroyNpChar(npc->x, npc->y, npc->view.back, 4);
				npc->act_no = 300;
				npc->act_wait = 0;
				npc->ani_no = 3;
				npc->hit.front = 0x2000;
			}

			break;

		case 220:
			if (++npc->act_wait % 8 == 1)
			{
				deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
				deg += (unsigned char)Random(-6, 6);
				ym = GetSin(deg);
				xm = GetCos(deg);

				if (npc->direct == 0)
					SetNpChar(202, npc->x - 0x1000, npc->y, xm, ym, 0, 0, 0x100);
				else
					SetNpChar(202, npc->x + 0x1000, npc->y, xm, ym, 0, 0, 0x100);

				PlaySoundObject(33, 1);
			}

			if (npc->act_wait > 50)
				npc->act_no = 200;

			break;

		case 300:
			if (++npc->act_wait > 100)
				npc->act_no = 200;

			break;

		case 400:
			npc->act_no = 401;
			npc->act_wait = 0;
			npc->ani_no = 0;
			npc->hit.front = 0x2000;
			npc->bits &= ~0x20;
			// Fallthrough
		case 401:
			if (++npc->act_wait == 3)
				npc->ani_no = 1;

			if (npc->act_wait == 6)
			{
				npc->ani_no = 2;
				npc->hit.front = 0x1000;
				npc->bits |= 0x20;
				npc->count2 = 0;
			}

			if (npc->act_wait > 20 && npc->act_wait % 32 == 1)
			{
				deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
				deg += (unsigned char)Random(-6, 6);
				ym = GetSin(deg);
				xm = GetCos(deg);

				if (npc->direct == 0)
					SetNpChar(202, npc->x - 0x1000, npc->y, xm, ym, 0, 0, 0x100);
				else
					SetNpChar(202, npc->x + 0x1000, npc->y, xm, ym, 0, 0, 0x100);

				PlaySoundObject(33, 1);
			}

			break;

		case 1000:
			npc->bits &= ~0x20;
			npc->ani_no = 3;
			break;
	}

	npc->direct = npc->pNpc->direct;

	if (npc->direct == 0)
		npc->x = npc->pNpc->x - 0x800;
	else
		npc->x = npc->pNpc->x + 0x800;

	npc->y = npc->pNpc->y - 0x1000;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

void ActBossChar_Twin(void)
{
	NPCHAR *npc = gBoss;

	switch (npc->act_no)
	{
		case 0:
			npc->cond = 0x80;
			npc->direct = 0;
			npc->act_no = 10;
			npc->exp = 0;
			npc->x = 0x14000;
			npc->y = 0x10000;
			npc->view.front = 0x1000;
			npc->view.top = 0x1000;
			npc->view.back = 0x10000;
			npc->view.bottom = 0x1000;
			npc->hit_voice = 54;
			npc->hit.front = 0x1000;
			npc->hit.top = 0x1000;
			npc->hit.back = 0x1000;
			npc->hit.bottom = 0x1000;
			npc->bits = 8;
			npc->bits |= 0x200;
			npc->size = 3;
			npc->damage = 0;
			npc->code_event = 1000;
			npc->life = 500;
			npc->count2 = Random(700, 1200);
			npc->tgt_x = 180;
			npc->tgt_y = 61;

			gBoss[2].cond = 0x80;
			gBoss[2].view.back = 0x2800;
			gBoss[2].view.front = 0x2800;
			gBoss[2].view.top = 0x2000;
			gBoss[2].view.bottom = 0x2000;
			gBoss[2].hit.back = 0x1800;
			gBoss[2].hit.front = 0x1800;
			gBoss[2].hit.top = 0x1400;
			gBoss[2].hit.bottom = 0x1400;
			gBoss[2].bits = 12;
			gBoss[2].pNpc = &gBoss[3];
			gBoss[2].cond |= 0x10;
			gBoss[2].damage = 10;

			gBoss[3].cond = 0x80;
			gBoss[3].view.back = 0x2800;
			gBoss[3].view.front = 0x2800;
			gBoss[3].view.top = 0x2800;
			gBoss[3].view.bottom = 0x2800;
			gBoss[3].hit.back = 0x1800;
			gBoss[3].hit.front = 0x1800;
			gBoss[3].hit.top = 0x400;
			gBoss[3].hit.bottom = 0x2000;
			gBoss[3].bits = 8;
			gBoss[3].pNpc = npc;
			gBoss[3].damage = 10;

			gBoss[4] = gBoss[2];
			gBoss[4].pNpc = &gBoss[5];

			gBoss[5] = gBoss[3];
			gBoss[5].count1 = 128;
			break;

		case 20:
			if (--npc->tgt_x <= 112)
			{
				npc->act_no = 100;
				npc->act_wait = 0;
				gBoss[2].act_no = 100;
				gBoss[4].act_no = 100;
				gBoss[3].act_no = 100;
				gBoss[5].act_no = 100;
			}

			break;

		case 100:
			if (++npc->act_wait < 100)
			{
				++npc->count1;
			}
			else
			{
				if (npc->act_wait < 120)
				{
					npc->count1 += 2;
				}
				else
				{
					if (npc->act_wait < npc->count2)
					{
						npc->count1 += 4;
					}
					else
					{
						if (npc->act_wait < npc->count2 + 40)
						{
							npc->count1 += 2;
						}
						else
						{
							if (npc->act_wait < npc->count2 + 60)
							{
								++npc->count1;
							}
							else
							{
								npc->act_wait = 0;
								npc->act_no = 110;
								npc->count2 = Random(400, 700);
								break;
							}
						}
					}
				}
			}

			if (npc->count1 > 0x3FF)
				npc->count1 -= 0x400;

			break;

		case 110:
			if (++npc->act_wait < 20)
			{
				--npc->count1;
			}
			else
			{
				if (npc->act_wait < 60)
				{
					npc->count1 -= 2;
				}
				else
				{
					if (npc->act_wait < npc->count2)
					{
						npc->count1 -= 4;
					}
					else
					{
						if (npc->act_wait < npc->count2 + 40)
						{
							npc->count1 -= 2;
						}
						else
						{
							if (npc->act_wait < npc->count2 + 60)
							{
								--npc->count1;
							}
							else
							{
								if (npc->life < 300)
								{
									npc->act_wait = 0;
									npc->act_no = 400;
									gBoss[2].act_no = 400;
									gBoss[4].act_no = 400;
								}
								else
								{
									npc->act_wait = 0;
									npc->act_no = 100;
									npc->count2 = Random(400, 700);
								}

								break;
							}
						}
					}
				}
			}

			if (npc->count1 <= 0)
				npc->count1 += 0x400;

			break;

		case 400:
			if (++npc->act_wait > 100)
			{
				npc->act_wait = 0;
				npc->act_no = 401;
			}

			break;

		case 401:
			if (++npc->act_wait < 100)
			{
				++npc->count1;
			}
			else
			{
				if (npc->act_wait < 120)
				{
					npc->count1 += 2;
				}
				else
				{
					if (npc->act_wait < 500)
					{
						npc->count1 += 4;
					}
					else
					{
						if (npc->act_wait < 540)
						{
							npc->count1 += 2;
						}
						else
						{
							if (npc->act_wait < 560)
							{
								++npc->count1;
							}
							else
							{
								npc->act_no = 100;
								npc->act_wait = 0;
								gBoss[2].act_no = 100;
								gBoss[4].act_no = 100;
								break;
							}
						}
					}
				}
			}

			if (npc->count1 > 0x3FF)
				npc->count1 -= 0x400;

			break;

		case 1000:
			npc->act_no = 1001;
			npc->act_wait = 0;
			gBoss[2].act_no = 1000;
			gBoss[3].act_no = 1000;
			gBoss[4].act_no = 1000;
			gBoss[5].act_no = 1000;
			SetDestroyNpChar(npc->x, npc->y, npc->view.back, 40);
			// Fallthrough

		case 1001:
			if (++npc->act_wait > 100)
				npc->act_no = 1010;

			SetNpChar(4, npc->x + (Random(-0x80, 0x80) * 0x200), npc->y + (Random(-70, 70) * 0x200), 0, 0, 0, 0, 0x100);
			break;

		case 1010:
			npc->count1 += 4;

			if (npc->count1 > 0x3FF)
				npc->count1 -= 0x400;

			if (npc->tgt_x > 8)
				--npc->tgt_x;
			if (npc->tgt_y > 0)
				--npc->tgt_y;

			if (npc->tgt_x < -8)
				++npc->tgt_x;
			if (npc->tgt_y < 0)
				++npc->tgt_y;

			if (npc->tgt_y == 0)
			{
				npc->act_no = 1020;
				npc->act_wait = 0;
				SetFlash(gBoss[0].x, gBoss[0].y, 1);
				PlaySoundObject(35, 1);
			}

			break;

		case 1020:
			if (++gBoss[0].act_wait > 50)
			{
				DeleteNpCharCode(211, 1);
				gBoss[0].cond = 0;
				gBoss[1].cond = 0;
				gBoss[2].cond = 0;
				gBoss[3].cond = 0;
				gBoss[4].cond = 0;
				gBoss[5].cond = 0;
				gBoss[0].act_no = 0;
			}

			break;
	}

	ActBossCharT_DragonHead(&gBoss[2]);
	ActBossCharT_DragonBody(&gBoss[3]);
	ActBossCharT_DragonHead(&gBoss[4]);
	ActBossCharT_DragonBody(&gBoss[5]);

	RECT rc = {0, 0, 0, 0};
	npc->rect = rc;
}
