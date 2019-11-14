#include "Shoot.h"

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "Bullet.h"
#include "Caret.h"
#include "KeyControl.h"
#include "MyChar.h"
#include "MycParam.h"
#include "Sound.h"

int empty;
int spur_charge;

void ShootBullet_Frontia1(int level)
{
	int bul_no;

	switch (level)
	{
		case 1:
			bul_no = 1;
			break;

		case 2:
			bul_no = 2;
			break;

		case 3:
			bul_no = 3;
			break;
	}

	if (CountArmsBullet(1) > 3)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			ChangeToFirstArms();
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y - (10 * 0x200), 1);
					SetCaret(gMC.x - (3 * 0x200), gMC.y - (10 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y - (10 * 0x200), 1);
					SetCaret(gMC.x + (3 * 0x200), gMC.y - (10 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y + (10 * 0x200), 3);
					SetCaret(gMC.x - (3 * 0x200), gMC.y + (10 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y + (10 * 0x200), 3);
					SetCaret(gMC.x + (3 * 0x200), gMC.y + (10 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (2 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (2 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (2 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (2 * 0x200), 3, 0);
				}
			}

			PlaySoundObject(33, 1);
		}
	}
}

void ShootBullet_PoleStar(int level)
{
	int bul_no;

	switch (level)
	{
		case 1:
			bul_no = 4;
			break;

		case 2:
			bul_no = 5;
			break;

		case 3:
			bul_no = 6;
			break;
	}

	if (CountArmsBullet(2) > 1)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
			}

			if (level == 3)
				PlaySoundObject(49, 1);
			else
				PlaySoundObject(32, 1);
		}
	}
}

void ShootBullet_FireBall(int level)
{
	int bul_no;

	switch (level)
	{
		case 1:
			if (CountArmsBullet(3) > 1)
				return;

			bul_no = 7;
			break;

		case 2:
			if (CountArmsBullet(3) > 2)
				return;

			bul_no = 8;
			break;

		case 3:
			if (CountArmsBullet(3) > 3)
				return;

			bul_no = 9;
			break;
	}

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			ChangeToFirstArms();
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (4 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x - (4 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (4 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (4 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (4 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (4 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (4 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (4 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (2 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (2 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (2 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (2 * 0x200), 3, 0);
				}
			}

			PlaySoundObject(34, 1);
		}
	}
}

void ShootBullet_Machinegun1(int level)
{
	int bul_no;

	if (CountArmsBullet(4) > 4)
		return;

	switch (level)
	{
		case 1:
			bul_no = 10;
			break;

		case 2:
			bul_no = 11;
			break;

		case 3:
			bul_no = 12;
			break;
	}

	if (!(gKey & gKeyShot))
		gMC.rensha = 6;

	if (gKey & gKeyShot)
	{
		if (++gMC.rensha < 6)
			return;

		gMC.rensha = 0;

		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);

			if (empty == 0)
			{
				SetCaret(gMC.x, gMC.y, 16, 0);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (level == 3)
				gMC.ym += 0x100;

			if (gMC.direct == 0)
			{
				SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x - (3 * 0x200), gMC.y - (8 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x + (3 * 0x200), gMC.y - (8 * 0x200), 3, 0);
			}
		}
		else if (gMC.down)
		{
			if (level == 3)
			{
				if (gMC.ym > 0)
					gMC.ym /= 2;

				if (gMC.ym > -0x400)
				{
					gMC.ym -= 0x200;
					if (gMC.ym < -0x400)
						gMC.ym = -0x400;
				}
			}

			if (gMC.direct == 0)
			{
				SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x - (3 * 0x200), gMC.y + (8 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x + (3 * 0x200), gMC.y + (8 * 0x200), 3, 0);
			}
		}
		else
		{
			if (gMC.direct == 0)
			{
				SetBullet(bul_no, gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 0);
				SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(bul_no, gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 2);
				SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
		}

		if (level == 3)
			PlaySoundObject(49, 1);
		else
			PlaySoundObject(32, 1);
	}
	else
	{
		static int wait = 0;
		++wait;

		if (gMC.equip & 8)
		{
			if (wait > 1)
			{
				wait = 0;
				ChargeArmsEnergy(1);
			}
		}
		else
		{
			if (wait > 4)
			{
				wait = 0;
				ChargeArmsEnergy(1);
			}
		}
	}
}

void ShootBullet_Missile(int level, BOOL bSuper)
{
	int bul_no;

	if (bSuper)
	{
		switch (level)
		{
			case 1:
				bul_no = 28;
				break;

			case 2:
				bul_no = 29;
				break;

			case 3:
				bul_no = 30;
				break;
		}

		switch (level)
		{
			case 1:
				if (CountArmsBullet(10) > 0)
					return;

				if (CountArmsBullet(11) > 0)
					return;

				break;

			case 2:
				if (CountArmsBullet(10) > 1)
					return;

				if (CountArmsBullet(11) > 1)
					return;

				break;

			case 3:
				if (CountArmsBullet(10) > 3)
					return;

				if (CountArmsBullet(11) > 3)
					return;

				break;
		}

	}
	else
	{
		switch (level)
		{
			case 1:
				bul_no = 13;
				break;

			case 2:
				bul_no = 14;
				break;

			case 3:
				bul_no = 15;
				break;
		}

		switch (level)
		{
			case 1:
				if (CountArmsBullet(5) > 0)
					return;

				if (CountArmsBullet(6) > 0)
					return;

				break;

			case 2:
				if (CountArmsBullet(5) > 1)
					return;

				if (CountArmsBullet(6) > 1)
					return;

				break;

			case 3:
				if (CountArmsBullet(5) > 3)
					return;

				if (CountArmsBullet(6) > 3)
					return;

				break;
		}
	}

	if (gKeyTrg & gKeyShot)
	{
		if (level < 3)
		{
			if (!UseArmsEnergy(1))
			{
				PlaySoundObject(37, 1);

				if (empty == 0)
				{
					SetCaret(gMC.x, gMC.y, 16, 0);
					empty = 50;
				}

				return;
			}

			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y, 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y, 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y, 3, 0);
				}
			}
		}
		else
		{
			if (!UseArmsEnergy(1))
			{
				PlaySoundObject(37, 1);

				if (empty == 0)
				{
					SetCaret(gMC.x, gMC.y, 16, 0);
					empty = 50;
				}

				return;
			}

			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y, 1);
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y, 1);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y, 1);
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y, 1);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y, 3);
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y, 3);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x - (3 * 0x200), gMC.y, 3);
					SetBullet(bul_no, gMC.x + (3 * 0x200), gMC.y, 3);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (1 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (1 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x, gMC.y - (8 * 0x200), 0);
					SetBullet(bul_no, gMC.x + (4 * 0x200), gMC.y - (1 * 0x200), 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (1 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (1 * 0x200), 3, 0);
					SetBullet(bul_no, gMC.x, gMC.y - (8 * 0x200), 2);
					SetBullet(bul_no, gMC.x - (4 * 0x200), gMC.y - (1 * 0x200), 2);
				}
			}
		}

		PlaySoundObject(32, 1);
	}
}

void ShootBullet_Bubblin1(void)
{
	static int wait;

	if (CountArmsBullet(7) > 3)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);

			if (empty == 0)
			{
				SetCaret(gMC.x, gMC.y, 16, 0);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (1 * 0x200), gMC.y - (2 * 0x200), 1);
				SetCaret(gMC.x - (1 * 0x200), gMC.y - (2 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(19, gMC.x + (1 * 0x200), gMC.y - (2 * 0x200), 1);
				SetCaret(gMC.x + (1 * 0x200), gMC.y - (2 * 0x200), 3, 0);
			}
		}
		else if (gMC.down)
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (1 * 0x200), gMC.y + (2 * 0x200), 3);
				SetCaret(gMC.x - (1 * 0x200), gMC.y + (2 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(19, gMC.x + (1 * 0x200), gMC.y + (2 * 0x200), 3);
				SetCaret(gMC.x + (1 * 0x200), gMC.y + (2 * 0x200), 3, 0);
			}
		}
		else
		{
			if (gMC.direct == 0)
			{
				SetBullet(19, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
				SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(19, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
				SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
		}

		PlaySoundObject(48, 1);
	}
	else if (++wait > 20)
	{
		wait = 0;
		ChargeArmsEnergy(1);
	}
}

void ShootBullet_Bubblin2(int level)
{
	static int wait;

	if (CountArmsBullet(7) > 15)
		return;

	level += 18;

	if (!(gKey & gKeyShot))
		gMC.rensha = 6;

	if (gKey & gKeyShot)
	{
		if (++gMC.rensha < 7)
			return;

		gMC.rensha = 0;

		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);

			if (empty == 0)
			{
				SetCaret(gMC.x, gMC.y, 16, 0);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x - (3 * 0x200), gMC.y - (16 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(level, gMC.x + (3 * 0x200), gMC.y - (8 * 0x200), 1);
				SetCaret(gMC.x + (3 * 0x200), gMC.y - (16 * 0x200), 3, 0);
			}
		}
		else if (gMC.down)
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x - (3 * 0x200), gMC.y + (16 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(level, gMC.x + (3 * 0x200), gMC.y + (8 * 0x200), 3);
				SetCaret(gMC.x + (3 * 0x200), gMC.y + (16 * 0x200), 3, 0);
			}
		}
		else
		{
			if (gMC.direct == 0)
			{
				SetBullet(level, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
				SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
			else
			{
				SetBullet(level, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
				SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
			}
		}

		PlaySoundObject(48, 1);
	}
	else if (++wait > 1)
	{
		wait = 0;
		ChargeArmsEnergy(1);
	}
}

void ShootBullet_Sword(int level)
{
	int bul_no;

	if (CountArmsBullet(9) > 0)
		return;

	switch (level)
	{
		case 1:
			bul_no = 25;
			break;

		case 2:
			bul_no = 26;
			break;

		case 3:
			bul_no = 27;
			break;
	}

	if (gKeyTrg & gKeyShot)
	{
		if (gMC.up)
		{
			if (gMC.direct == 0)
				SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (4 * 0x200), 1);
			else
				SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (4 * 0x200), 1);
		}
		else if (gMC.down)
		{
			if (gMC.direct == 0)
				SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (6 * 0x200), 3);
			else
				SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (6 * 0x200), 3);
		}
		else
		{
			if (gMC.direct == 0)
				SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y - (3 * 0x200), 0);
			else
				SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y - (3 * 0x200), 2);
		}

		PlaySoundObject(34, 1);
	}
}

void ShootBullet_Nemesis(int level)
{
	int bul_no;

	switch (level)
	{
		case 1:
			bul_no = 34;
			break;

		case 2:
			bul_no = 35;
			break;

		case 3:
			bul_no = 36;
			break;
	}

	if (CountArmsBullet(12) > 1)
		return;

	if (gKeyTrg & gKeyShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (12 * 0x200), 1);
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (12 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (12 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (12 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (12 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (12 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (12 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (12 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (22 * 0x200), gMC.y + (3 * 0x200), 0);
					SetCaret(gMC.x - (16 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (22 * 0x200), gMC.y + (3 * 0x200), 2);
					SetCaret(gMC.x + (16 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
			}

			switch (level)
			{
				case 1:
					PlaySoundObject(117, 1);
					break;

				case 2:
					PlaySoundObject(49, 1);
					break;

				case 3:
					PlaySoundObject(60, 1);
					break;
			}
		}
	}
}

void ResetSpurCharge(void)
{
	spur_charge = 0;

	if (gArmsData[gSelectedArms].code == 13)
		ZeroExpMyChar();
}

void ShootBullet_Spur(int level)
{
	static BOOL bMax;
	BOOL bShot;
	int bul_no;

	bShot = FALSE;

	if (gKey & gKeyShot)
	{
		if (gMC.equip & 8)
			AddExpMyChar(3);
		else
			AddExpMyChar(2);

		if (++spur_charge / 2 % 2)
		{
			switch (level)
			{
				case 1:
					PlaySoundObject(59, 1);
					break;

				case 2:
					PlaySoundObject(60, 1);
					break;

				case 3:
					if (!IsMaxExpMyChar())
						PlaySoundObject(61, 1);

					break;
			}
		}
	}
	else
	{
		if (spur_charge)
			bShot = TRUE;

		spur_charge = 0;
	}

	if (IsMaxExpMyChar())
	{
		if (!bMax)
		{
			bMax = TRUE;
			PlaySoundObject(65, 1);
		}
	}
	else
	{
		bMax = FALSE;
	}

	if (!(gKey & gKeyShot))
		ZeroExpMyChar();

	switch (level)
	{
		case 1:
			bul_no = 6;
			bShot = FALSE;
			break;

		case 2:
			bul_no = 37;
			break;

		case 3:
			if (bMax)
				bul_no = 39;
			else
				bul_no = 38;

			break;
	}

	if (CountArmsBullet(13) > 0 || CountArmsBullet(14) > 0)
		return;

	if (gKeyTrg & gKeyShot || bShot)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);
		}
		else
		{
			if (gMC.up)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x - (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 1);
					SetCaret(gMC.x + (1 * 0x200), gMC.y - (8 * 0x200), 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x - (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3);
					SetCaret(gMC.x + (1 * 0x200), gMC.y + (8 * 0x200), 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - (6 * 0x200), gMC.y + (3 * 0x200), 0);
					SetCaret(gMC.x - (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + (6 * 0x200), gMC.y + (3 * 0x200), 2);
					SetCaret(gMC.x + (12 * 0x200), gMC.y + (3 * 0x200), 3, 0);
				}
			}

			switch (bul_no)
			{
				case 6:
					PlaySoundObject(49, 1);
					break;

				case 37:
					PlaySoundObject(62, 1);
					break;

				case 38:
					PlaySoundObject(63, 1);
					break;

				case 39:
					PlaySoundObject(64, 1);
					break;
			}
		}
	}
}

void ShootBullet(void)
{
	if (empty != 0)
		--empty;

	// Only let the player shoot every 4 frames
	static int soft_rensha;	// 'rensha' is Japanese for 'rapid-fire', apparently
	if (soft_rensha != 0)
		--soft_rensha;

	if (gKeyTrg & gKeyShot)
	{
		if (soft_rensha != 0)
			return;

		soft_rensha = 4;
	}

	// Run functions
	if (gMC.cond & 2)
		return;

	switch (gArmsData[gSelectedArms].code)
	{
		case 1:
			ShootBullet_Frontia1(gArmsData[gSelectedArms].level);
			break;

		case 2:
			ShootBullet_PoleStar(gArmsData[gSelectedArms].level);
			break;

		case 3:
			ShootBullet_FireBall(gArmsData[gSelectedArms].level);
			break;

		case 4:
			ShootBullet_Machinegun1(gArmsData[gSelectedArms].level);
			break;

		case 5:
			ShootBullet_Missile(gArmsData[gSelectedArms].level, FALSE);
			break;

		case 7:
			switch (gArmsData[gSelectedArms].level)
			{
				case 1:
					ShootBullet_Bubblin1();
					break;

				case 2:
					ShootBullet_Bubblin2(2);
					break;

				case 3:
					ShootBullet_Bubblin2(3);
					break;
			}

			break;

		case 9:
			switch (gArmsData[gSelectedArms].level)
			{
				case 1:
					ShootBullet_Sword(1);
					break;

				case 2:
					ShootBullet_Sword(2);
					break;

				case 3:
					ShootBullet_Sword(3);
					break;
			}

			break;

		case 10:
			ShootBullet_Missile(gArmsData[gSelectedArms].level, TRUE);
			break;

		case 12:
			ShootBullet_Nemesis(gArmsData[gSelectedArms].level);
			break;

		case 13:
			ShootBullet_Spur(gArmsData[gSelectedArms].level);
			break;
	}
}
