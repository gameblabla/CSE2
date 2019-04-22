#include "GenericLoad.h"

#include <stdio.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Ending.h"
#include "PixTone.h"
#include "Sound.h"
#include "Tags.h"

// Decompiled from PTone103.exe
static BOOL LoadPixToneFile(const char *filename, PIXTONEPARAMETER *pixtone_parameters)
{
	BOOL success = FALSE;

	FILE *fp = fopen(filename, "r");

	if (fp)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			float freq;
			fscanf(fp, "use  :%d\n", &pixtone_parameters[i].use);
			fscanf(fp, "size :%d\n", &pixtone_parameters[i].size);
			fscanf(fp, "main_model   :%d\n", &pixtone_parameters[i].oMain.model);
			fscanf(fp, "main_freq    :%f\n", &freq);
			pixtone_parameters[i].oMain.num = freq;
			fscanf(fp, "main_top     :%d\n", &pixtone_parameters[i].oMain.top);
			fscanf(fp, "main_offset  :%d\n", &pixtone_parameters[i].oMain.offset);
			fscanf(fp, "pitch_model  :%d\n", &pixtone_parameters[i].oPitch.model);
			fscanf(fp, "pitch_freq   :%f\n", &freq);
			pixtone_parameters[i].oPitch.num = freq;
			fscanf(fp, "pitch_top    :%d\n", &pixtone_parameters[i].oPitch.top);
			fscanf(fp, "pitch_offset :%d\n", &pixtone_parameters[i].oPitch.offset);
			fscanf(fp, "volume_model :%d\n", &pixtone_parameters[i].oVolume.model);
			fscanf(fp, "volume_freq  :%f\n", &freq);
			pixtone_parameters[i].oVolume.num = freq;
			fscanf(fp, "volume_top   :%d\n", &pixtone_parameters[i].oVolume.top);
			fscanf(fp, "volume_offset:%d\n", &pixtone_parameters[i].oVolume.offset);
			fscanf(fp, "initialY:%d\n", &pixtone_parameters[i].initial);
			fscanf(fp, "ax      :%d\n", &pixtone_parameters[i].pointAx);
			fscanf(fp, "ay      :%d\n", &pixtone_parameters[i].pointAy);
			fscanf(fp, "bx      :%d\n", &pixtone_parameters[i].pointBx);
			fscanf(fp, "by      :%d\n", &pixtone_parameters[i].pointBy);
			fscanf(fp, "cx      :%d\n", &pixtone_parameters[i].pointCx);
			fscanf(fp, "cy      :%d\n\n", &pixtone_parameters[i].pointCy);
		}

		fclose(fp);

		success = TRUE;
	}

	return success;
}

BOOL LoadGenericData()
{
	char str[0x40];
	BOOL bError;
	int pt_size;

	bError = FALSE;
	if (!MakeSurface_File("Resource/BITMAP/PIXEL", SURFACE_ID_PIXEL))
		bError = TRUE;
	if (!MakeSurface_File("MyChar", SURFACE_ID_MY_CHAR))
		bError = TRUE;
	if (!MakeSurface_File("Title", SURFACE_ID_TITLE))
		bError = TRUE;
	if (!MakeSurface_File("ArmsImage", SURFACE_ID_ARMS_IMAGE))
		bError = TRUE;
	if (!MakeSurface_File("Arms", SURFACE_ID_ARMS))
		bError = TRUE;
	if (!MakeSurface_File("ItemImage", SURFACE_ID_ITEM_IMAGE))
		bError = TRUE;
	if (!MakeSurface_File("StageImage", SURFACE_ID_STAGE_ITEM))
		bError = TRUE;
	if (!MakeSurface_File("Npc/NpcSym", SURFACE_ID_NPC_SYM))
		bError = TRUE;
	if (!MakeSurface_File("Npc/NpcRegu", SURFACE_ID_NPC_REGU))
		bError = TRUE;
	if (!MakeSurface_File("TextBox", SURFACE_ID_TEXT_BOX))
		bError = TRUE;
	if (!MakeSurface_File("Caret", SURFACE_ID_CARET))
		bError = TRUE;
	if (!MakeSurface_File("Bullet", SURFACE_ID_BULLET))
		bError = TRUE;
	if (!MakeSurface_File("Face", SURFACE_ID_FACE))
		bError = TRUE;
	if (!MakeSurface_File("Fade", SURFACE_ID_FADE))
		bError = TRUE;
	if (!MakeSurface_File("Resource/BITMAP/CREDIT01", SURFACE_ID_CREDITS_IMAGE))
		bError = TRUE;
	
	if (bError)
	{
		return FALSE;
	}
	else
	{
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_SCREEN_GRAB);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_LEVEL_BACKGROUND);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_MAP);
		MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_CASTS);
		MakeSurface_Generic(256, 256, SURFACE_ID_LEVEL_TILESET);
		MakeSurface_Generic(160, 16, SURFACE_ID_ROOM_NAME);
		MakeSurface_Generic(40, 240, SURFACE_ID_VALUE_VIEW);
		MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_1);
		MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_2);
		MakeSurface_Generic(WINDOW_WIDTH, 16 * MAX_STRIP, SURFACE_ID_CREDIT_CAST);

		pt_size = 0;

		for (unsigned int i = 0; i < SOUND_NO; ++i)
		{
			PIXTONEPARAMETER pixtone_parameters[4];

			char path[PATH_LENGTH];
			sprintf(path, "%s/PixTone/%.3d.pxt", gDataPath, i);

			if (LoadPixToneFile(path, pixtone_parameters))
			{
				int ptp_num = 0;
				while (pixtone_parameters[ptp_num].use && ptp_num < 4)
					++ptp_num;

				pt_size += MakePixToneObject(pixtone_parameters, ptp_num, i);
			}
		}

		sprintf(str, "PixTone = %d byte", pt_size);
		// There must have been some kind of console print function here or something
		return TRUE;
	}
}
