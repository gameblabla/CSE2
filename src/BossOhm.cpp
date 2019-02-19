#include "BossOhm.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "Boss.h"
#include "Bullet.h"
#include "Flash.h"
#include "Frame.h"
#include "Game.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"

void ActBoss01_12()
{
	RECT rcLeft[1] = {80, 56, 104, 72};
	RECT rcRight[1] = {104, 56, 128, 72};

	for (int i = 1; i <= 2; i++)
	{
		gBoss[i].y = (gBoss[i].y + gBoss[i + 2].y - 0x1000) / 2;
		
		if (gBoss[i].direct)
		{
			gBoss[i].rect = rcRight[gBoss[i].count2];
			gBoss[i].x = gBoss[0].x + 0x2000;
		}
		else
		{
			gBoss[i].rect = rcLeft[gBoss[i].count2];
			gBoss[i].x = gBoss[0].x - 0x2000;
		}
	}
}

void ActBoss01_34()
{
	RECT rcLeft[2] = {
		{0, 56, 40, 88},
		{40, 56, 80, 88},
	};

	RECT rcRight[2] = {
		{ 0, 88, 40, 120 },
		{ 40, 88, 80, 120 },
	};

	for (int i = 3; i <= 4; i++)
	{
		switch (gBoss[i].act_no)
		{
			case 0:
				gBoss[i].act_no = 1;
				// Fallthrough
			case 1:
				if (i == 3)
					gBoss[i].x = gBoss[0].x - 0x2000;
				if (i == 4)
					gBoss[i].x = gBoss[0].x + 0x2000;

				gBoss[i].y = gBoss[0].y;
				break;

			case 3:
				if (i == 3)
					gBoss[i].x = gBoss[0].x - 0x2000;
				if (i == 4)
					gBoss[i].x = gBoss[0].x + 0x2000;

				gBoss[i].tgt_y = gBoss[0].y + 0x3000;
				gBoss[i].y += (gBoss[i].tgt_y - gBoss[i].y) / 2;
				break;

			default:
				break;
		}

		gBoss[i].count2 = !((gBoss[i].flag & 8) && gBoss[i].y > gBoss[i].tgt_y);

		if (gBoss[i].direct == 0)
			gBoss[i].rect = rcLeft[gBoss[i].count2];
		else
			gBoss[i].rect = rcRight[gBoss[i].count2];
	}
}

void ActBoss01_5()
{
	if (gBoss[5].act_no == 0)
	{
		gBoss[5].bits |= npc_solidSoft | npc_ignoreSolid;

		gBoss[5].hit.front = 0x2800;
		gBoss[5].hit.top = 0x4800;
		gBoss[5].hit.back = 0x2800;
		gBoss[5].hit.bottom = 0x2000;

		gBoss[5].act_no = 1;
	}
	
	if (gBoss[5].act_no == 1)
	{
		gBoss[5].x = gBoss[0].x;
		gBoss[5].y = gBoss[0].y;
	}
}

void ActBossChar_Omega()
{
	switch (gBoss[0].act_no)
	{
	case 0:
		gBoss[0].x = 0x1B6000;
		gBoss[0].y = 0x20000;

		gBoss[0].view.front = 0x5000;
		gBoss[0].view.top = 0x5000;
		gBoss[0].view.back = 0x5000;
		gBoss[0].view.bottom = 0x2000;

		gBoss[0].tgt_x = 0x1B6000;
		gBoss[0].tgt_y = 0x20000;

		gBoss[0].hit_voice = 52;

		gBoss[0].hit.front = 0x1000;
		gBoss[0].hit.top = 0x3000;
		gBoss[0].hit.back = 0x1000;
		gBoss[0].hit.bottom = 0x2000;

		gBoss[0].bits = (npc_ignoreSolid | npc_eventDie | npc_showDamage);
		gBoss[0].size = 3;
		gBoss[0].exp = 1;
		gBoss[0].code_event = 210;
		gBoss[0].life = 400;

		gBoss[1].cond = 0x80;

		gBoss[1].view.front = 0x1800;
		gBoss[1].view.top = 0x1000;
		gBoss[1].view.back = 0x1800;
		gBoss[1].view.bottom = 0x1000;

		gBoss[1].bits = npc_ignoreSolid;

		memcpy(&gBoss[2], &gBoss[1], sizeof(gBoss[2]));

		gBoss[1].direct = 0;
		gBoss[2].direct = 2;

		gBoss[3].cond = 0x80;

		gBoss[3].view.front = 0x3000;
		gBoss[3].view.top = 0x2000;
		gBoss[3].view.back = 0x2000;
		gBoss[3].view.bottom = 0x2000;

		gBoss[3].hit_voice = 52;

		gBoss[3].hit.front = 0x1000;
		gBoss[3].hit.top = 0x1000;
		gBoss[3].hit.back = 0x1000;
		gBoss[3].hit.bottom = 0x1000;

		gBoss[3].bits = npc_ignoreSolid;

		gBoss[3].y = gBoss[0].y;
		gBoss[3].direct = 0;

		memcpy(&gBoss[4], &gBoss[3], sizeof(gBoss[4]));
		gBoss[4].direct = 2;
		gBoss[3].x = gBoss[0].x + 0x2000;
		gBoss[5].cond = 0x80;
		break;

	case 20: //Rising out of the ground
		gBoss[0].act_no = 30;
		gBoss[0].act_wait = 0;
		gBoss[0].ani_no = 0;
		// Fallthrough
	case 30:
		SetQuake(2);
		gBoss[0].y -= 0x200;

		if (!(++gBoss[0].act_wait & 3))
			PlaySoundObject(26, 1);

		if (gBoss[0].act_wait >= 48)
		{
			gBoss[0].act_no = 40;
			gBoss[0].act_wait = 0;

			if (gBoss[0].life <= 280)
			{
				gBoss[0].act_no = 110;

				gBoss[0].bits |= npc_shootable;
				gBoss[0].bits &= ~npc_ignoreSolid;
				gBoss[3].bits &= ~npc_ignoreSolid;
				gBoss[4].bits &= ~npc_ignoreSolid;

				gBoss[3].act_no = 3;
				gBoss[4].act_no = 3;
				gBoss[5].hit.top = 0x2000;
			}
		}
		break;

	case 40:
		if (++gBoss[0].act_wait >= 48)
		{
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 50;
			gBoss[0].count1 = 0;
			gBoss[5].hit.top = 0x2000;
			PlaySoundObject(102, 1);
		}
		break;

	case 50: //Open mouth
		if (++gBoss[0].count1 > 2)
		{
			gBoss[0].count1 = 0;
			++gBoss[0].count2;
		}

		if (gBoss[0].count2 == 3)
		{
			gBoss[0].act_no = 60;
			gBoss[0].act_wait = 0;
			gBoss[0].bits |= npc_shootable;
			gBoss[0].hit.front = 0x2000;
			gBoss[0].hit.back = 0x2000;
		}
		break;

	case 60: //Shoot out of mouth
		if (++gBoss[0].act_wait > 20 && gBoss[0].act_wait < 80 && !(gBoss[0].act_wait % 3))
		{
			if (Random(0, 9) <= 7)
				SetNpChar(48, gBoss[0].x, gBoss[0].y - 0x2000, Random(-0x100, 0x100), -0x333, 0, NULL, 0x100);
			else
				SetNpChar(48, gBoss[0].x, gBoss[0].y - 0x2000, Random(-0x100, 0x100), -0x333, 2, NULL, 0x100);

			PlaySoundObject(39, 1);
		}

		if (gBoss[0].act_wait >= 200 || CountArmsBullet(6))
		{
			gBoss[0].count1 = 0;
			gBoss[0].act_no = 70;
			PlaySoundObject(102, 1);
		}
		break;

	case 70: //Close mouth
		if (++gBoss[0].count1 > 2)
		{
			gBoss[0].count1 = 0;
			--gBoss[0].count2;
		}

		if (gBoss[0].count2 == 1)
			gBoss[0].damage = 20;

		if (!gBoss[0].count2)
		{
			PlaySoundObject(102, 1);
			PlaySoundObject(12, 1);

			gBoss[0].act_no = 80;
			gBoss[0].act_wait = 0;

			gBoss[0].bits &= ~npc_shootable;

			gBoss[0].hit.front = 0x3000;
			gBoss[0].hit.back = 0x3000;
			gBoss[5].hit.top = 0x4800;

			gBoss[0].damage = 0;
		}
		break;

	case 80:
		if (++gBoss[0].act_wait >= 48)
		{
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 90;
		}
		break;

	case 90: //Go back into the ground
		SetQuake(2);
		gBoss[0].y += 0x200;

		if (!(++gBoss[0].act_wait & 3))
			PlaySoundObject(26, 1);

		if (gBoss[0].act_wait >= 48)
		{
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 100;
		}
		break;

	case 100: //Move to proper position for coming out of the ground
		if (++gBoss[0].act_wait >= 120)
		{
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 30;

			gBoss[0].x = gBoss[0].tgt_x + (Random(-64, 64) << 9);
			gBoss[0].y = gBoss[0].tgt_y;
		}
		break;

	case 110:
		if (++gBoss[0].count1 > 2)
		{
			gBoss[0].count1 = 0;
			++gBoss[0].count2;
		}

		if (gBoss[0].count2 == 3)
		{
			gBoss[0].act_no = 120;
			gBoss[0].act_wait = 0;
			gBoss[0].hit.front = 0x2000;
			gBoss[0].hit.back = 0x2000;
		}
		break;

	case 120:
		if (++gBoss[0].act_wait >= 50 || CountArmsBullet(6))
		{
			gBoss[0].act_no = 130;
			PlaySoundObject(102, 1);
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;
		}

		if (gBoss[0].act_wait < 30 && !(gBoss[0].act_wait % 5))
		{
			SetNpChar(48, gBoss[0].x, gBoss[0].y - 0x2000, Random(-0x155, 0x155), -0x333, 0, NULL, 0x100);
			PlaySoundObject(39, 1);
		}
		break;

	case 130:
		if (++gBoss[0].count1 > 2)
		{
			gBoss[0].count1 = 0;
			--gBoss[0].count2;
		}

		if (gBoss[0].count2 == 1)
			gBoss[0].damage = 20;

		if (!gBoss[0].count2)
		{
			gBoss[0].act_no = 140;
			gBoss[0].bits |= npc_shootable;

			gBoss[0].hit.front = 0x2000;
			gBoss[0].hit.back = 0x2000;

			gBoss[0].ym = -0x5FF;

			PlaySoundObject(102, 1);
			PlaySoundObject(12, 1);
			PlaySoundObject(25, 1);

			if (gBoss[0].x < gMC.x)
				gBoss[0].xm = 0x100;
			if (gBoss[0].x > gMC.x)
				gBoss[0].xm = -0x100;

			gBoss[0].damage = 0;
			gBoss[5].hit.top = 0x4800;
		}
		break;

	case 140:
		if (gMC.flag & 8 && gBoss[0].ym > 0)
			gBoss[5].damage = 20;
		else
			gBoss[5].damage = 0;

		gBoss[0].ym += 0x24;
		if (gBoss[0].ym > 0x5FF)
			gBoss[0].ym = 0x5FF;

		gBoss[0].x += gBoss[0].xm;
		gBoss[0].y += gBoss[0].ym;

		if (gBoss[0].flag & 8)
		{
			gBoss[0].act_no = 110;
			gBoss[0].act_wait = 0;
			gBoss[0].count1 = 0;

			gBoss[5].hit.top = 0x2000;
			gBoss[5].damage = 0;

			PlaySoundObject(26, 1);
			PlaySoundObject(12, 1);

			SetQuake(30);
		}
		break;

	case 150:
		SetQuake(2);

		if (!(++gBoss[0].act_wait % 12))
			PlaySoundObject(52, 1);

		SetDestroyNpChar(gBoss[0].x + (Random(-48, 48) << 9), gBoss[0].y + (Random(-48, 24) << 9), 1, 1);

		if (gBoss[0].act_wait > 100)
		{
			gBoss[0].act_wait = 0;
			gBoss[0].act_no = 160;
			SetFlash(gBoss[0].x, gBoss[0].y, 1);
			PlaySoundObject(35, 1);
		}
		break;

	case 160:
		SetQuake(40);

		if (++gBoss[0].act_wait > 50)
		{
			gBoss[0].cond = 0;
			gBoss[1].cond = 0;
			gBoss[2].cond = 0;
			gBoss[3].cond = 0;
			gBoss[4].cond = 0;
			gBoss[5].cond = 0;
		}
		break;

	default:
		break;
	}

	RECT rect[4] = {
		{0, 0, 80, 56},
		{80, 0, 160, 56},
		{160, 0, 240, 56},
		{80, 0, 160, 56},
	};

	gBoss[0].rect = rect[gBoss[0].count2];

	gBoss[1].shock = gBoss[0].shock;
	gBoss[2].shock = gBoss[0].shock;
	gBoss[3].shock = gBoss[0].shock;
	gBoss[4].shock = gBoss[0].shock;

	ActBoss01_34();
	ActBoss01_12();
	ActBoss01_5();

	if (!gBoss[0].life && gBoss[0].act_no < 150)
	{
		gBoss[0].act_no = 150;
		gBoss[0].act_wait = 0;
		gBoss[0].damage = 0;
		gBoss[5].damage = 0;

		for (int i = 0; i < NPC_MAX; i++)
		{
			if (gNPC[i].cond & 0x80)
			{
				if (gNPC[i].code_char == 48)
					gNPC[i].cond = 0;
			}
		}
	}
}
