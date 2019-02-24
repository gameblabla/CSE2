#pragma once

int rep_rand();
void rep_srand(unsigned int seed);

#ifndef BOOL
//typedef int BOOL;
#define BOOL int
#endif
#ifndef BOOLEAN
//typedef unsigned char BOOLEAN;
#define BOOLEAN unsigned char
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

struct RECT
{
	union
	{
		int left;
		int front;
	};
	int top;
	union
	{
		int right;
		int back;
	};
	int bottom;
};

bool SystemTask();
