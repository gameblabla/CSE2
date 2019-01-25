#include <string>
#include <stdint.h>

#include "WindowsWrapper.h"

#include "MyChar.h"
#include "Draw.h"
#include "Sound.h"
#include "KeyControl.h"

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
