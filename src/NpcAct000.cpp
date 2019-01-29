#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"

//Null
void ActNpc000(NPCHAR *npc)
{
	RECT rect[1];
	rect[0] = {0x00, 0x00, 0x10, 0x10};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
				npc->y += 0x2000;

			break;
	}

	npc->rect = rect[0];
}

//Experience
void ActNpc001(NPCHAR *npc)
{
	//When not in wind
	if (gBack.type != 5 && gBack.type != 6)
	{
		if (!npc->act_no)
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
			npc->xm = -npc->xm;
		if (npc->flag & 4 && npc->xm > 0)
			npc->xm = -npc->xm;

		//Bounce off ceiling
		if (npc->flag & 2 && npc->ym < 0)
			npc->ym = -npc->ym;

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
				npc->y -= 512;
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
	//In wind
	else
	{
		if (!npc->act_no)
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
		if (npc->x <= 0x9FFF)
			npc->cond = 0;

		//Limit speed (except pixel applied it to the X position)
		if (npc->x < -0x5FF)
			npc->x = -0x5FF;

		//Bounce off walls
		if (npc->flag & 1)
			npc->xm = 0x100;
		if (npc->flag & 2)
			npc->ym = 0x40;
		if (npc->flag & 8)
			npc->ym = -0x40;
	}

	//Move
	npc->x += npc->xm;
	npc->y += npc->ym;

	//Get framerects
	RECT rect[6];
	rect[0] = {0x00, 0x10, 0x10, 0x20};
	rect[1] = {0x10, 0x10, 0x20, 0x20};
	rect[2] = {0x20, 0x10, 0x30, 0x20};
	rect[3] = {0x30, 0x10, 0x40, 0x20};
	rect[4] = {0x40, 0x10, 0x50, 0x20};
	rect[5] = {0x50, 0x10, 0x60, 0x20};

	RECT rcNo = {0, 0, 0, 0};

	//Animate
	++npc->ani_wait;

	if (npc->direct)
	{
		if (npc->ani_wait > 2)
		{
			npc->ani_wait = 0;
			if (--npc->ani_no < 0)
				npc->ani_no = 5;
		}
	}
	else if (npc->ani_wait > 2)
	{
		npc->ani_wait = 0;
		if (++npc->ani_no > 5)
			npc->ani_no = 0;
	}

	npc->rect = rect[npc->ani_no];

	//Size
	if (npc->act_no)
	{
		if (npc->exp == 5)
		{
			npc->rect.top += 16;
			npc->rect.bottom += 16;
		}
		else if (npc->exp == 20)
		{
			npc->rect.top += 32;
			npc->rect.bottom += 32;
		}

		npc->act_no = 1;
	}

	//Delete after 500 frames
	if (++npc->count1 > 500 && npc->ani_no == 5 && npc->ani_wait == 2)
		npc->cond = 0;

	//Blink after 400 frames
	if (npc->count1 > 400)
	{
		if (npc->count1 / 2 & 1)
			npc->rect = rcNo;
	}
}

//Behemoth
void ActNpc002(NPCHAR *npc)
{
	//Rects
	RECT rcLeft[7];
	rcLeft[0] = {32, 0, 64, 24};
	rcLeft[1] = {0, 0, 32, 24};
	rcLeft[2] = {32, 0, 64, 24};
	rcLeft[3] = {64, 0, 96, 24};
	rcLeft[4] = {96, 0, 128, 24};
	rcLeft[5] = {128, 0, 160, 24};
	rcLeft[6] = {160, 0, 192, 24};

	RECT rcUp[7];
	rcUp[0] = {32, 24, 64, 48};
	rcUp[1] = {0, 24, 32, 48};
	rcUp[2] = {32, 24, 64, 48};
	rcUp[3] = {64, 24, 96, 48};
	rcUp[4] = {96, 24, 128, 48};
	rcUp[5] = {128, 24, 160, 48};
	rcUp[6] = {160, 24, 192, 48};

	//Turn when touching a wall
	if (npc->flag & 1)
		npc->direct = 2;
	else if (npc->flag & 4)
		npc->direct = 0;

	switch (npc->act_no)
	{
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
			if (npc->direct)
				npc->xm = 0x400;
			else
				npc->xm = -0x400;

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
				npc->ani_no = 5;
			break;
		case 0: //Walking
			if (npc->direct)
				npc->xm = 0x100;
			else
				npc->xm = -0x100;

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
	}

	//Gravity
	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	//Move
	npc->x += npc->xm;
	npc->y += npc->ym;

	//Set framerect
	if (npc->direct)
		npc->rect = rcUp[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

//Dead enemy (make sure damage shown doesn't teleport to a new loaded npc)
void ActNpc003(NPCHAR *npc)
{
	if (++npc->count1 > 100)
		npc->cond = 0;

	npc->rect = {0, 0, 0, 0};
}

//Smoke
void ActNpc004(NPCHAR *npc)
{
	RECT rcLeft[8];
	RECT rcUp[8];

	rcLeft[0] = {16, 0, 17, 1};
	rcLeft[1] = {16, 0, 32, 16};
	rcLeft[2] = {32, 0, 48, 16};
	rcLeft[3] = {48, 0, 64, 16};
	rcLeft[4] = {64, 0, 80, 16};
	rcLeft[5] = {80, 0, 96, 16};
	rcLeft[6] = {96, 0, 112, 16};
	rcLeft[7] = {112, 0, 128, 16};

	rcUp[0] = {16, 0, 17, 1};
	rcUp[1] = {80, 48, 96, 64};
	rcUp[2] = {0, 128, 16, 144};
	rcUp[3] = {16, 128, 32, 144};
	rcUp[4] = {32, 128, 48, 144};
	rcUp[5] = {48, 128, 64, 144};
	rcUp[6] = {64, 128, 80, 144};
	rcUp[7] = {80, 128, 96, 144};

	if (npc->act_no)
	{
		//Slight drag
		npc->xm = 20 * npc->xm / 21;
		npc->ym = 20 * npc->ym / 21;

		//Move
		npc->x += npc->xm;
		npc->y += npc->ym;
	}
	else
	{
		//Move in random direction at random speed
		if (!npc->direct || npc->direct == 1)
		{
			uint8_t deg = Random(0, 0xFF);
			npc->xm = GetCos(deg) * Random(0x200, 0x5FF) / 0x200;
			npc->ym = GetSin(deg) * Random(0x200, 0x5FF) / 0x200;
		}

		//Set state
		npc->ani_no = Random(0, 4);
		npc->ani_wait = Random(0, 3);
		npc->act_no = 1;
	}

	//Animate
	if (++npc->ani_wait > 4)
	{
		npc->ani_wait = 0;
		npc->ani_no++;
	}

	//Set framerect
	if (npc->ani_no < 8)
	{
		if (npc->direct == 1)
			npc->rect = rcUp[npc->ani_no];
		if (!npc->direct)
			npc->rect = rcLeft[npc->ani_no];
		if (npc->direct == 2)
			npc->rect = rcLeft[npc->ani_no];
	}
	else
	{
		//Destroy if over
		npc->cond = 0;
	}
}

//Critter (Green, Egg Corridor)
void ActNpc005(NPCHAR *npc)
{
	RECT rcLeft[3];
	RECT rcRight[3];

	rcLeft[0] = {0, 48, 16, 64};
	rcLeft[1] = {16, 48, 32, 64};
	rcLeft[2] = {32, 48, 48, 64};

	rcRight[0] = {0, 64, 16, 80};
	rcRight[1] = {16, 64, 32, 80};
	rcRight[2] = {32, 64, 48, 80};

	switch (npc->act_no)
	{
		case 0: //Init
			npc->y += 0x600;
			npc->act_no = 1;

		case 1: //Waiting
			//Look at player
			if (npc->x <= gMC.x)
				npc->direct = 2;
			else
				npc->direct = 0;

			//Open eyes near player
			if (npc->act_wait < 8 || npc->x - 0xE000 >= gMC.x || npc->x + 0xE000 <= gMC.x || npc->y - 0xA000 >= gMC.y || npc->y + 0xA000 <= gMC.y)
			{
				if (npc->act_wait < 8)
					++npc->act_wait;
				npc->ani_no = 0;
			}
			else
			{
				npc->ani_no = 1;
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
				if (npc->direct)
					npc->xm = 0x100;
				else
					npc->xm = -0x100;
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
	if (npc->direct)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

//Beetle (Goes left and right, Egg Corridor)
void ActNpc006(NPCHAR *npc)
{
	RECT rcLeft[5];
	RECT rcRight[5];
	
	rcLeft[0] = {0, 80, 16, 96};
	rcLeft[1] = {16, 80, 32, 96};
	rcLeft[2] = {32, 80, 48, 96};
	rcLeft[3] = {48, 80, 64, 96};
	rcLeft[4] = {64, 80, 80, 96};
	
	rcRight[0] = {0, 96, 16, 112};
	rcRight[1] = {16, 96, 32, 112};
	rcRight[2] = {32, 96, 48, 112};
	rcRight[3] = {48, 96, 64, 112};
	rcRight[4] = {64, 96, 80, 112};
	
	switch (npc->act_no)
	{
		case 0: //Init
			npc->act_no = 1;
			
			if (npc->direct)
				npc->act_no = 3;
			else
				npc->act_no = 1;
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
			
			if ( npc->ani_no > 2 )
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
			
			if ( npc->ani_no > 2 )
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
			
		default:
			break;
	}
	
	//Set framerect
	if (npc->direct)
		npc->rect = rcRight[npc->ani_no];
	else
		npc->rect = rcLeft[npc->ani_no];
}

//Beetle (Follows you, Egg Corridor)
void ActNpc008(NPCHAR *npc)
{
	RECT rcLeft[2];
	RECT rcRight[2];

	rcLeft[0] = {80, 80, 96, 96};
	rcLeft[1] = {96, 80, 112, 96};

	rcRight[0] = {80, 96, 96, 112};
	rcRight[1] = {96, 96, 112, 112};

	switch (npc->act_no)
	{
		case 0:
			if (gMC.x >= npc->x + 0x2000 || gMC.x <= npc->x - 0x2000)
			{
				npc->bits &= ~0x20;
				npc->rect.right = 0;
				npc->damage = 0;
				npc->xm = 0;
				npc->ym = 0;
				return;
			}

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

			break;

		case 1:
			if (npc->x <= gMC.x)
			{
				npc->direct = 2;
				npc->xm += 0x10;
			}
			else
			{
				npc->direct = 0;
				npc->xm -= 0x10;
			}

			if (npc->xm > 0x2FF)
				npc->xm = 0x2FF;
			if (npc->xm < -0x2FF)
				npc->xm = -0x2FF;

			if (npc->y >= npc->tgt_y)
				npc->ym -= 8;
			else
				npc->ym += 8;

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

//Chest (closed)
void ActNpc015(NPCHAR *npc)
{
	RECT rcLeft[3];

	rcLeft[0] = {240, 0, 256, 16};
	rcLeft[1] = {256, 0, 272, 16};
	rcLeft[2] = {272, 0, 288, 16};

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
	RECT rects[8];

	rects[0] = {96, 16, 112, 32};
	rects[1] = {112, 16, 128, 32};
	rects[2] = {128, 16, 144, 32};
	rects[3] = {144, 16, 160, 32};
	rects[4] = {160, 16, 176, 32};
	rects[5] = {176, 16, 192, 32};
	rects[6] = {192, 16, 208, 32};
	rects[7] = {208, 16, 224, 32};

	switch (npc->act_no)
	{
		case 0:
			npc->bits |= 0x2000u;
			npc->act_no = 1;

			if (npc->direct == 2)
			{
				npc->bits &= ~0x2000u;
				npc->ym = -0x200;

				for (int i = 0; i < 4; ++i)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			// Fallthrough
		case 1:
			if (npc->flag & 8)
				npc->bits |= 0x2000u;

			break;
	}

	if ( ++npc->ani_wait > 2 )
	{
		npc->ani_wait = 0;
		++npc->ani_no;
	}

	if ( npc->ani_no > 7 )
		npc->ani_no = 0;

	npc->ym += 0x40;
	if ( npc->ym > 0x5FF )
		npc->ym = 0x5FF;

	npc->y += npc->ym;

	npc->rect = rects[npc->ani_no];
}

// Health refill
void ActNpc017(NPCHAR *npc)
{
	RECT rect[2];

	rect[0] = {288, 0, 304, 16};
	rect[1] = {304, 0, 320, 16};

	int aa;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;

			if (npc->direct == 2)
			{
				npc->ym = -512;

				for (int a = 0; a < 4; ++a)
					SetNpChar(4, npc->x + (Random(-12, 12) * 0x200), npc->y + (Random(-12, 12) * 0x200), Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);
			}

			// Fallthrough
		case 1:
			aa = Random(0, 30);

			if (aa >= 10)
			{
				if (aa >= 25)
					npc->act_no = 4;
				else
					npc->act_no = 3;
			}
			else
			{
				npc->act_no = 2;
			}

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
	RECT rect[2];

	rect[0] = {224, 16, 240, 40};
	rect[1] = {192, 112, 208, 136};

	switch (npc->act_no)
	{
		case 0:
			if (npc->direct == 0)
				npc->rect = rect[0];
			else
				npc->rect = rect[1];

			break;

		case 1:
			for (int i = 0; i < 4; ++i)
				SetNpChar(4, npc->x, npc->y, Random(-341, 341), Random(-0x600, 0), 0, 0, 0x100);

			npc->act_no = 0;
			npc->rect = rect[0];
			break;
	}
}