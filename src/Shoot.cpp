#include "Shoot.h"

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

	if (CountArmsBullet(1) <= 3 && gKeyTrg & gKeyShot)
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
					SetBullet(bul_no, gMC.x - 0x600, gMC.y - 0x1400, 1);
					SetCaret(gMC.x - 0x600, gMC.y - 0x1400, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x600, gMC.y - 0x1400, 1);
					SetCaret(gMC.x + 0x600, gMC.y - 0x1400, 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0x600, gMC.y + 0x1400, 3);
					SetCaret(gMC.x - 0x600, gMC.y + 0x1400, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x600, gMC.y + 0x1400, 3);
					SetCaret(gMC.x + 0x600, gMC.y + 0x1400, 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0xC00, gMC.y + 0x400, 0);
					SetCaret(gMC.x - 0x1800, gMC.y + 0x400, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0xC00, gMC.y + 0x400, 2);
					SetCaret(gMC.x + 0x1800, gMC.y + 0x400, 3, 0);
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

void ShootBullet_FireBall(int level)
{
	int bul_no;

	switch (level)
	{
		case 1:
			if (CountArmsBullet(3) >= 2)
				return;
			bul_no = 7;
			break;
		case 2:
			if (CountArmsBullet(3) >= 3)
				return;
			bul_no = 8;
			break;
		case 3:
			if (CountArmsBullet(3) >= 4)
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
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0x800, gMC.y - 0x1000, 1);
					SetCaret(gMC.x + 0x800, gMC.y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0x800, gMC.y - 0x1000, 1);
					SetCaret(gMC.x - 0x800, gMC.y - 0x1000, 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0x800, gMC.y + 0x1000, 3);
					SetCaret(gMC.x + 0x800, gMC.y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0x800, gMC.y + 0x1000, 3);
					SetCaret(gMC.x - 0x800, gMC.y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (gMC.direct)
				{
					SetBullet(bul_no, gMC.x + 0xC00, gMC.y + 0x400, 2);
					SetCaret(gMC.x + 0x1800, gMC.y + 0x400, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x - 0xC00, gMC.y + 0x400, 0);
					SetCaret(gMC.x - 0x1800, gMC.y + 0x400, 3, 0);
				}
			}
			
			PlaySoundObject(34, 1);
		}
	}
}

void ShootBullet_Machinegun1(int level)
{
	if (CountArmsBullet(4) < 5)
	{
		int bul_no;

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
			if (++gMC.rensha > 6)
			{
				gMC.rensha = 0;
				if (!UseArmsEnergy(1))
				{
					PlaySoundObject(37, 1);
					
					if (!empty)
					{
						SetCaret(gMC.x, gMC.y, 16, 0);
						empty = 50;
					}
				}
				else
				{
					if (gMC.up)
					{
						if (level == 3)
							gMC.ym += 0x100;
					
						if (gMC.direct)
						{
							SetBullet(bul_no, gMC.x + 0x600, gMC.y - 0x1000, 1);
							SetCaret(gMC.x + 0x600, gMC.y - 0x1000, 3, 0);
						}
						else
						{
							SetBullet(bul_no, gMC.x - 0x600, gMC.y - 0x1000, 1);
							SetCaret(gMC.x - 0x600, gMC.y - 0x1000, 3, 0);
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
						
						if (gMC.direct)
						{
							SetBullet(bul_no, gMC.x + 0x600, gMC.y + 0x1000, 3);
							SetCaret(gMC.x + 0x600, gMC.y + 0x1000, 3, 0);
						}
						else
						{
							SetBullet(bul_no, gMC.x - 0x600, gMC.y + 0x1000, 3);
							SetCaret(gMC.x - 0x600, gMC.y + 0x1000, 3, 0);
						}
					}
					else
					{
						if (gMC.direct)
						{
							SetBullet(bul_no, gMC.x + 0x1800, gMC.y + 0x600, 2);
							SetCaret(gMC.x + 0x1800, gMC.y + 0x600, 3, 0);
						}
						else
						{
							SetBullet(bul_no, gMC.x - 0x1800, gMC.y + 0x600, 0);
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
}

void ShootBullet_Missile(int level, bool bSuper)
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
		
		if (level == 1)
		{
			if (CountArmsBullet(10) >= 1 || CountArmsBullet(11) >= 1)
				return;
		}
		else if (level == 2)
		{
			if (CountArmsBullet(10) >= 2 || CountArmsBullet(11) >= 2)
				return;
		}
		else if (level == 3)
		{
			if (CountArmsBullet(10) >= 4 || CountArmsBullet(11) >= 4)
				return;
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
		
		if (level == 1)
		{
			if (CountArmsBullet(5) >= 1 || CountArmsBullet(6) >= 1)
				return;
		}
		else if (level == 2)
		{
			if (CountArmsBullet(5) >= 2 || CountArmsBullet(6) >= 2)
				return;
		}
		else if ( level == 3 )
		{
			if (CountArmsBullet(5) >= 4 || CountArmsBullet(6) >= 4)
				return;
		}
	}
	
	if (!(gKeyTrg & gKeyShot))
		return;

	if (level < 3)
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);

			if (!empty)
			{
				SetCaret(gMC.x, gMC.y, 16, 0);
				empty = 50;
			}

			return;
		}

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
				SetBullet(bul_no, gMC.x + 0xC00, gMC.y, 2);
				SetCaret(gMC.x + 0x1800, gMC.y, 3, 0);
			}
			else
			{
				SetBullet(bul_no, gMC.x - 0xC00, gMC.y, 0);
				SetCaret(gMC.x - 0x1800, gMC.y, 3, 0);
			}
		}
	}
	else
	{
		if (!UseArmsEnergy(1))
		{
			PlaySoundObject(37, 1);

			if (!empty)
			{
				SetCaret(gMC.x, gMC.y, 16, 0);
				empty = 50;
			}

			return;
		}

		if (gMC.up)
		{
			if (gMC.direct)
			{
				SetBullet(bul_no, gMC.x + 0x200, gMC.y - 0x1000, 1);
				SetCaret(gMC.x + 0x200, gMC.y - 0x1000, 3, 0);
				SetBullet(bul_no, gMC.x + 0x600, gMC.y, 1);
				SetBullet(bul_no, gMC.x - 0x600, gMC.y, 1);
			}
			else
			{
				SetBullet(bul_no, gMC.x - 0x200, gMC.y - 0x1000, 1);
				SetCaret(gMC.x - 0x200, gMC.y - 0x1000, 3, 0);
				SetBullet(bul_no, gMC.x + 0x600, gMC.y, 1);
				SetBullet(bul_no, gMC.x - 0x600, gMC.y, 1);
			}
		}
		else if (gMC.down)
		{
			if (gMC.direct)
			{
				SetBullet(bul_no, gMC.x + 0x200, gMC.y + 0x1000, 3);
				SetCaret(gMC.x + 0x200, gMC.y + 0x1000, 3, 0);
				SetBullet(bul_no, gMC.x - 0x600, gMC.y, 3);
				SetBullet(bul_no, gMC.x + 0x600, gMC.y, 3);
			}
			else
			{
				SetBullet(bul_no, gMC.x - 0x200, gMC.y + 0x1000, 3);
				SetCaret(gMC.x - 0x200, gMC.y + 0x1000, 3, 0);
				SetBullet(bul_no, gMC.x + 0x600, gMC.y, 3);
				SetBullet(bul_no, gMC.x - 0x600, gMC.y, 3);
			}
		}
		else
		{
			if (gMC.direct)
			{
				SetBullet(bul_no, gMC.x + 0xC00, gMC.y + 0x200, 2);
				SetCaret(gMC.x + 0x1800, gMC.y + 0x200, 3, 0);
				SetBullet(bul_no, gMC.x, gMC.y - 0x1000, 2);
				SetBullet(bul_no, gMC.x - 0x800, gMC.y - 0x200, 2);
			}
			else
			{
				SetBullet(bul_no, gMC.x - 0xC00, gMC.y + 0x200, 0);
				SetCaret(gMC.x - 0x1800, gMC.y + 0x200, 3, 0);
				SetBullet(bul_no, gMC.x, gMC.y - 0x1000, 0);
				SetBullet(bul_no, gMC.x + 0x800, gMC.y - 0x200, 0);
			}
		}
	}

	PlaySoundObject(32, 1);
}

void ShootBullet_Bubblin1(void)
{
	static int wait;

	if (CountArmsBullet(7) <= 3)
	{
		if (gKeyTrg & gKeyShot)
		{
			if (!UseArmsEnergy(1))
			{
				PlaySoundObject(37, 1);
				if (!empty)
				{
					SetCaret(gMC.x, gMC.y, 16, 0);
					empty = 50;
				}
			}
			else
			{
				if (gMC.up)
				{
					if (gMC.direct)
					{
						SetBullet(19, gMC.x + 0x200, gMC.y - 0x400, 1);
						SetCaret(gMC.x + 0x200, gMC.y - 0x400, 3, 0);
					}
					else
					{
						SetBullet(19, gMC.x - 0x200, gMC.y - 0x400, 1);
						SetCaret(gMC.x - 0x200, gMC.y - 0x400, 3, 0);
					}
				}
				else if (gMC.down)
				{
					if (gMC.direct)
					{
						SetBullet(19, gMC.x + 0x200, gMC.y + 0x400, 3);
						SetCaret(gMC.x + 0x200, gMC.y + 0x400, 3, 0);
					}
					else
					{
						SetBullet(19, gMC.x - 0x200, gMC.y + 0x400, 3);
						SetCaret(gMC.x - 0x200, gMC.y + 0x400, 3, 0);
					}
				}
				else if (gMC.direct)
				{
					SetBullet(19, gMC.x + 0xC00, gMC.y + 0x600, 2);
					SetCaret(gMC.x + 0x1800, gMC.y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(19, gMC.x - 0xC00, gMC.y + 0x600, 0);
					SetCaret(gMC.x - 0x1800, gMC.y + 0x600, 3, 0);
				}
	
				PlaySoundObject(48, 1);
			}
		}
		else if (++wait > 20)
		{
			wait = 0;
			ChargeArmsEnergy(1);
		}
	}
}

void ShootBullet_Bubblin2(int level)
{
	static int wait;

	if (CountArmsBullet(7) <= 15)
	{
		level += 18;

		if (!(gKey & gKeyShot))
			gMC.rensha = 6;

		if (gKey & gKeyShot)
		{
			if (++gMC.rensha >= 7)
			{
				gMC.rensha = 0;

				if (!UseArmsEnergy(1))
				{
					PlaySoundObject(37, 1);
					if (!empty)
					{
						SetCaret(gMC.x, gMC.y, 16, 0);
						empty = 50;
					}
				}
				else
				{
					if (gMC.up)
					{
						if (gMC.direct)
						{
							SetBullet(level, gMC.x + 0x600, gMC.y - 0x1000, 1);
							SetCaret(gMC.x + 0x600, gMC.y - 0x2000, 3, 0);
						}
						else
						{
							SetBullet(level, gMC.x - 0x600, gMC.y - 0x1000, 1);
							SetCaret(gMC.x - 0x600, gMC.y - 0x2000, 3, 0);
						}
					}
					else if (gMC.down)
					{
						if (gMC.direct)
						{
							SetBullet(level, gMC.x + 0x600, gMC.y + 0x1000, 3);
							SetCaret(gMC.x + 0x600, gMC.y + 0x2000, 3, 0);
						}
						else
						{
							SetBullet(level, gMC.x - 0x600, gMC.y + 0x1000, 3);
							SetCaret(gMC.x - 0x600, gMC.y + 0x2000, 3, 0);
						}
					}
					else if (gMC.direct)
					{
						SetBullet(level, gMC.x + 0xC00, gMC.y + 0x600, 2);
						SetCaret(gMC.x + 0x1800, gMC.y + 0x600, 3, 0);
					}
					else
					{
						SetBullet(level, gMC.x - 0xC00, gMC.y + 0x600, 0);
						SetCaret(gMC.x - 0x1800, gMC.y + 0x600, 3, 0);
					}

					PlaySoundObject(48, 1);
				}
			}
		}
		else if (++wait > 1)
		{
			wait = 0;
			ChargeArmsEnergy(1);
		}
	}
}

void ShootBullet_Sword(int level)
{
	if (CountArmsBullet(9) <= 0)
	{
		int bul_no;

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
					SetBullet(bul_no, gMC.x - 0x200, gMC.y + 0x800, 1);
				else
					SetBullet(bul_no, gMC.x + 0x200, gMC.y + 0x800, 1);
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
					SetBullet(bul_no, gMC.x - 0x200, gMC.y - 0xC00, 3);
				else
					SetBullet(bul_no, gMC.x + 0x200, gMC.y - 0xC00, 3);
			}
			else
			{
				if (gMC.direct == 0)
					SetBullet(bul_no, gMC.x + 0xC00, gMC.y - 0x600, 0);
				else
					SetBullet(bul_no, gMC.x - 0xC00, gMC.y - 0x600, 2);
			}

			PlaySoundObject(34, 1);
		}
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

	if (CountArmsBullet(12) <= 1 && gKeyTrg & gKeyShot)
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
					SetBullet(bul_no, gMC.x - 0x200, gMC.y - 0x1800, 1);
					SetCaret(gMC.x - 0x200, gMC.y - 0x1800, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y - 0x1800, 1);
					SetCaret(gMC.x + 0x200, gMC.y - 0x1800, 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0x200, gMC.y + 0x1800, 3);
					SetCaret(gMC.x - 0x200, gMC.y + 0x1800, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y + 0x1800, 3);
					SetCaret(gMC.x + 0x200, gMC.y + 0x1800, 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0x2C00, gMC.y + 0x600, 0);
					SetCaret(gMC.x - 0x2000, gMC.y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x2C00, gMC.y + 0x600, 2);
					SetCaret(gMC.x + 0x2000, gMC.y + 0x600, 3, 0);
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
	int bul_no;

	bool bShot = false;

	if (gKey & gKeyShot)
	{
		if (gMC.equip & 8)
			AddExpMyChar(3);
		else
			AddExpMyChar(2);

		if (++spur_charge / 2 % 2)
		{
			if (level == 1)
			{
				PlaySoundObject(59, 1);
			}
			else if (level == 2)
			{
				PlaySoundObject(60, 1);
			}
			else if (level == 3)
			{
				if (!IsMaxExpMyChar())
					PlaySoundObject(61, 1);
			}
		}
	}
	else
	{
		if (spur_charge)
			bShot = true;

		spur_charge = 0;
	}

	static bool bMax;

	if (IsMaxExpMyChar())
	{
		if (bMax == false)
		{
			bMax = true;
			PlaySoundObject(65, 1);
		}
	}
	else
	{
		bMax = false;
	}

	if (!(gKey & gKeyShot))
		ZeroExpMyChar();

	switch (level)
	{
		case 1:
			bul_no = 6;
			bShot = false;
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

	if (CountArmsBullet(13) <= 0 && CountArmsBullet(14) <= 0 && (gKeyTrg & gKeyShot || bShot))
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
					SetBullet(bul_no, gMC.x - 0x200, gMC.y - 0x1000, 1);
					SetCaret(gMC.x - 0x200, gMC.y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y - 0x1000, 1);
					SetCaret(gMC.x + 0x200, gMC.y - 0x1000, 3, 0);
				}
			}
			else if (gMC.down)
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0x200, gMC.y + 0x1000, 3);
					SetCaret(gMC.x - 0x200, gMC.y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0x200, gMC.y + 0x1000, 3);
					SetCaret(gMC.x + 0x200, gMC.y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (gMC.direct == 0)
				{
					SetBullet(bul_no, gMC.x - 0xC00, gMC.y + 0x600, 0);
					SetCaret(gMC.x - 0x1800, gMC.y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(bul_no, gMC.x + 0xC00, gMC.y + 0x600, 2);
					SetCaret(gMC.x + 0x1800, gMC.y + 0x600, 3, 0);
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
				ShootBullet_Missile(gArmsData[gSelectedArms].level, false);
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
				ShootBullet_Missile(gArmsData[gSelectedArms].level, true);
				break;
			case 12:
				ShootBullet_Nemesis(gArmsData[gSelectedArms].level);
				break;
			case 13:
				ShootBullet_Spur(gArmsData[gSelectedArms].level);
				break;
		}
	}
}
