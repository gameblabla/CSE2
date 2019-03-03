#pragma once

#include <stdint.h>

#include "WindowsWrapper.h"

struct BULLET
{
	int flag;
	int code_bullet;
	int bbits;
	int cond;
	int x;
	int y;
	int xm;
	int ym;
	int tgt_x;
	int tgt_y;
	int act_no;
	int act_wait;
	int ani_wait;
	int ani_no;
	uint8_t direct;
	RECT rect;
	int count1;
	int count2;
	int life_count;
	int damage;
	int life;
	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;
	RECT view;
};

struct BULLET_TABLE
{
	int8_t damage;
	int8_t life;
	int life_count;
	int bbits;
	int enemyXL;
	int enemyYL;
	int blockXL;
	int blockYL;
	RECT view;
};

#define BULLET_MAX 0x40
extern BULLET gBul[BULLET_MAX];

void InitBullet();
int CountArmsBullet(int arms_code);
int CountBulletNum(int bullet_code);
void DeleteBullet(int code);
void ClearBullet();
void PutBullet(int fx, int fy);
void SetBullet(int no, int x, int y, int dir);
void ActBullet();
BOOL IsActiveSomeBullet(void);
