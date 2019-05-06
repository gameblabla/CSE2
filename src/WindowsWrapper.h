#pragma once

int rep_rand();
void rep_srand(unsigned int seed);

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;

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
