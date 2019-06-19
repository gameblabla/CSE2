#pragma once

#include <stdbool.h>

typedef struct MainLoopMeta
{
	int routine;
	void *user_data;
	struct MainLoopMeta *next;
} MainLoopMeta;

bool MainLoopSelector(void);
void EnterMainLoop(void (*function)(MainLoopMeta *meta), void (*return_handler)(MainLoopMeta *meta, int return_value), void *user_data);
void ExitMainLoop(int return_value);
