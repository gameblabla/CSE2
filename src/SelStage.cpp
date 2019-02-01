#include "SelStage.h"

#include <string.h>

#include "Draw.h"
#include "Escape.h"
#include "KeyControl.h"
#include "Main.h"
#include "TextScr.h"
#include "Sound.h"
#include "WindowsWrapper.h"

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

	RECT rcView;
	RECT rcCur[2];
	RECT rcTitle1;

	rcView = {0, 0, 320, 240};
	rcCur[0] = {80, 88, 112, 104};
	rcCur[1] = {80, 104, 112, 120};
	rcTitle1 = {80, 64, 144, 72};

	if (gStageSelectTitleY > 46)
		--gStageSelectTitleY;

	PutBitmap3(&rcView, 128, gStageSelectTitleY, &rcTitle1, SURFACE_ID_TEXT_BOX);

	int stage_num;
	for (stage_num = 0; gPermitStage[stage_num].index; ++stage_num);

	++flash;

	if (stage_num)
	{
		int stage_x = (320 - 40 * stage_num) / 2;

		PutBitmap3(&rcView, stage_x + 40 * gSelectedStage, 64, &rcCur[(flash >> 1) % 2], SURFACE_ID_TEXT_BOX);

		for (int i = 0; i < 8 && gPermitStage[i].index; ++i)
		{
			RECT rcStage;
			rcStage.left = 32 * (gPermitStage[i].index % 8);
			rcStage.right = rcStage.left + 32;
			rcStage.top = 16 * (gPermitStage[i].index / 8);
			rcStage.bottom = rcStage.top + 16;

			PutBitmap3(&rcView, stage_x + 40 * i, 64, &rcStage, SURFACE_ID_STAGE_ITEM);
		}
	}
}

int StageSelectLoop(int *p_event)
{
	char old_script_path[260];

	RECT rcView = {0, 0, 320, 240};

	gSelectedStage = 0;
	BackupSurface(10, &grcFull);
	GetTextScriptPath(old_script_path);
	LoadTextScript2("StageSelect.tsc");
	gStageSelectTitleY = 54;
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

		PutBitmap3(&rcView, 0, 0, &rcView, 10);
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