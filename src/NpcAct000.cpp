#include "NpcAct.h"

#include "WindowsWrapper.h"

#include "Back.h"
#include "Caret.h"
#include "Frame.h"
#include "Game.h"
#include "Map.h"
#include "MyChar.h"
#include "NpChar.h"
#include "Sound.h"
#include "Triangle.h"

//Null
void ActNpc000(NPCHAR *npc)
{
	RECT rect = {0x00, 0x00, 0x10, 0x10};

	if (npc->act_no == 0)
	{
		npc->act_no = 1;

		if (npc->direct == 2)
			npc->y += 0x2000;
	}

	npc->rect = rect;
}

//Experience
void ActNpc001(NPCHAR *npc)
{
	//In wind
	if (gBack.type == 5 || gBack.type == 6)
	{
		if (npc->act_no == 0)
		{
			//Set state
			npc->act_no = 1;

			//Set random speed
			npc->ym = Random(-0x80, 0x80);
			npc->xm = Random(0x7F, 0x100);
		}

		//Blow to the left
		npc->xm -= 8;

		//Destroy when off-screen
		if (npc->x < 0xA000)
			npc->cond = 0;

#ifdef FIX_BUGS
		//Limit speed
		if (npc->xm < -0x600)
			npc->xm = -0x600;
#else
		//Limit speed (except pixel applied it to the X position)
		if (npc->x < -0x600)
			npc->x = -0x600;
#endif

		//Bounce off walls
		if (npc->flag & 1)
			npc->xm = 0x100;
		if (npc->flag & 2)
			npc->ym = 0x40;
		if (npc->flag & 8)
			npc->ym = -0x40;
	}
	//When not in wind
	else
	{
		if (npc->act_no == 0)
		{
			//Set state
			npc->act_no = 1;
			npc->ani_no = Random(0, 4);

			//Random speed
			npc->xm = Random(-0x200, 0x200);
			npc->ym = Random(-0x400, 0);

			//Random direction (reverse animation or not)
			if (Random(0, 1) != 0)
				npc->direct = 0;
			else
				npc->direct = 2;
		}

		//Gravity
		if (npc->flag & 0x100)
			npc->ym += 21;
		else
			npc->ym += 42;

		//Bounce off walls
		if (npc->flag & 1 && npc->xm < 0)
			npc->xm *= -1;
		if (npc->flag & 4 && npc->xm > 0)
			npc->xm *= -1;

		//Bounce off ceiling
		if (npc->flag & 2 && npc->ym < 0)
			npc->ym *= -1;

		//Bounce off floor
		if (npc->flag & 8)
		{
			PlaySoundObject(45, 1);
			npc->ym = -0x280;
			npc->xm = 2 * npc->xm / 3;
		}

		//Play bounce song (and try to clip out of floor if stuck)
		if (npc->flag & 0xD)
		{
			PlaySoundObject(45, 1);
			if (++npc->count2 > 2)
				npc->y -= 0x200;
		}
		else
		{
			npc->count2 = 0;
		}

		//Limit speed
		if (npc->xm < -0x5FF)
			npc->xm = -0x5FF;
		if (npc->xm > 0x5FF)
			npc->xm = 0x5FF;
		if (npc->ym < -0x5FF)
			npc->ym = -0x5FF;
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;
	}

	//Move
	npc->y += npc->ym;
	npc->x += npc->xm;

	//Get framerects
	RECT rect[6] = {
		{0x00, 0x10, 0x10, 0x20},
		{0x10, 0x10, 0x20, 0x20},
		{0x20, 0x10, 0x30, 0x20},
		{0x30, 0x10, 0x40, 0x20},
		{0x40, 0x10, 0x50, 0x20},
		{0x50, 0x10, 0x60, 0x20},
	};

	RECT rcNo = {0, 0, 0, 0};

	//Animate
	++npc->ani_wait;

	if (npc->direct == 0)
	{
		if (npc->ani_wait > 2)
		{
			npc->ani_wait = 0;
			if (++npc->ani_no > 5)
				npc->ani_no = 0;
		}
	}
	else
	{
		if (npc->ani_wait > 2)
		{
			npc->ani_wait = 0;
			if (--npc->ani_no < 0)
				npc->ani_no = 5;
		}
	}

	npc->rect = rect[npc->ani_no];

	//Size
	if (npc->act_no)
	{
		switch (npc->exp)
		{
			case 5:
				npc->rect.top += 16;
				npc->rect.bottom += 16;
				break;

			case 20:
				npc->rect.top += 32;
				npc->rect.bottom += 32;
				break;
		}

		npc->act_no = 1;
	}

	//Delete after 500 frames
	if (++npc->count1 > 500 && npc->ani_no == 5 && npc->ani_wait == 2)
		npc->cond = 0;

	//Blink after 400 frames
	if (npc->count1 > 400)
	{
		if (npc->count1 / 2 % 2)
			npc->rect = rcNo;
	}
}

//Behemoth
void ActNpc002(NPCHAR *npc)
{
	//Rects
	RECT rcLeft[7] = {
		{32, 0, 64, 24},
		{0, 0, 32, 24},
		{32, 0, 64, 24},
		{64, 0, 96, 24},
		{96, 0, 128, 24},
		{128, 0, 160, 24},
		{160, 0, 192, 24},
	};

	RECT rcRight[7] = {
		{32, 24, 64, 48},
		{0, 24, 32, 48},
		{32, 24, 64, 48},
		{64, 24, 96, 48},
		{96, 24, 128, 48},
		{128, 24, 160, 48},
		{160, 24, 192, 48},
	};

	//Turn when touching a wall
	if (npc->flag & 1)
		npc->direct = 2;
	else if (npc->flag & 4)
		npc->direct = 0;

	switch (npc->act_no)
	{
		case 0: //Walking
			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			if (++npc->ani_wait > 8)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 3)
				npc->ani_no = 0;

			if (npc->shock)
			{
				npc->count1 = 0;
				npc->act_no = 1;
				npc->ani_no = 4;
			}
			break;
		case 1: //Shot
			npc->xm = 7 * npc->xm / 8;

			if (++npc->count1 > 40)
			{
				if (npc->shock)
				{
					npc->count1 = 0;
					npc->act_no = 2;
					npc->ani_no = 6;
					npc->ani_wait = 0;
					npc->damage = 5;
				}
				else
				{
					npc->act_no = 0;
					npc->ani_wait = 0;
				}
			}
			break;
		case 2: //Charge
			if (npc->direct == 0)
				npc->xm = -0x400;
			else
				npc->xm = 0x400;

			if (++npc->count1 > 200)
			{
				npc->act_no = 0;
				npc->damage = 1;
			}

			if (++npc->ani_wait > 5)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 6)
			{
				npc->ani_no = 5;
				// These three lines are missing in the Linux port. Could this be because it's based on an older version?
				PlaySoundObject(26, 1);
				SetNpChar(4, npc->x, npc->y + 0x600, 0, 0, 0, 0, 0x100);
				SetQuake(8);
			}
			break;
	}

	//Gravity
	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	//Move
	npc->x += npc->xm;
	npc->y += npc->ym;

	//Set framerect
	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Dead enemy (make sure damage shown doesn't teleport to a new loaded npc)
void ActNpc003(NPCHAR *npc)
{
	if (++npc->count1 > 100)
		npc->cond = 0;

	RECT rect = {0, 0, 0, 0};
	npc->rect = rect;
}

//Smoke
void ActNpc004(NPCHAR *npc)
{
	RECT rcLeft[8] = {
		{16, 0, 17, 1},
		{16, 0, 32, 16},
		{32, 0, 48, 16},
		{48, 0, 64, 16},
		{64, 0, 80, 16},
		{80, 0, 96, 16},
		{96, 0, 112, 16},
		{112, 0, 128, 16},
	};

	RECT rcUp[8] = {
		{16, 0, 17, 1},
		{80, 48, 96, 64},
		{0, 128, 16, 144},
		{16, 128, 32, 144},
		{32, 128, 48, 144},
		{48, 128, 64, 144},
		{64, 128, 80, 144},
		{80, 128, 96, 144},
	};

	unsigned char deg;

	if (npc->act_no == 0)
	{
		//Move in random direction at random speed
		if (npc->direct == 0 || npc->direct == 1)
		{
			deg = Random(0, 0xFF);
			npc->xm = GetCos(deg) * Random(0x200, 0x5FF) / 0x200;
			npc->ym = GetSin(deg) * Random(0x200, 0x5FF) / 0x200;
		}

		//Set state
		npc->ani_no = Random(0, 4);
		npc->ani_wait = Random(0, 3);
		npc->act_no = 1;
	}
	else
	{
		//Slight drag
		npc->xm = 20 * npc->xm / 21;
		npc->ym = 20 * npc->ym / 21;

		//Move
		npc->x += npc->xm;
		npc->y += npc->ym;
	}

	//Animate
	if (++npc->ani_wait > 4)
	{
		npc->ani_wait = 0;
		npc->ani_no++;
	}

	//Set framerect
	if (npc->ani_no > 7)
	{
		//Destroy if over
		npc->cond = 0;
	}
	else
	{
		if (npc->direct == 1)
			npc->rect = rcUp[npc->ani_no];
		if (npc->direct == 0)
			npc->rect = rcLeft[npc->ani_no];
		if (npc->direct == 2)
			npc->rect = rcLeft[npc->ani_no];
	}
}

//Critter (Green, Egg Corridor)
void ActNpc005(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{0, 48, 16, 64},
		{16, 48, 32, 64},
		{32, 48, 48, 64},
	};

	RECT rcRight[3] = {
		{0, 64, 16, 80},
		{16, 64, 32, 80},
		{32, 64, 48, 80},
	};

	switch (npc->act_no)
	{
		case 0: //Init
			npc->y += 0x600;
			npc->act_no = 1;
			// Fallthrough
		case 1: //Waiting
			//Look at player
			if (npc->x > gMC.x)
				npc->direct = 0;
			else
				npc->direct = 2;

			//Open eyes near player
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

			//Jump if attacked
			if (npc->shock)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}

			//Jump if player is nearby
			if (npc->act_wait >= 8 && npc->x - 0x6000 < gMC.x && npc->x + 0x6000 > gMC.x && npc->y - 0xA000 < gMC.y && npc->y + 0x6000 > gMC.y)
			{
				npc->act_no = 2;
				npc->ani_no = 0;
				npc->act_wait = 0;
			}
			break;

		case 2: //Going to jump
			if (++npc->act_wait > 8)
			{
				//Set jump state
				npc->act_no = 3;
				npc->ani_no = 2;

				//Jump
				npc->ym = -0x5FF;
				PlaySoundObject(30, 1);

				//Jump in facing direction
				if (npc->direct == 0)
					npc->xm = -0x100;
				else
					npc->xm = 0x100;
			}
			break;

		case 3: //Jumping
			//Land
			if (npc->flag & 8)
			{
				npc->xm = 0;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->act_no = 1;
				PlaySoundObject(23, 1);
			}
			break;
	}

	//Gravity
	npc->ym += 64;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	//Move
	npc->x += npc->xm;
	npc->y += npc->ym;

	//Set framerect
	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Beetle (Goes left and right, Egg Corridor)
void ActNpc006(NPCHAR *npc)
{
	RECT rcLeft[5] = {
		{0, 80, 16, 96},
		{16, 80, 32, 96},
		{32, 80, 48, 96},
		{48, 80, 64, 96},
		{64, 80, 80, 96},
	};

	RECT rcRight[5] = {
		{0, 96, 16, 112},
		{16, 96, 32, 112},
		{32, 96, 48, 112},
		{48, 96, 64, 112},
		{64, 96, 80, 112},
	};

	switch (npc->act_no)
	{
		case 0: //Init
			npc->act_no = 1;
			
			if (npc->direct == 0)
				npc->act_no = 1;
			else
				npc->act_no = 3;
			break;
		
		case 1:
			//Accelerate to the left
			npc->xm -= 0x10;
			if (npc->xm < -0x400)
				npc->xm = -0x400;
			
			//Move
			if (npc->shock)
				npc->x += npc->xm / 2;
			else
				npc->x += npc->xm;
			
			//Animate
			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}
			
			if (npc->ani_no > 2)
				npc->ani_no = 1;
			
			//Stop when hitting a wall
			if (npc->flag & 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->xm = 0;
				npc->direct = 2;
			}
			break;
			
		case 2:
			//Wait 60 frames then move to the right
			if (++npc->act_wait > 60)
			{
				npc->act_no = 3;
				npc->ani_wait = 0;
				npc->ani_no = 1;
			}
			break;
			
		case 3:
			//Accelerate to the right
			npc->xm += 0x10;
			if (npc->xm > 0x400)
				npc->xm = 0x400;
			
			//Move
			if (npc->shock)
				npc->x += npc->xm / 2;
			else
				npc->x += npc->xm;
			
			//Animate
			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}
			
			if (npc->ani_no > 2)
				npc->ani_no = 1;
			
			//Stop when hitting a wall
			if (npc->flag & 4)
			{
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->ani_no = 0;
				npc->xm = 0;
				npc->direct = 0;
			}
			break;
		
		case 4:
			//Wait 60 frames then move to the left
			if (++npc->act_wait > 60)
			{
				npc->act_no = 1;
				npc->ani_wait = 0;
				npc->ani_no = 1;
			}
			break;
	}
	
	//Set framerect
	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Basil
void ActNpc007(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{256, 64, 288, 80},
		{256, 80, 288, 96},
		{256, 96, 288, 112},
	};

	RECT rcRight[3] = {
		{288, 64, 320, 80},
		{288, 80, 320, 96},
		{288, 96, 320, 112},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->x = gMC.x;

			if (npc->direct == 0)
				npc->act_no = 1;
			else
				npc->act_no = 2;

			break;

		case 1:
			npc->xm -= 0x40;

			if (npc->x < gMC.x - 0x18000)
				npc->act_no = 2;

			if (npc->flag & 1)
			{
				npc->xm = 0;
				npc->act_no = 2;
			}

			break;

		case 2:
			npc->xm += 0x40;

			if (npc->x > gMC.x + 0x18000)
				npc->act_no = 1;

			if (npc->flag & 4)
			{
				npc->xm = 0;
				npc->act_no = 1;
			}

			break;
	}

	if (npc->xm < 0)
		npc->direct = 0;
	else
		npc->direct = 2;

	if (npc->xm > 0x5FF)
		npc->xm = 0x5FF;
	if (npc->xm < -0x5FF)
		npc->xm = -0x5FF;

	npc->x += npc->xm;

	if (++npc->ani_wait > 1)
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

//Beetle (Follows you, Egg Corridor)
void ActNpc008(NPCHAR *npc)
{
	RECT rcLeft[2] = {
		{80, 80, 96, 96},
		{96, 80, 112, 96},
	};

	RECT rcRight[2] = {
		{80, 96, 96, 112},
		{96, 96, 112, 112},
	};

	switch (npc->act_no)
	{
		case 0:
			if (gMC.x < npc->x + 0x2000 && gMC.x > npc->x - 0x2000)
			{
				npc->bits |= 0x20;
				npc->ym = -0x100;
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

			if (npc->ym > 0x100)
				npc->ym = 0x100;
			if (npc->ym < -0x100)
				npc->ym = -0x100;

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

//Balrog (drop-in)
void ActNpc009(NPCHAR *npc)
{
	int i;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 2;
			// Fallthrough
		case 1:
			npc->ym += 0x20;

			if (npc->count1 < 40)
			{
				++npc->count1;
			}
			else
			{
				npc->bits &= ~8;
				npc->bits |= 1;
			}

			if (npc->flag & 8)
			{
				for (i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

				npc->act_no = 2;
				npc->ani_no = 1;
				npc->act_wait = 0;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;

		case 2:
			if (++npc->act_wait > 16)
			{
				npc->act_no = 3;
				npc->ani_no = 0;
				npc->ani_wait = 0;
			}

			break;
	}

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;
	if (npc->ym < -0x5FF)
		npc->ym = -0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[3] = {
		{0, 0, 40, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
	};

	RECT rect_right[3] = {
		{0, 24, 40, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
	};

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}

//Balrog (shooting) (super-secret version from prototype)
void ActNpc010(NPCHAR *npc)
{
	unsigned char deg;
	int ym;
	int xm;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			if (++npc->act_wait > 12)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->count1 = 3;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 16)
			{
				--npc->count1;
				npc->act_wait = 0;

				deg = GetArktan(npc->x - gMC.x, npc->y + 0x800 - gMC.y);
				deg += (unsigned char)Random(-0x10, 0x10);
				ym = GetSin(deg);
				xm = GetCos(deg);
				SetNpChar(11, npc->x, npc->y + 0x800, xm, ym, 0, 0, 0x100);

				PlaySoundObject(39, 1);

				if (npc->count1 == 0)
				{
					npc->act_no = 3;
					npc->act_wait = 0;
				}
			}

			break;

		case 3:
			if (++npc->act_wait > 3)
			{
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->xm = (gMC.x - npc->x) / 100;
				npc->ym = -0x600;
				npc->ani_no = 3;
			}

			break;

		case 4:
			if (npc->flag & 5)
				npc->xm = 0;

			if (npc->y + 0x2000 < gMC.y)
				npc->damage = 5;
			else
				npc->damage = 0;

			if (npc->flag & 8)
			{
				npc->act_no = 5;
				npc->act_wait = 0;
				npc->ani_no = 2;
				PlaySoundObject(26, 1);
				SetQuake(30);
				npc->damage = 0;
			}

			break;

		case 5:
			npc->xm = 0;

			if (++npc->act_wait > 3)
			{
				npc->act_no = 1;
				npc->act_wait = 0;
			}

			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[4] = {
		{0, 0, 40, 24},
		{40, 0, 80, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
	};

	RECT rect_right[4] = {
		{0, 24, 40, 48},
		{40, 24, 80, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
	};

	if (npc->x < gMC.x)
		npc->direct = 2;
	else
		npc->direct = 0;

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}

//Proto-Balrog's projectile
void ActNpc011(NPCHAR *npc)
{
	if (npc->flag & 0xFF)
	{
		npc->cond = 0;
		SetCaret(npc->x, npc->y, 2, 0);
	}

	npc->y += npc->ym;
	npc->x += npc->xm;

	RECT rect_left[3] = {
		{208, 104, 224, 120},
		{224, 104, 240, 120},
		{240, 104, 256, 120},
	};

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;

		if (++npc->ani_no > 2)
			npc->ani_no = 0;
	}

	npc->rect = rect_left[npc->ani_no];

	if (++npc->count1 > 150)
	{
		SetCaret(npc->x, npc->y, 2, 0);
		npc->cond = 0;
	}
}

//Balrog (cutscene)
void ActNpc012(NPCHAR *npc)
{
	int i;
	int x;
	int y;

	switch (npc->act_no)
	{
		case 0:
			if (npc->direct == 4)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 1;
			npc->ani_no = 0;
			// Fallthrough
		case 1:
			if (Random(0, 100) == 0)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 16)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 10:
			if (npc->direct == 4)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 11;
			npc->ani_no = 2;
			npc->act_wait = 0;
			npc->tgt_x = 0;
			// Fallthrough
		case 11:
			if (++npc->act_wait > 30)
			{
				npc->act_no = 12;
				npc->act_wait = 0;
				npc->ani_no = 3;
				npc->ym = -0x800;
				npc->bits |= 8;
			}

			break;

		case 12:
			if (npc->flag & 5)
				npc->xm = 0;

			if (npc->y < 0)
			{
				npc->code_char = 0;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;

		case 20:
			if (npc->direct == 4)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 21;
			npc->ani_no = 5;
			npc->act_wait = 0;
			npc->count1 = 0;

			for (i = 0; i < 4; ++i)
				SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			PlaySoundObject(72, 1);
			// Fallthrough
		case 21:
			npc->tgt_x = 1;

			if (npc->flag & 8)
				++npc->act_wait;

			if (++npc->count1 / 2 % 2)
				npc->x += 0x200;
			else
				npc->x -= 0x200;

			if (npc->act_wait > 100)
			{
				npc->act_no = 11;
				npc->act_wait = 0;
				npc->ani_no = 2;
			}

			npc->ym += 0x20;
			if (npc->ym > 0x5FF)
				npc->ym = 0x5FF;

			break;

		case 30:
			npc->ani_no = 4;

			if (++npc->act_wait > 100)
			{
				npc->act_no = 0;
				npc->ani_no = 0;
			}

			break;

		case 40:
			if (npc->direct == 4)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 41;
			npc->act_wait = 0;
			npc->ani_no = 5;
			// Fallthrough
		case 41:
			if (++npc->ani_wait / 2 % 2)
				npc->ani_no = 5;
			else
				npc->ani_no = 6;

			break;

		case 42:
			if (npc->direct == 4)
			{
				if (npc->x > gMC.x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			npc->act_no = 43;
			npc->act_wait = 0;
			npc->ani_no = 6;
			// Fallthrough
		case 43:
			if (++npc->ani_wait / 2 % 2)
				npc->ani_no = 7;
			else
				npc->ani_no = 6;

			break;

		case 50:
			npc->ani_no = 8;
			npc->xm = 0;
			break;

		case 60:
			npc->act_no = 61;
			npc->ani_no = 9;
			npc->ani_wait = 0;
			// Fallthrough
		case 61:
			if (++npc->ani_wait > 3)
			{
				npc->ani_wait = 0;

				if (++npc->ani_no == 10 || npc->ani_no == 11)
					PlaySoundObject(23, 1);
			}

			if (npc->ani_no > 12)
				npc->ani_no = 9;

			if (npc->direct == 0)
				npc->xm = -0x200;
			else
				npc->xm = 0x200;

			break;

		case 70:
			npc->act_no = 71;
			npc->act_wait = 64;
			PlaySoundObject(29, 1);
			npc->ani_no = 13;
			// Fallthrough
		case 71:
			if (--npc->act_wait == 0)
				npc->cond = 0;

			break;

		case 80:
			npc->count1 = 0;
			npc->act_no = 81;
			// Fallthrough
		case 81:
			if (++npc->count1 / 2 % 2)
				npc->x += 0x200;
			else
				npc->x -= 0x200;

			npc->ani_no = 5;
			npc->xm = 0;
			npc->ym += 0x20;

			break;

		case 100:
			npc->act_no = 101;
			npc->act_wait = 0;
			npc->ani_no = 2;
			// Fallthrough
		case 101:
			if (++npc->act_wait > 20)
			{
				npc->act_no = 102;
				npc->act_wait = 0;
				npc->ani_no = 3;
				npc->ym = -0x800;
				npc->bits |= 8;
				DeleteNpCharCode(150, 0);
				DeleteNpCharCode(117, 0);
				SetNpChar(355, 0, 0, 0, 0, 0, npc, 0x100);
				SetNpChar(355, 0, 0, 0, 0, 1, npc, 0x100);
			}

			break;

		case 102:
		{
			x = npc->x / 0x200 / 0x10;
			y = npc->y / 0x200 / 0x10;

			if (y >= 0 && y < 35 && ChangeMapParts(x, y, 0))
			{
				ChangeMapParts(x - 1, y, 0);
				ChangeMapParts(x + 1, y, 0);
				PlaySoundObject(44, 1);
				SetQuake2(10);
			}

			if (npc->y < -0x4000)
			{
				npc->code_char = 0;
				SetQuake(30);
			}

			break;
		}
	}

	if (npc->tgt_x && Random(0, 10) == 0)
		SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[14] = {
		{0, 0, 40, 24},
		{160, 0, 200, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
		{240, 0, 280, 24},
		{200, 0, 240, 24},
		{280, 0, 320, 24},
		{0, 0, 0, 0},
		{80, 48, 120, 72},
		{0, 48, 40, 72},
		{0, 0, 40, 24},
		{40, 48, 80, 72},
		{0, 0, 40, 24},
		{280, 0, 320, 24},
	};

	RECT rect_right[14] = {
		{0, 24, 40, 48},
		{160, 24, 200, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
		{240, 24, 280, 48},
		{200, 24, 240, 48},
		{280, 24, 320, 48},
		{0, 0, 0, 0},
		{80, 72, 120, 96},
		{0, 72, 40, 96},
		{0, 24, 40, 48},
		{40, 72, 80, 96},
		{0, 24, 40, 48},
		{280, 24, 320, 48},
	};

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];

	if (npc->act_no == 71)
	{
		npc->rect.bottom = npc->rect.top + npc->act_wait / 2;

		if (npc->act_wait % 2)
			++npc->rect.left;
	}
}

//Forcefield
void ActNpc013(NPCHAR *npc)
{
	RECT rect[4] = {
		{128, 0, 144, 16},
		{144, 0, 160, 16},
		{160, 0, 176, 16},
		{176, 0, 192, 16},
	};

	if (++npc->ani_wait > 0)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 3)
		npc->ani_no = 0;

	npc->rect = rect[npc->ani_no];
}

//Santa's Key
void ActNpc014(NPCHAR *npc)
{
	RECT rect[3] = {
		{192, 0, 208, 16},
		{208, 0, 224, 16},
		{224, 0, 240, 16},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
			{
				npc->ym = -0x200;

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

		break;
	}

	if (++npc->ani_wait > 1)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 2)
		npc->ani_no = 0;

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	npc->rect = rect[npc->ani_no];
}

//Chest (closed)
void ActNpc015(NPCHAR *npc)
{
	RECT rcLeft[3] = {
		{240, 0, 256, 16},
		{256, 0, 272, 16},
		{272, 0, 288, 16},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->bits |= 0x2000u;

			if (npc->direct == 2)
			{
				npc->ym = -0x200;

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			// Fallthrough
		case 1:
			npc->ani_no = 0;

			if (Random(0, 30) == 0)
				npc->act_no = 2;

			break;

		case 2:
			if (++npc->ani_wait > 1)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 2)
			{
				npc->ani_no = 0;
				npc->act_no = 1;
			}

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	npc->rect = rcLeft[npc->ani_no];
}

//Save point
void ActNpc016(NPCHAR *npc)
{
	RECT rect[8] = {
		{96, 16, 112, 32},
		{112, 16, 128, 32},
		{128, 16, 144, 32},
		{144, 16, 160, 32},
		{160, 16, 176, 32},
		{176, 16, 192, 32},
		{192, 16, 208, 32},
		{208, 16, 224, 32},
	};

	switch (npc->act_no)
	{
		case 0:
			npc->bits |= 0x2000;
			npc->act_no = 1;

			if (npc->direct == 2)
			{
				npc->bits &= ~0x2000;
				npc->ym = -0x200;

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			// Fallthrough
		case 1:
			if (npc->flag & 8)
				npc->bits |= 0x2000;

			break;
	}

	if (++npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if (npc->ani_no > 7)
		npc->ani_no = 0;

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	npc->rect = rect[npc->ani_no];
}

// Health refill
void ActNpc017(NPCHAR *npc)
{
	RECT rect[2] = {
		{288, 0, 304, 16},
		{304, 0, 320, 16},
	};

	int a;
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
			{
				npc->ym = -0x200;

				for (a = 0; a < 4; ++a)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			// Fallthrough
		case 1:
			a = Random(0, 30);

			if (a < 10)
				npc->act_no = 2;
			else if (a < 25)
				npc->act_no = 3;
			else
				npc->act_no = 4;

			npc->act_wait = Random(0x10, 0x40);
			npc->ani_wait = 0;
			break;

		case 2:
			npc->rect = rect[0];

			if (--npc->act_wait == 0)
				npc->act_no = 1;

			break;

		case 3:
			if (++npc->ani_wait % 2)
				npc->rect = rect[0];
			else
				npc->rect = rect[1];

			if (--npc->act_wait == 0)
				npc->act_no = 1;

			break;

		case 4:
			npc->rect = rect[1];

			if (--npc->act_wait == 0)
				npc->act_no = 1;

			break;
	}

	npc->ym += 0x40;

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->y += npc->ym;
}

// Door
void ActNpc018(NPCHAR *npc)
{
	RECT rect[2] = {
		{224, 16, 240, 40},
		{192, 112, 208, 136},
	};

	switch (npc->act_no)
	{
		case 0:
			if (npc->direct == 0)
				npc->rect = rect[0];
			else
				npc->rect = rect[1];

			break;

		case 1:
			for (int i = 0; i < 4; i++)
				SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			npc->act_no = 0;
			npc->rect = rect[0];
			break;
	}
}

// Balrog (burst)
void ActNpc019(NPCHAR *npc)
{
	int i;

	switch (npc->act_no)
	{
		case 0:
			for (i = 0; i < 0x10; ++i)
				SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			npc->y += 0x1400;
			npc->act_no = 1;
			npc->ani_no = 3;
			npc->ym = -0x100;
			PlaySoundObject(12, 1);
			PlaySoundObject(26, 1);
			SetQuake(30);
			// Fallthrough
		case 1:
			npc->ym += 0x10;

			if (npc->ym > 0 && npc->flag & 8)
			{
				npc->act_no = 2;
				npc->ani_no = 2;
				npc->act_wait = 0;
				PlaySoundObject(26, 1);
				SetQuake(30);
			}

			break;

		case 2:
			if (++npc->act_wait > 0x10)
			{
				npc->act_no = 3;
				npc->ani_no = 0;
				npc->ani_wait = 0;
			}

			break;

		case 3:
			if (Random(0, 100) == 0)
			{
				npc->act_no = 4;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 4:
			if (++npc->act_wait > 0x10)
			{
				npc->act_no = 3;
				npc->ani_no = 0;
			}

			break;
	}

	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;
	if (npc->ym < -0x5FF)
		npc->ym = -0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	RECT rect_left[4] = {
		{0, 0, 40, 24},
		{160, 0, 200, 24},
		{80, 0, 120, 24},
		{120, 0, 160, 24},
	};

	RECT rect_right[4] = {
		{0, 24, 40, 48},
		{160, 24, 200, 48},
		{80, 24, 120, 48},
		{120, 24, 160, 48},
	};

	if (npc->direct == 0)
		npc->rect = rect_left[npc->ani_no];
	else
		npc->rect = rect_right[npc->ani_no];
}
