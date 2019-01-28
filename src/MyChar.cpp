#include <string>
#include <stdint.h>

#include "WindowsWrapper.h"

#include "MyChar.h"
#include "MycParam.h"
#include "ArmsItem.h"
#include "NpChar.h"
#include "Draw.h"
#include "Sound.h"
#include "ValueView.h"
#include "KeyControl.h"
#include "TextScr.h"
#include "Flags.h"
#include "Game.h"
#include "Caret.h"

MYCHAR gMC;

void InitMyChar()
{
	memset(&gMC, 0, sizeof(MYCHAR));
	gMC.cond = 0x80;
	gMC.direct = 2;
	gMC.view = {0x1000, 0x1000, 0x1000, 0x1000};
	gMC.hit = {0xA00, 0x1000, 0xA00, 0x1000};
	gMC.life = 3;
	gMC.max_life = 3;
	gMC.unit = 0;
}

void AnimationMyChar(bool bKey)
{
	RECT rcLeft[12];
	RECT rcRight[12];
	
	rcLeft[0] = {0, 0, 16, 16};
	rcLeft[1] = {16, 0, 32, 16};
	rcLeft[2] = {0, 0, 16, 16};
	rcLeft[3] = {32, 0, 48, 16};
	rcLeft[4] = {0, 0, 16, 16};
	rcLeft[5] = {48, 0, 64, 16};
	rcLeft[6] = {64, 0, 80, 16};
	rcLeft[7] = {48, 0, 64, 16};
	rcLeft[8] = {80, 0, 96, 16};
	rcLeft[9] = {48, 0, 64, 16};
	rcLeft[10] = {96, 0, 112, 16};
	rcLeft[11] = {112, 0, 128, 16};

	rcRight[0] = {0, 16, 16, 32};
	rcRight[1] = {16, 16, 32, 32};
	rcRight[2] = {0, 16, 16, 32};
	rcRight[3] = {32, 16, 48, 32};
	rcRight[4] = {0, 16, 16, 32};
	rcRight[5] = {48, 16, 64, 32};
	rcRight[6] = {64, 16, 80, 32};
	rcRight[7] = {48, 16, 64, 32};
	rcRight[8] = {80, 16, 96, 32};
	rcRight[9] = {48, 16, 64, 32};
	rcRight[10] = {96, 16, 112, 32};
	rcRight[11] = {112, 16, 128, 32};
	
	if (!(gMC.cond & 2))
	{
		if (gMC.flag & 8)
		{
			if (gMC.cond & 1)
			{
				gMC.ani_no = 11;
			}
			else if (gKey & gKeyUp && (gKeyRight | gKeyLeft) & gKey && bKey)
			{
				gMC.cond |= 4;
				
				if (++gMC.ani_wait > 4)
				{
					gMC.ani_wait = 0;
					if (++gMC.ani_no == 7 || gMC.ani_no == 9)
						PlaySoundObject(24, 1);
				}
				
				if (gMC.ani_no > 9 || gMC.ani_no < 6)
					gMC.ani_no = 6;
			}
			else if ((gKeyRight | gKeyLeft) & gKey && bKey)
			{
				gMC.cond |= 4;
				
				if (++gMC.ani_wait > 4)
				{
					gMC.ani_wait = 0;
					if (++gMC.ani_no == 2 || gMC.ani_no == 4)
						PlaySoundObject(24, 1);
				}
				
				if (gMC.ani_no > 4 || gMC.ani_no < 1)
					gMC.ani_no = 1;
			}
			else if ( gKey & gKeyUp && bKey )
			{
				if (gMC.cond & 4)
					PlaySoundObject(24, 1);
				
				gMC.cond &= ~4;
				gMC.ani_no = 5;
			}
			else
			{
				if (gMC.cond & 4)
					PlaySoundObject(24, 1);
				
				gMC.cond &= ~4;
				gMC.ani_no = 0;
			}
		}
		else if (gMC.up)
		{
			gMC.ani_no = 6;
		}
		else if (gMC.down)
		{
			gMC.ani_no = 10;
		}
		else if ( gMC.ym <= 0 )
		{
			gMC.ani_no = 3;
		}
		else
		{
			gMC.ani_no = 1;
		}
		
		if (gMC.direct)
			gMC.rect = rcRight[gMC.ani_no];
		else
			gMC.rect = rcLeft[gMC.ani_no];
	}
}

void ShowMyChar(bool bShow)
{
	if (bShow)
		gMC.cond &= ~2;
	else
		gMC.cond |= 2;
}

void PutMyChar(int fx, int fy)
{
	if ((gMC.cond & 0x80u) && !(gMC.cond & 2))
	{
		//Draw weapon
		gMC.rect_arms.left = 24 * (gArmsData[gSelectedArms].code % 13);
		gMC.rect_arms.right = gMC.rect_arms.left + 24;
		gMC.rect_arms.top = 96 * (gArmsData[gSelectedArms].code / 13);
		gMC.rect_arms.bottom = gMC.rect_arms.top + 16;
		
		if (gMC.direct == 2)
		{
			gMC.rect_arms.top += 16;
			gMC.rect_arms.bottom += 16;
		}
		
		int arms_offset_y;
		if (gMC.up)
		{
			arms_offset_y = -4;
			gMC.rect_arms.top += 32;
			gMC.rect_arms.bottom += 32;
		}
		else if (gMC.down)
		{
			arms_offset_y = 4;
			gMC.rect_arms.top += 64;
			gMC.rect_arms.bottom += 64;
		}
		else
		{
			arms_offset_y = 0;
		}
		
		if (gMC.ani_no == 1 || gMC.ani_no == 3 || gMC.ani_no == 6 || gMC.ani_no == 8)
			++gMC.rect_arms.top;
		
		if (gMC.direct)
			PutBitmap3(
				&grcGame,
				(gMC.x - gMC.view.left) / 0x200 - fx / 0x200,
				(gMC.y - gMC.view.top) / 0x200 - fy / 0x200 + arms_offset_y,
				&gMC.rect_arms,
				11);
		else
			PutBitmap3(
				&grcGame,
				(gMC.x - gMC.view.left) / 0x200 - fx / 0x200 - 8,
				(gMC.y - gMC.view.top) / 0x200 - fy / 0x200 + arms_offset_y,
				&gMC.rect_arms,
				11);
		
		if (!((gMC.shock >> 1) & 1))
		{
			//Draw player
			RECT rect = gMC.rect;
			if (gMC.equip & 0x40)
			{
				rect.top += 32;
				rect.bottom += 32;
			}
			
			PutBitmap3(&grcGame, (gMC.x - gMC.view.left) / 0x200 - fx / 0x200, (gMC.y - gMC.view.top) / 0x200 - fy / 0x200, &rect, 16);
			
			//Draw airtank
			RECT rcBubble[2];
			rcBubble[0] = {56, 96, 80, 120};
			rcBubble[1] = {80, 96, 104, 120};
			
			++gMC.bubble;
			if (gMC.equip & 0x10 && gMC.flag & 0x100)
				PutBitmap3(&grcGame, gMC.x / 0x200 - 12 - fx / 0x200, gMC.y / 0x200 - 12 - fy / 0x200, &rcBubble[(gMC.bubble >> 1) & 1], 19);
			else if (gMC.unit == 1)
				PutBitmap3(&grcGame, gMC.x / 0x200 - 12 - fx / 0x200, gMC.y / 0x200 - 12 - fy / 0x200, &rcBubble[(gMC.bubble >> 1) & 1], 19);
		}
	}
}

void ActMyChar_Normal(bool bKey)
{
	if (!(gMC.cond & 2))
	{
		//Get speeds and accelerations
		int max_dash;
		int gravity1;
		int gravity2;
		int jump;
		int dash1;
		int dash2;
		int resist;
		
		if (gMC.flag & 0x100)
		{
			max_dash = 0x196;
			gravity1 = 0x28;
			gravity2 = 0x10;
			jump = 0x280;
			dash1 = 0x2A;
			dash2 = 0x10;
			resist = 0x19;
		}
		else
		{
			max_dash = 0x32C;
			gravity1 = 0x50;
			gravity2 = 0x20;
			jump = 0x500;
			dash1 = 0x55;
			dash2 = 0x20;
			resist = 0x33;
		}
		
		//Don't create "?" effect
		gMC.ques = 0;
		
		//If can't control player, stop boosting
		if (!bKey)
			gMC.boost_sw = 0;
		
		//Movement on the ground
		if (gMC.flag & 8 || gMC.flag & 0x10 || gMC.flag & 0x20)
		{
			//Stop boosting and refuel
			gMC.boost_sw = 0;
			
			if (gMC.equip & 1)
			{
				gMC.boost_cnt = 50;
			}
			else if (gMC.equip & 0x20)
			{
				gMC.boost_cnt = 50;
			}
			else
			{
				gMC.boost_cnt = 0;
			}
			
			//Move in direction held
			if (bKey)
			{
				if (gKeyTrg != gKeyDown || gKey != gKeyDown || (gMC.cond & 1) || g_GameFlags & 4)
				{
					if (gKey != gKeyDown)
					{
						if (gKey & gKeyLeft && gMC.xm > -max_dash)
							gMC.xm -= dash1;
						if (gKey & gKeyRight && gMC.xm < max_dash)
							gMC.xm += dash1;
						
						if (gKey & gKeyLeft)
							gMC.direct = 0;
						if (gKey & gKeyRight)
							gMC.direct = 2;
					}
				}
				else
				{
					gMC.cond |= 1;
					gMC.ques = 1;
				}
			}
			
			//Friction
			if (!(gMC.cond & 0x20))
			{
				if (gMC.xm < 0)
				{
					if (gMC.xm <= -resist)
						gMC.xm += resist;
					else
						gMC.xm = 0;
				}
				if (gMC.xm > 0)
				{
					if (gMC.xm >= resist)
						gMC.xm -= resist;
					else
						gMC.xm = 0;
				}
			}
		}
		else
		{
			//Start boosting
			if (bKey)
			{
				if (gMC.equip & 0x21 && gKeyTrg & gKeyJump && gMC.boost_cnt)
				{
					//Booster 0.8
					if (gMC.equip & 1)
					{
						gMC.boost_sw = 1;
						if (gMC.ym > 0x100)
							gMC.ym /= 2;
					}
					
					//Booster 2.0
					if (gMC.equip & 0x20)
					{
						if (gKey & gKeyUp)
						{
							gMC.boost_sw = 2;
							gMC.xm = 0;
							gMC.ym = -0x5FF;
						}
						else if ( gKey & gKeyLeft )
						{
							gMC.boost_sw = 1;
							gMC.ym = 0;
							gMC.xm = -0x5FF;
						}
						else if ( gKey & gKeyRight )
						{
							gMC.boost_sw = 1;
							gMC.ym = 0;
							gMC.xm = 0x5FF;
						}
						else if ( gKey & gKeyDown )
						{
							gMC.boost_sw = 3;
							gMC.xm = 0;
							gMC.ym = 0x5FF;
						}
						else
						{
							gMC.boost_sw = 2;
							gMC.xm = 0;
							gMC.ym = -0x5FF;
						}
					}
				}
				
				//Move left and right
				if ( gKey & gKeyLeft && gMC.xm > -max_dash )
					gMC.xm -= dash2;
				if ( gKey & gKeyRight && gMC.xm < max_dash )
					gMC.xm += dash2;
				
				if ( gKey & gKeyLeft )
					gMC.direct = 0;
				if ( gKey & gKeyRight )
					gMC.direct = 2;
			}
			
			//Slow down when stopped boosting (Booster 2.0)
			if (gMC.equip & 0x20 && gMC.boost_sw && (!(gKey & gKeyJump) || !gMC.boost_cnt))
			{
				if (gMC.boost_sw == 1)
					gMC.xm /= 2;
				else if (gMC.boost_sw == 2)
					gMC.ym /= 2;
			}
			
			//Stop boosting
			if (!gMC.boost_cnt || !(gKey & gKeyJump))
				gMC.boost_sw = 0;
		}
		
		//Jumping
		if ( bKey )
		{
			//Look up and down
			gMC.up = (gKey & gKeyUp) != 0;
			gMC.down = gKey & gKeyDown && !(gMC.flag & 8);
			
			if (gKeyTrg & gKeyJump
				&& (gMC.flag & 8 || gMC.flag & 0x10 || gMC.flag & 0x20)
				&& !(gMC.flag & 0x2000))
			{
				gMC.ym = -jump;
				PlaySoundObject(15, 1);
			}
		}
		
		//Stop interacting when moved
		if (bKey && (gKeyShot | gKeyJump | gKeyUp | gKeyRight | gKeyLeft) & gKey)
			gMC.cond &= ~1;
		
		//Booster losing fuel
		if (gMC.boost_sw && gMC.boost_cnt)
			--gMC.boost_cnt;
		
		//Wind / current forces
		if (gMC.flag & 0x1000)
			gMC.xm -= 0x88;
		if (gMC.flag & 0x2000)
			gMC.ym -= 0x80;
		if (gMC.flag & 0x4000)
			gMC.xm += 0x88;
		if (gMC.flag & 0x8000)
			gMC.ym += 0x55;
		
		//Booster 2.0 forces and effects
		if (gMC.equip & 0x20 && gMC.boost_sw)
		{
			if (gMC.boost_sw == 1)
			{
				//Go up when going into a wall
				if (gMC.flag & 5)
					gMC.ym = -0x100;
				
				//Move in direction facing
				if (!gMC.direct)
					gMC.xm -= 0x20;
				if (gMC.direct == 2)
					gMC.xm += 0x20;
				
				//Boost particles (and sound)
				if (gKeyTrg & gKeyJump || gMC.boost_cnt % 3 == 1)
				{
					if (!gMC.direct)
						SetCaret(gMC.x + 0x400, gMC.y + 0x400, 7, 2);
					if (gMC.direct == 2)
						SetCaret(gMC.x - 0x400, gMC.y + 0x400, 7, 0);
					
					PlaySoundObject(113, 1);
				}
			}
			else if (gMC.boost_sw == 2)
			{
				//Move upwards
				gMC.ym -= 0x20;
				
				//Boost particles (and sound)
				if (gKeyTrg & gKeyJump || gMC.boost_cnt % 3 == 1)
				{
					SetCaret(gMC.x, gMC.y + 0xC00, 7, 3);
					PlaySoundObject(113, 1);
				}
			}
			else if (gMC.boost_sw == 3 && (gKeyTrg & gKeyJump || gMC.boost_cnt % 3 == 1))
			{
				//Boost particles (and sound)
				SetCaret(gMC.x, gMC.y - 0xC00, 7, 1);
				PlaySoundObject(113, 1);
			}
		}
		//Upwards wind/current
		else if (gMC.flag & 0x2000)
		{
			gMC.ym += gravity1;
		}
		//Booster 0.8
		else if (gMC.equip & 1 && gMC.boost_sw && gMC.ym > -0x400)
		{
			//Upwards force
			gMC.ym -= 0x20;
			
			if (!(gMC.boost_cnt % 3))
			{
				SetCaret(gMC.x, gMC.hit.bottom / 2 + gMC.y, 7, 3);
				PlaySoundObject(113, 1);
			}
			
			//Bounce off of ceiling
			if (gMC.flag & 2)
				gMC.ym = 0x200;
		}
		//Gravity while jump is held
		else if (gMC.ym < 0 && bKey && gKey & gKeyJump)
		{
			gMC.ym += gravity2;
		}
		//Normal gravity
		else
		{
			gMC.ym += gravity1;
		}
		
		//Keep player on slopes
		if (bKey && !(gKeyTrg & gKeyJump))
		{
			if (gMC.flag & 0x10 && gMC.xm < 0)
				gMC.ym = -gMC.xm;
			if (gMC.flag & 0x20 && gMC.xm > 0)
				gMC.ym = gMC.xm;
			if (gMC.flag & 8 && gMC.flag & 0x80000 && gMC.xm < 0)
				gMC.ym = 0x400;
			if (gMC.flag & 8 && gMC.flag & 0x10000 && gMC.xm > 0)
				gMC.ym = 0x400;
			if (gMC.flag & 8 && gMC.flag & 0x20000 && gMC.flag & 0x40000)
				gMC.ym = 0x400;
		}
		
		//Limit speed
		if (!(gMC.flag & 0x100) || gMC.flag & 0xF000)
		{
			if (gMC.xm < -0x5FF)
				gMC.xm = -0x5FF;
			if (gMC.xm > 0x5FF)
				gMC.xm = 0x5FF;
			if (gMC.ym < -0x5FF)
				gMC.ym = -0x5FF;
			if (gMC.ym > 0x5FF)
				gMC.ym = 0x5FF;
		}
		else
		{
			if (gMC.xm < -0x2FF)
				gMC.xm = -0x2FF;
			if (gMC.xm > 0x2FF)
				gMC.xm = 0x2FF;
			if (gMC.ym < -0x2FF)
				gMC.ym = -0x2FF;
			if (gMC.ym > 0x2FF)
				gMC.ym = 0x2FF;
		}
		
		//Water splashing
		if (!gMC.sprash && gMC.flag & 0x100)
		{
			int dir;
			if (gMC.flag & 0x800)
				dir = 2;
			else
				dir = 0;
			
			if (gMC.flag & 8 || gMC.ym <= 0x200)
			{
				if (gMC.xm > 0x200 || gMC.xm < -0x200)
				{
					for (int a = 0; a < 8; a++)
						SetNpChar(73, gMC.x + (Random(-8, 8) << 9), gMC.y, gMC.xm + Random(-0x200, 0x200), gMC.ym + Random(-0x200, 0x80), dir, 0, 0);
					
					PlaySoundObject(56, 1);
				}
			}
			else
			{
				for (int a = 0; a < 8; a++)
					SetNpChar(73, gMC.x + (Random(-8, 8) << 9), gMC.y, gMC.xm + Random(-0x200, 0x200), Random(-0x200, 0x80) - gMC.ym / 2, dir, 0, 0);
				
				PlaySoundObject(56, 1);
			}
			
			gMC.sprash = 1;
		}
		
		if (!(gMC.flag & 0x100))
			gMC.sprash = 0;
		
		//Spike damage
		if (gMC.flag & 0x400)
			DamageMyChar(10);
		
		//Camera
		if (gMC.direct)
		{
			gMC.index_x += 0x200;
			if (gMC.index_x > 0x8000)
				gMC.index_x = 0x8000;
		}
		else
		{
			gMC.index_x -= 0x200;
			if (gMC.index_x < -0x8000)
				gMC.index_x = -0x8000;
		}
		if (gKey & gKeyUp && bKey)
		{
			gMC.index_y -= 0x200;
			if (gMC.index_y < -0x8000)
				gMC.index_y = -0x8000;
		}
		else if (gKey & gKeyDown && bKey)
		{
			gMC.index_y += 0x200;
			if (gMC.index_y > 0x8000)
				gMC.index_y = 0x8000;
		}
		else
		{
			if (gMC.index_y > 0x200)
				gMC.index_y -= 0x200;
			if (gMC.index_y < -0x200)
				gMC.index_y += 0x200;
		}
		
		gMC.tgt_x = gMC.x + gMC.index_x;
		gMC.tgt_y = gMC.y + gMC.index_y;
		
		//Change position
		if (gMC.xm > resist || gMC.xm < -resist)
			gMC.x += gMC.xm;
		gMC.y += gMC.ym;
	}
}

void AirProcess()
{
	if (gMC.equip & 0x10)
	{
		gMC.air = 1000;
		gMC.air_get = 0;
	}
	else
	{
		if (gMC.flag & 0x100)
		{
			if (--gMC.air <= 0)
			{
				if (GetNPCFlag(4000))
				{
					//Core cutscene
					StartTextScript(1100);
				}
				else
				{
					//Drown
					StartTextScript(41);
					
					if (gMC.direct)
						SetCaret(gMC.x, gMC.y, 8, 2);
					else
						SetCaret(gMC.x, gMC.y, 8, 0);
					
					gMC.cond &= ~0x80;
				}
			}
		}
		else
		{
			gMC.air = 1000;
		}
		
		if ( gMC.flag & 0x100 )
		{
			gMC.air_get = 60;
		}
		else if (gMC.air_get)
		{
			--gMC.air_get;
		}
	}
}

void ActMyChar(bool bKey)
{
	if (gMC.cond & 0x80)
	{
		if (gMC.exp_wait)
			--gMC.exp_wait;
		
		if (gMC.shock)
		{
			--gMC.shock;
		}
		else if (gMC.exp_count)
		{
			SetValueView(&gMC.x, &gMC.y, gMC.exp_count);
			gMC.exp_count = 0;
		}
		
		switch (gMC.unit)
		{
			case 0:
				if (!(g_GameFlags & 4) && bKey)
					AirProcess();
				ActMyChar_Normal(bKey);
				break;
			
			case 1:
				//ActMyChar_Stream(bKey);
				break;
			
			default:
				break;
		}
		
		gMC.cond &= ~0x20;
	}
}

void GetMyCharPosition(int *x, int *y)
{
	*x = gMC.x;
	*y = gMC.y;
}

void SetMyCharPosition(int x, int y)
{
	gMC.x = x;
	gMC.y = y;
	gMC.tgt_x = x;
	gMC.tgt_y = y;
	gMC.index_x = 0;
	gMC.index_y = 0;
	gMC.xm = 0;
	gMC.ym = 0;
	gMC.cond &= ~1;
	//InitStar();
}

void MoveMyChar(int x, int y)
{
	gMC.x = x;
	gMC.y = y;
}
