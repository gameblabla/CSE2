#include "SelStage.h"

#include <string.h>

#include "WindowsWrapper.h"

#include "Draw.h"
#include "Escape.h"
#include "KeyControl.h"
#include "Main.h"
#include "Sound.h"
#include "TextScr.h"

PERMIT_STAGE gPermitStage[8];

static int gSelectedStage;
static int gStageSelectTitleY;

void ClearPermitStage(void)
{
	memset(gPermitStage, 0, 0x40);
}

bool AddPermitStage(int index, int event)
{
	for (int i = 0; i < 8; ++i)
	{
		if (gPermitStage[i].index == 0 || gPermitStage[i].index == index)
		{
			gPermitStage[i].index = index;
			gPermitStage[i].event = event;
			return true;
		}
	}

	return false;
}

bool SubPermitStage(int index)
{
	int i;
	for (i = 0; i < 8 && gPermitStage[i].index != index; ++i);

#ifdef FIX_BUGS
	if (i != 8)
#else
	if (i != 32)
#endif
	{
		int ia;
		for (ia = i + 1; ia < 8; ++ia)
		{
			gPermitStage[ia - 1].index = gPermitStage[ia].index;
			gPermitStage[ia - 1].event = gPermitStage[ia].event;
		}

		gPermitStage[ia - 1].index = 0;
		gPermitStage[ia - 1].event = 0;

		return true;
	}

	return false;
}

void MoveStageSelectCursor(void)
{
	int stage_num;
	for (stage_num = 0; gPermitStage[stage_num].index != 0; ++stage_num);

	if (stage_num)
	{
		if (gKeyTrg & gKeyLeft)
			--gSelectedStage;

		if (gKeyTrg & gKeyRight)
			++gSelectedStage;

		if (gSelectedStage < 0)
			gSelectedStage = stage_num - 1;

		if (stage_num - 1 < gSelectedStage)
			gSelectedStage = 0;

		if ((gKeyRight | gKeyLeft) & gKeyTrg)
			StartTextScript(gPermitStage[gSelectedStage].index + 1000);

		if ((gKeyRight | gKeyLeft) & gKeyTrg)
			PlaySoundObject(1, 1);
	}
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

	if (gStageSelectTitleY > (WINDOW_HEIGHT / 2) - 74)
		--gStageSelectTitleY;

	PutBitmap3(&rcView, (WINDOW_WIDTH / 2) - 32, gStageSelectTitleY, &rcTitle1, SURFACE_ID_TEXT_BOX);

	int stage_num;
	for (stage_num = 0; gPermitStage[stage_num].index; ++stage_num);

	++flash;

	if (stage_num)
	{
		int stage_x = (WINDOW_WIDTH - 40 * stage_num) / 2;

		PutBitmap3(&rcView, stage_x + 40 * gSelectedStage, (WINDOW_HEIGHT / 2) - 56, &rcCur[(flash >> 1) % 2], SURFACE_ID_TEXT_BOX);

		for (int i = 0; i < 8 && gPermitStage[i].index; ++i)
		{
			RECT rcStage;
			rcStage.left = 32 * (gPermitStage[i].index % 8);
			rcStage.right = rcStage.left + 32;
			rcStage.top = 16 * (gPermitStage[i].index / 8);
			rcStage.bottom = rcStage.top + 16;

			PutBitmap3(&rcView, stage_x + 40 * i, (WINDOW_HEIGHT / 2) - 56, &rcStage, SURFACE_ID_STAGE_ITEM);
		}
	}
}

int StageSelectLoop(int *p_event)
{
	char old_script_path[260];

	RECT rcView = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

	gSelectedStage = 0;
	BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcFull);
	GetTextScriptPath(old_script_path);
	LoadTextScript2("StageSelect.tsc");
	gStageSelectTitleY = (WINDOW_HEIGHT / 2) - 66;
	StartTextScript(gPermitStage[gSelectedStage].index + 1000);

	do
	{
		GetTrg();

		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 2;
		}

		MoveStageSelectCursor();

		int tscRet = TextScriptProc();
		if (tscRet == 0)
			return 0;
		if (tscRet == 2)
			return 2;

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
			LoadTextScript_Stage(old_script_path);
			*p_event = gPermitStage[gSelectedStage].event;
			return 1;
		}

		if (gKeyTrg & gKeyCancel)
		{
			StopTextScript();
			LoadTextScript_Stage(old_script_path);
			*p_event = 0;
			return 1;
		}

		PutFramePerSecound();
	}
	while (Flip_SystemTask());

	return 0;
}