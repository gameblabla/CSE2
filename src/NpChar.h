#pragma once

#include "WindowsWrapper.h"

#include "Draw.h"

#define NPC_MAX 0x200

enum NPC_flags
{
	npc_solidSoft = 0x1, //Pushes quote out
	npc_ignore44 = 0x2, //Ignores tile 44 (No NPC)
	npc_invulnerable = 0x4, //Can't get hit
	npc_ignoreSolid = 0x8, //Doesn't collide with anything
	npc_bouncy = 0x10, //Quote bounces on the top
	npc_shootable = 0x20, //Can be shot
	npc_solidHard = 0x40, //Essentially acts as level tiles
	npc_rearTop = 0x80, //Rear and top don't hurt
	npc_eventTouch = 0x100, //Run event when touched
	npc_eventDie = 0x200, //Run event when killed
	npc_appearSet = 0x800, //Only appear when flag is set
	npc_altDir = 0x1000, //Spawn facing to the right (or however the npc interprets the direction as)
	npc_interact = 0x2000, //Run event when interacted with
	npc_hideSet = 0x4000, //Hide when flag is set
	npc_showDamage = 0x8000 //Show #Damage taken
};

struct NPCHAR
{
	unsigned char cond;
	int flag;
	int x;
	int y;
	int xm;
	int ym;
	int xm2;
	int ym2;
	int tgt_x;
	int tgt_y;
	int code_char;
	int code_flag;
	int code_event;
	Surface_Ids surf;
	int hit_voice;
	int destroy_voice;
	int life;
	int exp;
	int size;
	int direct;
	unsigned short bits;
	RECT rect;
	int ani_wait;
	int ani_no;
	int count1;
	int count2;
	int act_no;
	int act_wait;
	struct
	{
		int front;
		int top;
		int back;
		int bottom;
	} hit;
	struct
	{
		int front;
		int top;
		int back;
		int bottom;
	} view;
	unsigned char shock;
	int damage_view;
	int damage;
	NPCHAR *pNpc;
};

struct EVENT
{
	short x;
	short y;
	short code_flag;
	short code_event;
	short code_char;
	unsigned short bits;
};

extern NPCHAR gNPC[NPC_MAX];
extern int gCurlyShoot_wait;
extern int gCurlyShoot_x;
extern int gCurlyShoot_y;
extern int gSuperXpos;
extern int gSuperYpos;

void InitNpChar();
BOOL LoadEvent(const char *path_event);
void SetNpChar(int code_char, int x, int y, int xm, int ym, int dir, NPCHAR *npc, int start_index);
void SetDestroyNpChar(int x, int y, int w, int num);
void SetDestroyNpCharUp(int x, int y, int w, int num);
void SetExpObjects(int x, int y, int exp);
BOOL SetBulletObject(int x, int y, int val);
BOOL SetLifeObject(int x, int y, int val);
void VanishNpChar(NPCHAR *npc);
void PutNpChar(int fx, int fy);
void ActNpChar();
void ChangeNpCharByEvent(int code_event, int code_char, int dir);
void ChangeCheckableNpCharByEvent(int code_event, int code_char, int dir);
void SetNpCharActionNo(int code_event, int act_no, int dir);
void MoveNpChar(int code_event, int x, int y, int dir);
void BackStepMyChar(int code_event);
void DeleteNpCharEvent(int code);
void DeleteNpCharCode(int code, BOOL bSmoke);
void GetNpCharPosition(int *x, int *y, int i);
BOOL IsNpCharCode(int code);
BOOL GetNpCharAlive(int code_event);
int CountAliveNpChar();
