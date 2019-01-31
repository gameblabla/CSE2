#include "Flash.h"

#include "Draw.h"
#include "WindowsWrapper.h"

static struct
{
	int mode;
	int act_no;
	bool flag;
	int cnt;
	int width;
	int x;
	int y;
	RECT rect1;
	RECT rect2;
} flash;

static unsigned long gFlashColor;

void InitFlash(void)
{
	gFlashColor = 0xFEFFFF;
}

void SetFlash(int x, int y, int mode)
{
	flash.act_no = 0;
	flash.flag = true;
	flash.x = x;
	flash.y = y;
	flash.mode = mode;
	flash.cnt = 0;
	flash.width = 0;
}

void ActFlash_Explosion(int flx, int fly)
{
	if (flash.act_no == 0)
	{
		flash.cnt += 0x200;
		flash.width += flash.cnt;

		int right = (flash.x - flx - flash.width) / 0x200;
		int left = (flash.y - fly - flash.width) / 0x200;
		int top = (flash.width + flash.x - flx) / 0x200;
		int bottom = (flash.width + flash.y - fly) / 0x200;

		if (right < 0)
			right = 0;
		if (left < 0)
			left = 0;
		if (top > 320)
			top = 320;
		if (bottom > 240)
			bottom = 240;

		flash.rect1.left = right;
		flash.rect1.right = top;
		flash.rect1.top = 0;
		flash.rect1.bottom = 240;

		flash.rect2.left = 0;
		flash.rect2.right = 320;
		flash.rect2.top = left;
		flash.rect2.bottom = bottom;

		if (flash.width > 0xA0000)
		{
			flash.act_no = 1;
			flash.cnt = 0;
			flash.width = 0x1E000;
		}
	}
	else if (flash.act_no == 1)
	{
		flash.width -= flash.width / 8;

		if ((flash.width / 0x100) == 0)
			flash.flag = false;

		int top = (flash.y - fly - flash.width) / 0x200;
		if (top < 0)
			top = 0;

		int bottom = (flash.width + flash.y - fly) / 0x200;
		if (bottom > 240)
			bottom = 240;

		flash.rect1.left = 0;
		flash.rect1.right = 0;
		flash.rect1.top = 0;
		flash.rect1.bottom = 0;

		flash.rect2.top = top;
		flash.rect2.bottom = bottom;
		flash.rect2.left = 0;
		flash.rect2.right = 320;
	}
}

void ActFlash_Flash(void)
{
	++flash.cnt;

	flash.rect1.left = 0;
	flash.rect1.right = 0;
	flash.rect1.top = 0;
	flash.rect1.bottom = 0;

	if (flash.cnt / 2 % 2)
	{
		flash.rect2.top = 0;
		flash.rect2.bottom = 240;
		flash.rect2.left = 0;
		flash.rect2.right = 320;
	}
	else
	{
		flash.rect2.left = 0;
		flash.rect2.right = 0;
		flash.rect2.top = 0;
		flash.rect2.bottom = 0;
	}

	if (flash.cnt > 20)
		flash.flag = false;
}

void ActFlash(int flx, int fly)
{
	if (flash.flag)
	{
		if (flash.mode == 1)
			ActFlash_Explosion(flx, fly);
		else if (flash.mode == 2)
			ActFlash_Flash();
	}
}

void PutFlash(void)
{
	if (flash.flag)
	{
		CortBox(&flash.rect1, gFlashColor);
		CortBox(&flash.rect2, gFlashColor);
	}
}

void ResetFlash(void)
{
	flash.flag = false;
}
