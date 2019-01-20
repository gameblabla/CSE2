#pragma once
#include <stdint.h>
#include "WindowsWrapper.h"

struct NPCHAR
{
	uint8_t cond;
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
	int surf;
	int hit_voice;
	int destroy_voice;
	int life;
	int exp;
	int size;
	int direct;
	uint16_t bits;
	RECT rect;
	int ani_wait;
	int ani_no;
	int count1;
	int count2;
	int act_no;
	int act_wait;
	RECT hit;
	RECT view;
	uint8_t shock;
	int damage_view;
	int damage;
	NPCHAR *pNpc;
};

bool LoadEvent(char *path_event);
