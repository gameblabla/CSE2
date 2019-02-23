#include "NpChar.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "CommonDefines.h"
#include "Caret.h"
#include "Draw.h"
#include "File.h"
#include "Flags.h"
#include "Game.h"
#include "MyChar.h"
#include "NpcTbl.h"
#include "Sound.h"
#include "Tags.h"
#include "ValueView.h"

NPCHAR gNPC[NPC_MAX];
int gCurlyShoot_wait;
int gCurlyShoot_x;
int gCurlyShoot_y;
int gSuperXpos;
int gSuperYpos;

const char *gPassPixEve = "PXE\0";

void InitNpChar()
{
	memset(gNPC, 0, sizeof(gNPC));
}

void SetUniqueParameter(NPCHAR *npc)
{
	int code = npc->code_char;
	npc->surf = (Surface_Ids)gNpcTable[code].surf;
	npc->hit_voice = gNpcTable[code].hit_voice;
	npc->destroy_voice = gNpcTable[code].destroy_voice;
	npc->damage = gNpcTable[code].damage;
	npc->size = gNpcTable[code].size;
	npc->life = gNpcTable[code].life;
	npc->hit.front = gNpcTable[code].hit.front << 9;
	npc->hit.back = gNpcTable[code].hit.back << 9;
	npc->hit.top = gNpcTable[code].hit.top << 9;
	npc->hit.bottom = gNpcTable[code].hit.bottom << 9;
	npc->view.front = gNpcTable[code].view.front << 9;
	npc->view.back = gNpcTable[code].view.back << 9;
	npc->view.top = gNpcTable[code].view.top << 9;
	npc->view.bottom = gNpcTable[code].view.bottom << 9;
}

bool LoadEvent(char *path_event)
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gDataPath, path_event);

	FILE *fp = fopen(path, "rb");
	if (fp == NULL)
		return false;

	//Read "PXE" check
	char code[4];
	fread(code, 1, 4, fp);
	if (memcmp(code, gPassPixEve, 3))
	{
#ifdef FIX_BUGS
		// The original game forgot to close the file here
		fclose(fp);
#endif
		return false;
	}

	//Get amount of NPCs
	int count = File_ReadLE32(fp);

	//Load NPCs
	memset(gNPC, 0, sizeof(gNPC));

	int n = 170;
	for (int i = 0; i < count; i++)
	{
		//Get data from file
		EVENT eve;
		eve.x = File_ReadLE16(fp);
		eve.y = File_ReadLE16(fp);
		eve.code_flag = File_ReadLE16(fp);
		eve.code_event = File_ReadLE16(fp);
		eve.code_char = File_ReadLE16(fp);
		eve.bits = File_ReadLE16(fp);

		//Set NPC parameters
		if (eve.bits & npc_altDir)
			gNPC[n].direct = 2;
		else
			gNPC[n].direct = 0;
		gNPC[n].code_char = eve.code_char;
		gNPC[n].code_event = eve.code_event;
		gNPC[n].code_flag = eve.code_flag;
		gNPC[n].x = eve.x << 13;
		gNPC[n].y = eve.y << 13;
		gNPC[n].bits = eve.bits;
		gNPC[n].bits |= gNpcTable[gNPC[n].code_char].bits;
		gNPC[n].exp = gNpcTable[gNPC[n].code_char].exp;
		SetUniqueParameter(&gNPC[n]);

		//Check flags
		if (gNPC[n].bits & npc_appearSet)
		{
			if (GetNPCFlag(gNPC[n].code_flag))
				gNPC[n].cond |= 0x80u;
		}
		else if (gNPC[n].bits & npc_hideSet)
		{
			if (!GetNPCFlag(gNPC[n].code_flag))
				gNPC[n].cond |= 0x80u;
		}
		else
		{
			gNPC[n].cond = 0x80;
		}

		//Increase index
		n++;
	}

	fclose(fp);
	return true;
}

void SetNpChar(int code_char, int x, int y, int xm, int ym, int dir, NPCHAR *npc, int start_index)
{
	for (int n = start_index; n < NPC_MAX; n++)
	{
		if (!gNPC[n].cond)
		{
			//Set NPC parameters
			memset(&gNPC[n], 0, sizeof(NPCHAR));
			gNPC[n].cond |= 0x80u;
			gNPC[n].direct = dir;
			gNPC[n].code_char = code_char;
			gNPC[n].x = x;
			gNPC[n].y = y;
			gNPC[n].xm = xm;
			gNPC[n].ym = ym;
			gNPC[n].pNpc = npc;
			gNPC[n].bits = gNpcTable[gNPC[n].code_char].bits;
			gNPC[n].exp = gNpcTable[gNPC[n].code_char].exp;
			SetUniqueParameter(&gNPC[n]);
			break;
		}
	}
}

void SetDestroyNpChar(int x, int y, int w, int num)
{
	//Create smoke
	int wa = w / 0x200;
	for (int i = 0; i < num; i++)
	{
		int offset_x = Random(-wa, wa) << 9;
		int offset_y = Random(-wa, wa) << 9;
		SetNpChar(4, x + offset_x, offset_y + y, 0, 0, 0, NULL, 0x100);
	}

	//Flash effect
	SetCaret(x, y, 12, 0);
}

void SetDestroyNpCharUp(int x, int y, int w, int num)
{
	//Create smoke
	int wa = w / 0x200;
	for (int i = 0; i < num; i++)
	{
		int offset_x = Random(-wa, wa) << 9;
		int offset_y = Random(-wa, wa) << 9;
		SetNpChar(4, x + offset_x, offset_y + y, 0, 0, 1, NULL, 0x100);
	}

	//Flash effect
	SetCaret(x, y, 12, 0);
}

void SetExpObjects(int x, int y, int exp)
{
	int sub_exp;
	for (int n = 0x100; exp; SetUniqueParameter(&gNPC[n]))
	{
		while (true)
		{
			bool v3 = n < NPC_MAX && gNPC[n].cond;
			if (!v3)
				break;
			++n;
		}
		
		if (n == NPC_MAX)
			break;
		
		memset(&gNPC[n], 0, sizeof(NPCHAR));

		if (exp < 20)
		{
			if (exp < 5)
			{
				if (exp > 0)
				{
					--exp;
					sub_exp = 1;
				}
			}
			else
			{
				exp -= 5;
				sub_exp = 5;
			}
		}
		else
		{
			exp -= 20;
			sub_exp = 20;
		}

		gNPC[n].cond |= 0x80u;
		gNPC[n].direct = 0;
		gNPC[n].code_char = 1;
		gNPC[n].x = x;
		gNPC[n].y = y;
		gNPC[n].bits = gNpcTable[gNPC[n].code_char].bits;
		gNPC[n].exp = sub_exp;
	}
}

bool SetBulletObject(int x, int y, int val)
{
	int tamakazu_ari[10];

	int n;
	int t = 0;
	memset(tamakazu_ari, 0, sizeof(tamakazu_ari));
	for (n = 0; n < 8; n++)
	{
		int code = gArmsData[n].code;
		if (code == 5)
			tamakazu_ari[t++] = 0;
		else if (code == 10)
			tamakazu_ari[t++] = 1;
		else
			tamakazu_ari[t] = 0;
	}

	if (!t)
		return false;

	n = Random(1, 10 * t);
	int bullet_no = tamakazu_ari[n % t];
	for (n = 0x100; n < NPC_MAX; n++)
	{
		if (!gNPC[n].cond)
		{
			memset(&gNPC[n], 0, sizeof(NPCHAR));
			gNPC[n].cond |= 0x80u;
			gNPC[n].direct = 0;
			gNPC[n].code_event = bullet_no;
			gNPC[n].code_char = 86;
			gNPC[n].x = x;
			gNPC[n].y = y;
			gNPC[n].bits = gNpcTable[gNPC[n].code_char].bits;
			gNPC[n].exp = val;
			SetUniqueParameter(&gNPC[n]);
			return true;
		}
	}

	return false;
}

bool SetLifeObject(int x, int y, int val)
{
	for (int n = 0x100; n < NPC_MAX; n++)
	{
		if (!gNPC[n].cond)
		{
			memset(&gNPC[n], 0, sizeof(NPCHAR));
			gNPC[n].cond |= 0x80u;
			gNPC[n].direct = 0;
			gNPC[n].code_char = 87;
			gNPC[n].x = x;
			gNPC[n].y = y;
			gNPC[n].bits = gNpcTable[gNPC[n].code_char].bits;
			gNPC[n].exp = val;
			SetUniqueParameter(&gNPC[n]);
			return true;
		}
	}

	return false;
}

void VanishNpChar(NPCHAR *npc)
{
	int x = npc->x;
	int y = npc->y;
	memset(npc, 0, sizeof(NPCHAR));
	npc->count1 = 0;
	npc->x = x;
	npc->y = y;
	npc->cond |= 0x80u;
	npc->direct = 0;
	npc->code_char = 3;
	npc->bits = gNpcTable[npc->code_char].bits;
	npc->exp = gNpcTable[npc->code_char].exp;
	SetUniqueParameter(npc);
}

void PutNpChar(int fx, int fy)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if (gNPC[n].cond & 0x80)
		{
			int8_t a;

			if (gNPC[n].shock)
			{
				a = 2 * ((gNPC[n].shock >> 1) & 1) - 1;
			}
			else
			{
				a = 0;
				if (gNPC[n].bits & npc_showDamage && gNPC[n].damage_view)
				{
					SetValueView(&gNPC[n].x, &gNPC[n].y, gNPC[n].damage_view);
					gNPC[n].damage_view = 0;
				}
			}

			int side;
			if (gNPC[n].direct)
				side = gNPC[n].view.back;
			else
				side = gNPC[n].view.front;

			PutBitmap3(
				&grcGame,
				(gNPC[n].x - side) / 0x200 - fx / 0x200 + a,
				(gNPC[n].y - gNPC[n].view.top) / 0x200 - fy / 0x200,
				&gNPC[n].rect,
				(Surface_Ids)gNPC[n].surf);
		}
	}
}

void ActNpChar()
{
	for (int i = 0; i < NPC_MAX; i++)
	{
		if (gNPC[i].cond & 0x80)
		{
			gpNpcFuncTbl[gNPC[i].code_char](&gNPC[i]);

			if (gNPC[i].shock)
				--gNPC[i].shock;
		}
	}
}

void ChangeNpCharByEvent(int code_event, int code_char, int dir)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80u) && gNPC[n].code_event == code_event)
		{
			gNPC[n].bits &= (npc_eventTouch | npc_eventDie | 0x400 | npc_appearSet | npc_altDir | npc_interact | npc_hideSet);
			gNPC[n].code_char = code_char;
			gNPC[n].bits |= gNpcTable[gNPC[n].code_char].bits;
			gNPC[n].exp = gNpcTable[gNPC[n].code_char].exp;
			SetUniqueParameter(&gNPC[n]);
			gNPC[n].cond |= 0x80u;
			gNPC[n].act_no = 0;
			gNPC[n].act_wait = 0;
			gNPC[n].count1 = 0;
			gNPC[n].count2 = 0;
			gNPC[n].ani_no = 0;
			gNPC[n].ani_wait = 0;
			gNPC[n].xm = 0;
			gNPC[n].ym = 0;
			
			if (dir != 5)
			{
				if (dir == 4)
				{
					if (gNPC[n].x >= gMC.x)
						gNPC[n].direct = 0;
					else
						gNPC[n].direct = 2;
				}
				else
				{
					gNPC[n].direct = dir;
				}
			}
			
			gpNpcFuncTbl[code_char](&gNPC[n]);
		}
	}
}

void ChangeCheckableNpCharByEvent(int code_event, int code_char, int dir)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80u) != 0 && gNPC[n].code_event == code_event)
		{
			gNPC[n].bits &= (npc_eventTouch | npc_eventDie | 0x400 | npc_appearSet | npc_altDir | npc_interact | npc_hideSet);
			gNPC[n].bits |= npc_interact;
			gNPC[n].code_char = code_char;
			gNPC[n].bits |= gNpcTable[gNPC[n].code_char].bits;
			gNPC[n].exp = gNpcTable[gNPC[n].code_char].exp;
			SetUniqueParameter(&gNPC[n]);
			gNPC[n].cond |= 0x80u;
			gNPC[n].act_no = 0;
			gNPC[n].act_wait = 0;
			gNPC[n].count1 = 0;
			gNPC[n].count2 = 0;
			gNPC[n].ani_no = 0;
			gNPC[n].ani_wait = 0;
			gNPC[n].xm = 0;
			gNPC[n].ym = 0;
			
			if (dir != 5)
			{
				if (dir == 4)
				{
					if (gNPC[n].x >= gMC.x)
						gNPC[n].direct = 0;
					else
						gNPC[n].direct = 2;
				}
				else
				{
					gNPC[n].direct = dir;
				}
			}
			
			gpNpcFuncTbl[code_char](&gNPC[n]);
		}
	}
}

void SetNpCharActionNo(int code_event, int act_no, int dir)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80) && gNPC[n].code_event == code_event)
		{
			gNPC[n].act_no = act_no;
			
			if (dir != 5)
			{
				if (dir == 4)
				{
					if (gNPC[n].x >= gMC.x)
						gNPC[n].direct = 0;
					else
						gNPC[n].direct = 2;
				}
				else
				{
					gNPC[n].direct = dir;
				}
			}
			break;
		}
	}
}

void MoveNpChar(int code_event, int x, int y, int dir)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80) && gNPC[n].code_event == code_event)
		{
			gNPC[n].x = x;
			gNPC[n].y = y;
			
			if (dir != 5)
			{
				if (dir == 4)
				{
					if (gNPC[n].x >= gMC.x)
						gNPC[n].direct = 0;
					else
						gNPC[n].direct = 2;
				}
				else
				{
					gNPC[n].direct = dir;
				}
			}
			break;
		}
	}
}

void BackStepMyChar(int code_event)
{
	gMC.cond &= ~1;
	gMC.ym = -0x200;
	
	if (code_event)
	{
		if (code_event == 2)
		{
			gMC.direct = 2;
			gMC.xm = -0x200;
		}
		else
		{
			for (int n = 0; n < NPC_MAX; n++)
			{
				if ((gNPC[n].cond & 0x80) && gNPC[n].code_event == code_event)
				{
					if (gNPC[n].x >= gMC.x)
					{
						gMC.direct = 2;
						gMC.xm = -0x200;
					}
					else
					{
						gMC.direct = 0;
						gMC.xm = 0x200;
					}
				}
			}
		}
	}
	else
	{
		gMC.direct = 0;
		gMC.xm = 0x200;
	}
}

void DeleteNpCharEvent(int code)
{
	for (int i = 0; i < NPC_MAX; i++)
	{
		if ((gNPC[i].cond & 0x80) && gNPC[i].code_event == code)
		{
			gNPC[i].cond = 0;
			SetNPCFlag(gNPC[i].code_flag);
		}
	}
}

void DeleteNpCharCode(int code, bool bSmoke)
{
	for (int n = 0; n < NPC_MAX; n++)
	{
		if ((gNPC[n].cond & 0x80) && gNPC[n].code_char == code)
		{
			gNPC[n].cond = 0;
			SetNPCFlag(gNPC[n].code_flag);
			
			if (bSmoke)
			{
				PlaySoundObject(gNPC[n].destroy_voice, 1);
				
				switch (gNPC[n].size)
				{
					case 2:
						SetDestroyNpChar(gNPC[n].x, gNPC[n].y, gNPC[n].view.back, 8);
						break;
					case 3:
						SetDestroyNpChar(gNPC[n].x, gNPC[n].y, gNPC[n].view.back, 16);
						break;
					case 1:
						SetDestroyNpChar(gNPC[n].x, gNPC[n].y, gNPC[n].view.back, 4);
						break;
				}
			}
		}
	}
}

void GetNpCharPosition(int *x, int *y, int i)
{
  *x = gNPC[i].x;
  *y = gNPC[i].y;
}

BOOL IsNpCharCode(int code)
{
	for (int i = 0; i < NPC_MAX; i++)
	{
		if ((gNPC[i].cond & 0x80) && gNPC[i].code_char == code)
			return TRUE;
	}
	
	return FALSE;
}

BOOL GetNpCharAlive(int code_event)
{
	for (int i = 0; i < NPC_MAX; i++)
	{
		if ((gNPC[i].cond & 0x80) && gNPC[i].code_event == code_event)
			return TRUE;
	}
	
	return FALSE;
}

int CountAliveNpChar()
{
	int count = 0;
	for (int n = 0; n < NPC_MAX; ++n)
	{
		if (gNPC[n].cond & 0x80)
			++count;
	}
	
	return count;
}
