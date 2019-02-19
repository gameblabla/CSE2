#pragma once

#include <stdint.h>

#include "Draw.h"
#include "NpChar.h"

struct NPC_TBL_RECT
{
  uint8_t front;
  uint8_t top;
  uint8_t back;
  uint8_t bottom;
};

struct NPC_TABLE
{
	uint16_t bits;
	uint16_t life;
	uint8_t surf;
	uint8_t hit_voice;
	uint8_t destroy_voice;
	uint8_t size;
	int32_t exp;
	int32_t damage;
	NPC_TBL_RECT hit;
	NPC_TBL_RECT view;
};

extern NPC_TABLE *gNpcTable;

bool LoadNpcTable(const char *path);
void ReleaseNpcTable();

//NPC Function table
typedef void (*NPCFUNCTION)(NPCHAR*);
extern NPCFUNCTION gpNpcFuncTbl[];
