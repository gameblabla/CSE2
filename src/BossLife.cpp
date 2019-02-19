#include "BossLife.h"

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Boss.h"
#include "Draw.h"
#include "NpChar.h"

static struct
{
	bool flag;
	int *pLife;
	int max;
	int br;
	int count;
} gBL;

void InitBossLife(void)
{
	gBL.flag = false;
}

bool StartBossLife(int code_event)
{
	for (int i = 0; i < 0x200; ++i)
	{
		if (gNPC[i].code_event == code_event)
		{
			gBL.flag = true;
			gBL.max = gNPC[i].life;
			gBL.br = gNPC[i].life;
			gBL.pLife = &gNPC[i].life;
			return true;
		}
	}

	return false;
}

bool StartBossLife2(void)
{
	gBL.flag = true;
	gBL.max = gBoss[0].life;
	gBL.br = gBoss[0].life;
	gBL.pLife = &gBoss[0].life;
	return true;
}

void PutBossLife(void)
{
	RECT rcText = {0, 48, 32, 56};
	RECT rcBox1 = {0, 0, 244, 8};
	RECT rcBox2 = {0, 16, 244, 24};
	RECT rcLife = {0, 24, 0, 32};
	RECT rcBr = {0, 32, 232, 40};

	if (gBL.flag)
	{
		if (*gBL.pLife >= 1)
		{
			rcLife.right = 198 * *gBL.pLife / gBL.max;
	
			if (gBL.br <= *gBL.pLife)
			{
				gBL.count = 0;
			}
			else if (++gBL.count > 30)
			{
				--gBL.br;
			}
	
			rcBr.right = 198 * gBL.br / gBL.max;

			PutBitmap3(&grcGame, (WINDOW_WIDTH - 256) / 2, WINDOW_HEIGHT - 20, &rcBox1, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&grcGame, (WINDOW_WIDTH - 256) / 2, WINDOW_HEIGHT - 12, &rcBox2, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&grcGame, (WINDOW_WIDTH - 176) / 2, WINDOW_HEIGHT - 16, &rcBr, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&grcGame, (WINDOW_WIDTH - 176) / 2, WINDOW_HEIGHT - 16, &rcLife, SURFACE_ID_TEXT_BOX);
			PutBitmap3(&grcGame, (WINDOW_WIDTH - 240) / 2, WINDOW_HEIGHT - 16, &rcText, SURFACE_ID_TEXT_BOX);
		}
		else
		{
			gBL.flag = false;
		}
	}
}
