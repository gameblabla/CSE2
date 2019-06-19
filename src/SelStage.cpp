#include "SelStage.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Escape.h"
#include "KeyControl.h"
#include "Main.h"
#include "MainLoop.h"
#include "Sound.h"
#include "TextScr.h"

PERMIT_STAGE gPermitStage[8];

static int gSelectedStage;
static int gStageSelectTitleY;

void ClearPermitStage(void)
{
	memset(gPermitStage, 0, 0x40);
}

BOOL AddPermitStage(int index, int event)
{
	int i = 0;
	while (i < 8)
	{
		if (gPermitStage[i].index == index)
			break;

		if (gPermitStage[i].index == 0)
			break;

		++i;
	}

	if (i == 8)
		return FALSE;

	gPermitStage[i].index = index;
	gPermitStage[i].event = event;

	return TRUE;
}

BOOL SubPermitStage(int index)
{
	int i;
	for (i = 0; i < 8; ++i)
		if (gPermitStage[i].index == index)
			break;

#ifdef FIX_BUGS
	if (i == 8)
#else
	if (i == 32)
#endif
		return FALSE;

	for (i += 1; i < 8; ++i)
	{
		gPermitStage[i - 1] = gPermitStage[i];
	}

	gPermitStage[i - 1].index = 0;
	gPermitStage[i - 1].event = 0;

	return TRUE;
}

void MoveStageSelectCursor(void)
{
	int stage_num = 0;
	while (gPermitStage[stage_num].index != 0)
		++stage_num;

	int stage_x = (WINDOW_WIDTH - 40 * stage_num) / 2;	// Unused

	if (stage_num == 0)
		return;

	if (gKeyTrg & gKeyLeft)
		--gSelectedStage;

	if (gKeyTrg & gKeyRight)
		++gSelectedStage;

	if (gSelectedStage < 0)
		gSelectedStage = stage_num - 1;

	if (gSelectedStage > stage_num - 1)
		gSelectedStage = 0;

	if ((gKeyLeft | gKeyRight) & gKeyTrg)
		StartTextScript(gPermitStage[gSelectedStage].index + 1000);

	if ((gKeyLeft | gKeyRight) & gKeyTrg)
		PlaySoundObject(1, 1);
}

void PutStageSelectObject(void)
{
	static unsigned int flash;

	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	RECT rcCur[2] = {
		{80, 88, 112, 104},
		{80, 104, 112, 120},
	};

	RECT rcTitle1 = {80, 64, 144, 72};

	int i;
	int stage_num;
	int stage_x;
	RECT rcStage;

	if (gStageSelectTitleY > (WINDOW_HEIGHT / 2) - 74)
		--gStageSelectTitleY;

	PutBitmap3(&rcView, (WINDOW_WIDTH / 2) - 32, gStageSelectTitleY, &rcTitle1, SURFACE_ID_TEXT_BOX);

	stage_num = 0;
	while (gPermitStage[stage_num].index)
		++stage_num;

	++flash;

	if (stage_num)
	{
		stage_x = (WINDOW_WIDTH - 40 * stage_num) / 2;

		PutBitmap3(&rcView, stage_x + 40 * gSelectedStage, (WINDOW_HEIGHT / 2) - 56, &rcCur[(flash >> 1) % 2], SURFACE_ID_TEXT_BOX);

		for (i = 0; i < 8; ++i)
		{
			if (gPermitStage[i].index == 0)
				break;

			rcStage.left = 32 * (gPermitStage[i].index % 8);
			rcStage.right = rcStage.left + 32;
			rcStage.top = 16 * (gPermitStage[i].index / 8);
			rcStage.bottom = rcStage.top + 16;

			PutBitmap3(&rcView, stage_x + 40 * i, (WINDOW_HEIGHT / 2) - 56, &rcStage, SURFACE_ID_STAGE_ITEM);
		}
	}
}

static void StageSelectLoopReturn(MainLoopMeta *meta, int return_value)
{
	(void)meta;

	switch (return_value)
	{
		case 0:
			ExitMainLoop(0);
			return;

		case 2:
			ExitMainLoop(2);
			return;
	}
}

void StageSelectLoop(MainLoopMeta *meta)
{
	StageSelectLoop_Data *data = (StageSelectLoop_Data*)meta->user_data;

	static char old_script_path[260];

	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	switch (meta->routine)
	{
		case 0:
			gSelectedStage = 0;
			BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcFull);
			GetTextScriptPath(old_script_path);
			LoadTextScript2("StageSelect.tsc");
			gStageSelectTitleY = (WINDOW_HEIGHT / 2) - 66;
			StartTextScript(gPermitStage[gSelectedStage].index + 1000);

			++meta->routine;
			// Fallthrough
		case 1:
			GetTrg();

			if (gKey & KEY_ESCAPE)
			{
				EnterMainLoop(Call_Escape, StageSelectLoopReturn, &ghWnd);
				return;
			}

			MoveStageSelectCursor();

			switch (TextScriptProc(StageSelectLoopReturn))
			{
				case 0:
				{
					ExitMainLoop(0);
					return;
				}
				case 2:
				{
					ExitMainLoop(2);
					return;
				}
			}

	#ifdef FIX_BUGS
			PutBitmap4(&rcView, 0, 0, &rcView, SURFACE_ID_SCREEN_GRAB);
	#else
			// The original accidentally drew the screencap with transparency enabled
			PutBitmap3(&rcView, 0, 0, &rcView, SURFACE_ID_SCREEN_GRAB);
	#endif
			PutStageSelectObject();
			PutTextScript();

			if (gKeyTrg & gKeyOk)
			{
				StopTextScript();
			}
			else
			{
				if (gKeyTrg & gKeyCancel)
				{
					StopTextScript();
					LoadTextScript_Stage(old_script_path);
					data->event = 0;
					ExitMainLoop(1);
					return;
				}

				PutFramePerSecound();

				if (!Flip_SystemTask(ghWnd))
				{
					ExitMainLoop(0);
					return;
				}

				break;
			}

			LoadTextScript_Stage(old_script_path);
			data->event = gPermitStage[gSelectedStage].event;
			ExitMainLoop(1);
			return;
	}
}
