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

	unsigned char deg;
	switch (npc->act_no)
	{
		case 0:
			deg = npc->count1 + (npc->pNpc->count1 / 4);
			npc->act_no = 10;
			npc->x += npc->pNpc->x + npc->pNpc->tgt_x * GetCos(deg);
			npc->y += npc->pNpc->y + npc->pNpc->tgt_y * GetSin(deg);
			// Fallthrough
		case 10:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;

		case 100:
			deg = npc->count1 + (npc->pNpc->count1 / 4);
			npc->tgt_x = npc->pNpc->x + npc->pNpc->tgt_x * GetCos(deg);
			npc->tgt_y = npc->pNpc->y + npc->pNpc->tgt_y * GetSin(deg);
			npc->x += (npc->tgt_x - npc->x) / 8;
			npc->y += (npc->tgt_y - npc->y) / 8;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			break;

		case 1000:
			npc->act_no = 1001;
			npc->bits &= ~0x20;
			// Fallthrough
		case 1001:
			deg = npc->count1 + (npc->pNpc->count1 / 4);
			npc->tgt_x = npc->pNpc->x + npc->pNpc->tgt_x * GetCos(deg);
			npc->tgt_y = npc->pNpc->y + npc->pNpc->tgt_y * GetSin(deg);
			npc->x += (npc->tgt_x - npc->x) / 8;
			npc->y += (npc->tgt_y - npc->y) / 8;

			if (npc->pNpc->x < npc->x)
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
				const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y) + Random(-6, 6);
				const int ym = GetSin(deg);
				const int xm = GetCos(deg);

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
				const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y) + Random(-6, 6);
				const int ym = GetSin(deg);
				const int xm = GetCos(deg);

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
	switch (gBoss[0].act_no)
	{
		case 0:
			gBoss[0].cond = 0x80;
			gBoss[0].direct = 0;
			gBoss[0].act_no = 10;
			gBoss[0].exp = 0;
			gBoss[0].x = 0x14000;
			gBoss[0].y = 0x10000;
			gBoss[0].view.front = 0x1000;
			gBoss[0].view.top = 0x1000;
			gBoss[0].view.back = 0x10000;
			gBoss[0].view.bottom = 0x1000;
			gBoss[0].hit_voice = 54;
			gBoss[0].hit.front = 0x1000;
			gBoss[0].hit.top = 0x1000;
			gBoss[0].hit.back = 0x1000;
			gBoss[0].hit.bottom = 0x1000;
			gBoss[0].bits = 8;
			gBoss[0].bits |= 0x200;
			gBoss[0].size = 3;
			gBoss[0].damage = 0;
			gBoss[0].code_event = 1000;
			gBoss[0].life = 500;
			gBoss[0].count2 = Random(700, 1200);
			gBoss[0].tgt_x = 180;
			gBoss[0].tgt_y = 61;

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
			gBoss[2].cond = 0x90;
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
			gBoss[3].pNpc = gBoss;
			gBoss[3].damage = 10;

			gBoss[4] = gBoss[2];
			gBoss[4].pNpc = &gBoss[5];

			gBoss[5] = gBoss[3];
			gBoss[5].count1 = 128;
			break;

		case 20:
			if (--gBoss[0].tgt_x <= 112)
			{
				gBoss[0].act_no = 100;
				gBoss[0].act_wait = 0;
				gBoss[2].act_no = 100;
				gBoss[4].act_no = 100;
				gBoss[3].act_no = 100;
				gBoss[5].act_no = 100;
			}

			break;

		case 100:
			if (++gBoss[0].act_wait < 100)
			{
				++gBoss[0].count1;
			}
			else
			{
				if (gBoss[0].act_wait < 120)
				{
					gBoss[0].count1 += 2;
				}
				else
				{
					if (gBoss[0].act_wait < gBoss[0].count2)
					{
						gBoss[0].count1 += 4;
					}
					else
					{
						if (gBoss[0].act_wait < gBoss[0].count2 + 40)
						{
							gBoss[0].count1 += 2;
						}
						else
						{
							if (gBoss[0].act_wait < gBoss[0].count2 + 60)
							{
								++gBoss[0].count1;
							}
							else
							{
								gBoss[0].act_wait = 0;
								gBoss[0].act_no = 110;
								gBoss[0].count2 = Random(400, 700);
								break;
							}
						}
					}
				}
			}

			if (gBoss[0].count1 > 0x3FF)
				gBoss[0].count1 -= 0x400;

			break;

		case 110:
			if (++gBoss[0].act_wait < 20)
			{
				--gBoss[0].count1;
			}
			else
			{
				if (gBoss[0].act_wait < 60)
				{
					gBoss[0].count1 -= 2;
				}
				else
				{
					if (gBoss[0].act_wait < gBoss[0].count2)
					{
						gBoss[0].count1 -= 4;
					}
					else
					{
						if (gBoss[0].act_wait < gBoss[0].count2 + 40)
						{
							gBoss[0].count1 -= 2;
						}
						else
						{
							if (gBoss[0].act_wait < gBoss[0].count2 + 60)
							{
								--gBoss[0].count1;
							}
							else
							{
								if (gBoss[0].life < 300)
								{
									gBoss[0].act_wait = 0;
									gBoss[0].act_no = 400;
									gBoss[2].act_no = 400;
									gBoss[4].act_no = 400;
								}
								else
								{
									gBoss[0].act_wait = 0;
									gBoss[0].act_no = 100;
									gBoss[0].count2 = Random(400, 700);
								}

								break;
							}
						}
					}
				}
			}

			if (gBoss[0].count1 <= 0)
				gBoss[0].count1 += 0x400;

			break;

		case 400:
			if (++gBoss[0].act_wait > 100)
			{
				gBoss[0].act_wait = 0;
				gBoss[0].act_no = 401;
			}

			break;

		case 401:
			if (++gBoss[0].act_wait < 100)
			{
				++gBoss[0].count1;
			}
			else
			{
				if (gBoss[0].act_wait < 120)
				{
					gBoss[0].count1 += 2;
				}
				else
				{
					if (gBoss[0].act_wait < 500)
					{
						gBoss[0].count1 += 4;
					}
					else
					{
						if (gBoss[0].act_wait < 540)
						{
							gBoss[0].count1 += 2;
						}
						else
						{
							if (gBoss[0].act_wait < 560)
							{
								++gBoss[0].count1;
							}
							else
							{
								gBoss[0].act_no = 100;
								gBoss[0].act_wait = 0;
								gBoss[2].act_no = 100;
								gBoss[4].act_no = 100;
								break;
							}
						}
					}
				}
			}

			if ( gBoss[0].count1 > 0x3FF )
				gBoss[0].count1 -= 0x400;

			break;

		case 1000:
			gBoss[0].act_no = 1001;
			gBoss[0].act_wait = 0;
			gBoss[2].act_no = 1000;
			gBoss[3].act_no = 1000;
			gBoss[4].act_no = 1000;
			gBoss[5].act_no = 1000;
			SetDestroyNpChar(gBoss[0].x, gBoss[0].y, gBoss[0].view.back, 40);
			break;

		case 1001:
			if (++gBoss[0].act_wait > 100)
				gBoss[0].act_no = 1010;

			SetNpChar(4, gBoss[0].x + (Random(-0x80, 0x80) * 0x200), gBoss[0].y + (Random(-70, 70) * 0x200), 0, 0, 0, 0, 0x100);
			break;

		case 1010:
			gBoss[0].count1 += 4;

			if (gBoss[0].count1 > 0x3FF)
				gBoss[0].count1 -= 0x400;

			if (gBoss[0].tgt_x > 8)
				--gBoss[0].tgt_x;
			if (gBoss[0].tgt_y > 0)
				--gBoss[0].tgt_y;

			if (gBoss[0].tgt_x < -8)
				++gBoss[0].tgt_x;
			if (gBoss[0].tgt_y < 0)
				++gBoss[0].tgt_y;

			if (gBoss[0].tgt_y == 0)
			{
				gBoss[0].act_no = 1020;
				gBoss[0].act_wait = 0;
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
	gBoss[0].rect = rc;
}