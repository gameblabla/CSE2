#include "WindowsWrapper.h"

#include "NpcAct.h"

#include "MyChar.h"
#include "NpChar.h"
#include "Game.h"
#include "Sound.h"
#include "Back.h"
#include "Triangle.h"
#include "Caret.h"
#include "Map.h"
#include "Frame.h"

//Igor (enemy)
void ActNpc268(NPCHAR *npc)
{
	RECT rcLeft[10];
	RECT rcRight[10];

	rcLeft[0] = {0, 0, 40, 40};
	rcLeft[1] = {40, 0, 80, 40};
	rcLeft[2] = {80, 0, 120, 40};
	rcLeft[3] = {0, 0, 40, 40};
	rcLeft[4] = {120, 0, 160, 40};
	rcLeft[5] = {0, 0, 40, 40};
	rcLeft[6] = {40, 80, 80, 120};
	rcLeft[7] = {0, 80, 40, 120};
	rcLeft[8] = {240, 0, 280, 40};
	rcLeft[9] = {280, 0, 320, 40};

	rcRight[0] = {0, 40, 40, 80};
	rcRight[1] = {40, 40, 80, 80};
	rcRight[2] = {80, 40, 120, 80};
	rcRight[3] = {0, 40, 40, 80};
	rcRight[4] = {120, 40, 160, 80};
	rcRight[5] = {0, 40, 40, 80};
	rcRight[6] = {160, 80, 200, 120};
	rcRight[7] = {120, 80, 160, 120};
	rcRight[8] = {240, 40, 280, 80};
	rcRight[9] = {280, 40, 320, 80};

	if (npc->x < gMC.x - 0x28000 || npc->x > gMC.x + 0x28000 || npc->y < gMC.y - 0x1E000 || npc->y > gMC.y + 0x1E000)
		npc->act_no = 1;

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y += 0x1000;
			// Fallthrough
		case 1:
			if (++npc->ani_wait > 20)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->x < gMC.x + 0xE000 && npc->x > gMC.x - 0xE000 && npc->x < gMC.x + 0x6000 && npc->x > gMC.x - 0xE000)
				npc->act_no = 10;

			if (npc->shock)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->ani_no = 0;
			npc->ani_wait = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 11:
			if (npc->direct == 0)
				npc->xm = -0x200;
			else
				npc->xm = 0x200;

			if (npc->x < gMC.x + 0x8000 && npc->x > gMC.x - 0x8000)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
			}

			if (npc->xm < 0 && npc->flag & 1)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
			}

			if (npc->xm > 0 && npc->flag & 4)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
			}

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 5)
				npc->ani_no = 2;

			break;

		case 20:
			npc->xm = 0;
			npc->ani_no = 6;

			if (++npc->act_wait > 10)
			{
				npc->act_no = 30;
				npc->ym = -0x5FF;

				if (npc->direct == 0)
					npc->xm = -0x200;
				else
					npc->xm = 0x200;

				PlaySoundObject(108, 1);
			}

			break;

		case 30:
			npc->ani_no = 7;

			if (npc->flag & 8)
			{
				npc->act_no = 40;
				npc->act_wait = 0;
				SetQuake(20);
				PlaySoundObject(26, 1);
			}

			break;

		case 40:
			npc->xm = 0;
			npc->ani_no = 6;

			if (++npc->act_wait > 30)
				npc->act_no = 50;

			break;

		case 50:
			npc->act_no = 51;
			npc->act_wait = 0;

			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;
			// Fallthrough
		case 51:
			if (++npc->act_wait > 30 && npc->act_wait % 4 == 1)
			{
				unsigned char deg;

				if (npc->direct == 0)
					deg = -120;
				else
					deg = -8;

				deg += Random(-0x10, 0x10);
				const int ym = 5 * GetSin(deg);
				const int xm = 5 * GetCos(deg);
				SetNpChar(11, npc->x, npc->y + 0x800, xm, ym, 0, 0, 0x100);
				PlaySoundObject(12, 1);
			}

			if (npc->act_wait < 50 && npc->act_wait / 2 % 2)
				npc->ani_no = 9;
			else
				npc->ani_no = 8;

			if (npc->act_wait > 82)
			{
				npc->act_no = 10;

				if (gMC.x < npc->x)
					npc->direct = 0;
				else
					npc->direct = 2;
			}

			break;
	}

	npc->ym += 0x33;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Ironhead block
void ActNpc271(NPCHAR *npc)
{
	if (npc->xm < 0 && npc->x < -0x2000)
	{
		VanishNpChar(npc);
	}
	else
	{
		if (npc->xm > 0 && npc->x > (gMap.width + 1) * 0x2000)
		{
			VanishNpChar(npc);
		}
		else
		{
			if (npc->act_no == 0)
			{
				npc->act_no = 1;
				int a = Random(0, 9);

				if (a == 9)
				{
					npc->rect.left = 0;
					npc->rect.right = 0x20;
					npc->rect.top = 0x40;
					npc->rect.bottom = 0x60;
					npc->view.front = 0x2000;
					npc->view.back = 0x2000;
					npc->view.top = 0x2000;
					npc->view.bottom = 0x2000;
					npc->hit.front = 0x1800;
					npc->hit.back = 0x1800;
					npc->hit.top = 0x1800;
					npc->hit.bottom = 0x1800;
				}
				else
				{
					npc->rect.left = 16 * (a % 3 + 7);
					npc->rect.top = 16 * (a / 3);
					npc->rect.right = npc->rect.left + 16;
					npc->rect.bottom = npc->rect.top + 16;
				}

				if (npc->direct == 0)
					npc->xm = -2 * Random(0x100, 0x200);
				else
					npc->xm = 2 * Random(0x100, 0x200);

				npc->ym = Random(-0x200, 0x200);
			}

			if (npc->ym < 0 && npc->y - npc->hit.top <= 0xFFF)
			{
				npc->ym = -npc->ym;
				SetCaret(npc->x, npc->y - 0x1000, 13, 0);
				SetCaret(npc->x, npc->y - 0x1000, 13, 0);
			}

			if (npc->ym > 0 && npc->y + npc->hit.bottom > 0x1D000)
			{
				npc->ym = -npc->ym;
				SetCaret(npc->x, npc->y + 0x1000, 13, 0);
				SetCaret(npc->x, npc->y + 0x1000, 13, 0);
			}

			npc->x += npc->xm;
			npc->y += npc->ym;
		}
	}
}

// Ironhead block generator
void ActNpc272(NPCHAR *npc)
{
	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->act_wait = Random(0, 200);
			// Fallthrough
		case 1:
			if (npc->act_wait)
			{
				--npc->act_wait;
			}
			else
			{
				npc->act_no = 0;
				SetNpChar(271, npc->x, npc->y + (Random(-32, 32) * 0x200), 0, 0, npc->direct, 0, 0x100);
			}

			break;
	}
}

//Droll projectile
void ActNpc273(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {248, 40, 272, 64};
	rc[1] = {272, 40, 296, 64};
	rc[2] = {296, 40, 320, 64};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->flag & 0xFF)
			{
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				VanishNpChar(npc);
				return;
			}

			if (++npc->act_wait % 5 == 0)
				PlaySoundObject(110, 1);

			if (++npc->ani_no > 2)
				npc->ani_no = 0;

			break;
	}

	npc->rect = rc[npc->ani_no];
}

//Droll
void ActNpc274(NPCHAR *npc)
{
	RECT rcLeft[6];
	RECT rcRight[6];

	rcLeft[0] = {0, 0, 32, 40};
	rcLeft[1] = {32, 0, 64, 40};
	rcLeft[2] = {64, 0, 96, 40};
	rcLeft[3] = {64, 80, 96, 120};
	rcLeft[4] = {96, 80, 128, 120};
	rcLeft[5] = {96, 0, 128, 40};

	rcRight[0] = {0, 40, 32, 80};
	rcRight[1] = {32, 40, 64, 80};
	rcRight[2] = {64, 40, 96, 80};
	rcRight[3] = {64, 120, 96, 160};
	rcRight[4] = {96, 120, 128, 160};
	rcRight[5] = {96, 40, 128, 80};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			npc->tgt_x = npc->x;
			// Fallthrough
		case 1:
			npc->xm = 0;
			npc->act_no = 2;
			npc->ani_no = 0;
			// Fallthrough
		case 2:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (++npc->ani_wait > 40)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->shock)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			npc->ani_no = 2;
			npc->act_wait = 0;
			// Fallthrough
		case 11:
			if (++npc->act_wait > 10)
			{
				npc->act_no = 12;
				npc->ani_no = 3;
				npc->ym = -0x600u;
				npc->count1 = 0;

				if (npc->tgt_x > npc->x)
					npc->xm = 0x200;
				else
					npc->xm = -0x200;
			}

			break;

		case 12:
			if (npc->ym > 0)
			{
				npc->ani_no = 4;

				if (npc->count1 == 0)
				{
					++npc->count1;
					const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - 0x1400 - gMC.y);
					const int ym = 4 * GetSin(deg);
					const int xm = 4 * GetCos(deg);
					SetNpChar(273, npc->x, npc->y - 0x1400, xm, ym, 0, 0, 0x100);
					PlaySoundObject(39, 1);
				}
			}

			if (npc->ym > 0x200)
				npc->ani_no = 5;

			if (npc->flag & 8)
			{
				npc->ani_no = 2;
				npc->act_no = 13;
				npc->act_wait = 0;
				npc->xm = 0;
			}

			break;

		case 13:
			npc->xm /= 2;

			if (++npc->act_wait > 10)
				npc->act_no = 1;

			break;
	}

	npc->ym += 0x55;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Puppy (plantation)
void ActNpc275(NPCHAR *npc)
{
	RECT rcRight[4];

	rcRight[0] = {272, 80, 288, 96};
	rcRight[1] = {288, 80, 304, 96};
	rcRight[2] = {272, 80, 288, 96};
	rcRight[3] = {304, 80, 320, 96};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			// Fallthrough
		case 1:
			if (Random(0, 120) == 10)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (gMC.x > npc->x - 0x8000 && gMC.x < npc->x + 0x8000 && gMC.y > npc->y - 0x4000 && gMC.y < npc->y + 0x2000)
			{
				if (++npc->ani_wait > 3)
				{
					npc->ani_wait = 0;
					++npc->ani_no;
				}

				if (npc->ani_no > 3)
					npc->ani_no = 2;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
	}

	npc->ym += 0x40;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	npc->rect = rcRight[npc->ani_no];
}

//Red Demon
void ActNpc276(NPCHAR *npc)
{
	RECT rcLeft[9];
	RECT rcRight[9];

	rcLeft[0] = {0, 64, 32, 104};
	rcLeft[1] = {32, 64, 64, 104};
	rcLeft[2] = {64, 64, 96, 104};
	rcLeft[3] = {96, 64, 128, 104};
	rcLeft[4] = {128, 64, 160, 104};
	rcLeft[5] = {160, 64, 192, 104};
	rcLeft[6] = {192, 64, 224, 104};
	rcLeft[7] = {224, 64, 256, 104};
	rcLeft[8] = {256, 64, 288, 104};

	rcRight[0] = {0, 104, 32, 144};
	rcRight[1] = {32, 104, 64, 144};
	rcRight[2] = {64, 104, 96, 144};
	rcRight[3] = {96, 104, 128, 144};
	rcRight[4] = {128, 104, 160, 144};
	rcRight[5] = {160, 104, 192, 144};
	rcRight[6] = {192, 104, 224, 144};
	rcRight[7] = {224, 104, 256, 144};
	rcRight[8] = {256, 104, 288, 144};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->y -= 0x1000;
			// Fallthrough
		case 1:
			npc->xm = 0;
			npc->act_no = 2;
			npc->ani_no = 0;
			// Fallthrough
		case 2:
			if (gMC.x < npc->x)
				npc->direct = 0;
			else
				npc->direct = 2;

			if (++npc->ani_wait > 20)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (npc->shock)
				npc->act_no = 10;

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = 0;
			npc->ani_no = 3;
			npc->bits |= 0x20u;
			// Fallthrough
		case 11:
			switch (++npc->act_wait)
			{
				case 30:
				case 40:
				case 50:
				{
					npc->ani_no = 4;
					const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - gMC.y);
					const int ym = 4 * GetSin(deg);
					const int xm = 4 * GetCos(deg);
					SetNpChar(277, npc->x, npc->y, xm, ym, 0, 0, 0x100);
					PlaySoundObject(39, 1);
					break;
				}
				case 34:
				case 44:
				case 54:
				{
					npc->ani_no = 3;
					break;
				}
			}

			if (npc->act_wait > 60)
			{
				npc->act_no = 20;
				npc->act_wait = 0;
				npc->ani_no = 2;
			}

			break;

		case 20:
			if (++npc->act_wait > 20)
			{
				npc->act_no = 21;
				npc->act_wait = 0;
				npc->ani_no = 5;
				npc->ym = -0x5FF;
				if (gMC.x > npc->x)
					npc->xm = 0x100;
				else
					npc->xm = -0x100;
			}

			break;

		case 21:
			switch (++npc->act_wait)
			{
				case 30:
				case 40:
				case 50:
				{
					npc->ani_no = 6;
					const unsigned char deg = GetArktan(npc->x - gMC.x, npc->y - 0x1400 - gMC.y);
					const int ym = 4 * GetSin(deg);
					const int xm = 4 * GetCos(deg);
					SetNpChar(277, npc->x, npc->y - 0x1400, xm, ym, 0, 0, 0x100);
					PlaySoundObject(39, 1);
					break;
				}
				case 34:
				case 44:
				{
					npc->ani_no = 5;
					break;
				}
			}

			if (npc->act_wait > 53)
				npc->ani_no = 7;

			if (npc->flag & 8)
			{
				npc->act_no = 22;
				npc->act_wait = 0;
				npc->ani_no = 2;
				SetQuake(10);
				PlaySoundObject(26, 1);
			}

			break;

		case 22:
			npc->xm /= 2;

			if (++npc->act_wait > 22)
				npc->act_no = 10;

			break;

		case 50:
			npc->bits &= ~0x20;
			npc->damage = 0;

			if (npc->flag & 8)
			{
				npc->act_no = 51;
				npc->ani_no = 2;
				SetQuake(10);
				SetExpObjects(npc->x, npc->y, 19);
				SetDestroyNpChar(npc->x, npc->y, npc->view.back, 8);
				PlaySoundObject(72, 1);
			}

			break;

		case 51:
			npc->xm = 7 * npc->xm / 8;
			npc->ani_no = 8;
			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	if (npc->act_no < 50)
	{
		if (gMC.x > npc->x)
			npc->direct = 2;
		else
			npc->direct = 0;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

//Red Demon projectile
void ActNpc277(NPCHAR *npc)
{
	RECT rc[3];

	rc[0] = {128, 0, 152, 24};
	rc[1] = {152, 0, 176, 24};
	rc[2] = {176, 0, 200, 24};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			// Fallthrough
		case 1:
			npc->x += npc->xm;
			npc->y += npc->ym;

			if (npc->flag & 0xFF)
			{
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				SetNpChar(4, npc->x, npc->y, 0, 0, 0, 0, 0x100);
				VanishNpChar(npc);
				return;
			}

			if (++npc->act_wait % 5 == 0)
				PlaySoundObject(110, 1);

			if (++npc->ani_no > 2)
				npc->ani_no = 0;

			break;
	}

	npc->rect = rc[npc->ani_no];
}

//Little family
void ActNpc278(NPCHAR *npc)
{
	RECT rcMama[2];
	RECT rcPapa[2];
	RECT rcKodomo[2];

	rcPapa[0] = {0, 120, 8, 128};
	rcPapa[1] = {8, 120, 16, 128};

	rcMama[0] = {16, 120, 24, 128};
	rcMama[1] = {24, 120, 32, 128};

	rcKodomo[0] = {32, 120, 40, 128};
	rcKodomo[1] = {40, 120, 48, 128};

	switch (npc->act_no)
	{
		case 0:
			npc->act_no = 1;
			npc->ani_no = 0;
			npc->ani_wait = 0;
			npc->xm = 0;
			// Fallthrough
		case 1:
			if (Random(0, 60) == 1)
			{
				npc->act_no = 2;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			if (Random(0, 60) == 1)
			{
				npc->act_no = 10;
				npc->act_wait = 0;
				npc->ani_no = 1;
			}

			break;

		case 2:
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;

		case 10:
			npc->act_no = 11;
			npc->act_wait = Random(0, 16);
			npc->ani_no = 0;
			npc->ani_wait = 0;

			if (Random(0, 9) % 2)
				npc->direct = 0;
			else
				npc->direct = 2;

			// Fallthrough
		case 11:
			if (npc->direct == 0 && (npc->flag & 1))
				npc->direct = 2;
			else if ( npc->direct == 2 && npc->flag & 4 )
				npc->direct = 0;

			if (npc->direct == 0)
				npc->xm = -0x100;
			else
				npc->xm = 0x100;

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				++npc->ani_no;
			}

			if (npc->ani_no > 1)
				npc->ani_no = 0;

			if (++npc->act_wait > 0x20)
				npc->act_no = 0;

			break;
	}

	npc->ym += 0x20;
	if (npc->ym > 0x5FF)
		npc->ym = 0x5FF;

	npc->x += npc->xm;
	npc->y += npc->ym;

	
	switch (npc->code_event)
	{
		case 200:
			npc->rect = rcPapa[npc->ani_no];
			break;

		case 210:
			npc->rect = rcMama[npc->ani_no];
			break;

		default:
			npc->rect = rcKodomo[npc->ani_no];
			break;
	}
}
