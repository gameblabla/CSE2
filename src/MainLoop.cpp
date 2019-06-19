#include "MainLoop.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct MainLoop
{
	void (*function)(MainLoopMeta *meta);
	void (*return_handler)(MainLoopMeta *meta, int return_value);
	int return_value;
	MainLoopMeta meta;
	struct MainLoop *next;
} MainLoop;

static MainLoop *mainloop_list_head;

void EnterMainLoop(void (*function)(MainLoopMeta *meta), void (*return_handler)(MainLoopMeta *meta, int return_value), void *user_data)
{
	MainLoop *mainloop = (MainLoop*)malloc(sizeof(MainLoop));
	mainloop->function = function;
	mainloop->return_handler = return_handler;
	mainloop->return_value = 0;
	mainloop->meta.routine = 0;
	mainloop->meta.user_data = user_data;
	mainloop->meta.next = mainloop_list_head ? &mainloop_list_head->meta : NULL;
	mainloop->next = mainloop_list_head;
	mainloop_list_head = mainloop;
}

void ExitMainLoop(int return_value)
{
	mainloop_list_head->meta.routine = -1;
	mainloop_list_head->return_value = return_value;
}

bool MainLoopSelector(void)
{
	MainLoop *old_mainloop_list_head;

	do
	{
		old_mainloop_list_head = mainloop_list_head;

		MainLoop *mainloop = mainloop_list_head;

		if (mainloop->meta.routine != -1)
			mainloop->function(&mainloop->meta);

		if (mainloop->meta.routine == -1)
		{
			mainloop->function(&mainloop->meta);

			mainloop_list_head = mainloop_list_head->next;

			if (mainloop_list_head == NULL)
			{
				free(mainloop);
				return false;
			}

			if (mainloop->return_handler)
				mainloop->return_handler(&mainloop->meta, mainloop->return_value);

			free(mainloop);
		}
	} while (old_mainloop_list_head != mainloop_list_head);

	return true;
}
