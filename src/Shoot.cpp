#include "ArmsItem.h"
#include "MycParam.h"
#include "Shoot.h"
#include "Caret.h"
#include "Bullet.h"
#include "Sound.h"
#include "MyChar.h"
#include "KeyControl.h"

int empty;
int spur_charge;

void ShootBullet_PoleStar(int level)
{
	int bul_no;
	switch (level)
	{
	case 2:
		bul_no = 5;
		break;
	case 3:
		bul_no = 6;
		break;
	case 1:
		bul_no = 4;
		break;
	}
	
	if (CountArmsBullet(2) < 2 && gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y - 0x1000, 1);
					SetCaret(gMC.x + 0x200, gMC.y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0x200, gMC.y - 0x1000, 1);
					SetCaret(gMC.x - 0x200, gMC.y - 0x1000, 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y + 0x1000, 3);
					SetCaret(gMC.x + 0x200, gMC.y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0x200, gMC.y + 0x1000, 3);
					SetCaret(gMC.x - 0x200, gMC.y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0xC00, gMC.y + 0x600, 2);
					SetCaret(gMC.x + 0x1800, gMC.y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0xC00, gMC.y + 0x600, 0);
					SetCaret(gMC.x - 0x1800, gMC.y + 0x600, 3, 0);
				}
			}
			
			if (level == 3)
				PlaySoundObject(49, 1);
			else
				PlaySoundObject(32, 1);
		}
	}
}

void ShootBullet()
{
	if (empty)
		--empty;
	
	//Only let the player shoot every 4 frames
	static int soft_rensha;
	if (soft_rensha)
		--soft_rensha;
	
	if (gKeyTrg & gKeyShot)
	{
		if (soft_rensha)
			return;
		soft_rensha = 4;
	}
	
	//Run functions
	if (!(gMC.cond & 2))
	{
		switch (gArmsData[gSelectedArms].code)
		{
			case 2:
				ShootBullet_PoleStar(gArmsData[gSelectedArms].level);
				break;
		}
	}
}
