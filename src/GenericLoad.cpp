#include "GenericLoad.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Draw.h"
#include "Ending.h"
#ifdef EXTRA_SOUND_FORMATS
#include "ExtraSoundFormats.h"
#endif
#include "Main.h"
#include "PixTone.h"
#include "Sound.h"

enum
{
	SOUND_TYPE_PIXTONE,
	SOUND_TYPE_OTHER
};

static const struct
{
	int slot;
	const char *path;
	int type;
} ptp_table[] = {
	// TODO - Name the files
	{1, "PixTone/001.pxt", SOUND_TYPE_PIXTONE},
	{2, "PixTone/002.pxt", SOUND_TYPE_PIXTONE},
	{3, "PixTone/003.pxt", SOUND_TYPE_PIXTONE},
	{4, "PixTone/004.pxt", SOUND_TYPE_PIXTONE},
	{5, "PixTone/005.pxt", SOUND_TYPE_PIXTONE},
	{6, "PixTone/006.pxt", SOUND_TYPE_PIXTONE},
	{7, "PixTone/007.pxt", SOUND_TYPE_PIXTONE},
	{11, "PixTone/011.pxt", SOUND_TYPE_PIXTONE},
	{12, "PixTone/012.pxt", SOUND_TYPE_PIXTONE},
	{14, "PixTone/014.pxt", SOUND_TYPE_PIXTONE},
	{15, "PixTone/015.pxt", SOUND_TYPE_PIXTONE},
	{16, "PixTone/016.pxt", SOUND_TYPE_PIXTONE},
	{17, "PixTone/017.pxt", SOUND_TYPE_PIXTONE},
	{18, "PixTone/018.pxt", SOUND_TYPE_PIXTONE},
	{20, "PixTone/020.pxt", SOUND_TYPE_PIXTONE},
	{21, "PixTone/021.pxt", SOUND_TYPE_PIXTONE},
	{22, "PixTone/022.pxt", SOUND_TYPE_PIXTONE},
	{23, "PixTone/023.pxt", SOUND_TYPE_PIXTONE},
	{24, "PixTone/024.pxt", SOUND_TYPE_PIXTONE},
	{25, "PixTone/025.pxt", SOUND_TYPE_PIXTONE},
	{26, "PixTone/026.pxt", SOUND_TYPE_PIXTONE},
	{27, "PixTone/027.pxt", SOUND_TYPE_PIXTONE},
	{28, "PixTone/028.pxt", SOUND_TYPE_PIXTONE},
	{29, "PixTone/029.pxt", SOUND_TYPE_PIXTONE},
	{30, "PixTone/030.pxt", SOUND_TYPE_PIXTONE},
	{31, "PixTone/031.pxt", SOUND_TYPE_PIXTONE},
	{32, "PixTone/032.pxt", SOUND_TYPE_PIXTONE},
	{33, "PixTone/033.pxt", SOUND_TYPE_PIXTONE},
	{34, "PixTone/034.pxt", SOUND_TYPE_PIXTONE},
	{35, "PixTone/035.pxt", SOUND_TYPE_PIXTONE},
	{37, "PixTone/037.pxt", SOUND_TYPE_PIXTONE},
	{38, "PixTone/038.pxt", SOUND_TYPE_PIXTONE},
	{39, "PixTone/039.pxt", SOUND_TYPE_PIXTONE},
	{40, "PixTone/040.pxt", SOUND_TYPE_PIXTONE},
	{41, "PixTone/041.pxt", SOUND_TYPE_PIXTONE},
	{42, "PixTone/042.pxt", SOUND_TYPE_PIXTONE},
	{43, "PixTone/043.pxt", SOUND_TYPE_PIXTONE},
	{44, "PixTone/044.pxt", SOUND_TYPE_PIXTONE},
	{45, "PixTone/045.pxt", SOUND_TYPE_PIXTONE},
	{46, "PixTone/046.pxt", SOUND_TYPE_PIXTONE},
	{47, "PixTone/047.pxt", SOUND_TYPE_PIXTONE},
	{48, "PixTone/048.pxt", SOUND_TYPE_PIXTONE},
	{49, "PixTone/049.pxt", SOUND_TYPE_PIXTONE},
	{50, "PixTone/050.pxt", SOUND_TYPE_PIXTONE},
	{51, "PixTone/051.pxt", SOUND_TYPE_PIXTONE},
	{52, "PixTone/052.pxt", SOUND_TYPE_PIXTONE},
	{53, "PixTone/053.pxt", SOUND_TYPE_PIXTONE},
	{54, "PixTone/054.pxt", SOUND_TYPE_PIXTONE},
	{55, "PixTone/055.pxt", SOUND_TYPE_PIXTONE},
	{56, "PixTone/056.pxt", SOUND_TYPE_PIXTONE},
	{57, "PixTone/057.pxt", SOUND_TYPE_PIXTONE},
	{58, "PixTone/058.pxt", SOUND_TYPE_PIXTONE},
	{59, "PixTone/059.pxt", SOUND_TYPE_PIXTONE},
	{60, "PixTone/060.pxt", SOUND_TYPE_PIXTONE},
	{61, "PixTone/061.pxt", SOUND_TYPE_PIXTONE},
	{62, "PixTone/062.pxt", SOUND_TYPE_PIXTONE},
	{63, "PixTone/063.pxt", SOUND_TYPE_PIXTONE},
	{64, "PixTone/064.pxt", SOUND_TYPE_PIXTONE},
	{65, "PixTone/065.pxt", SOUND_TYPE_PIXTONE},
	{70, "PixTone/070.pxt", SOUND_TYPE_PIXTONE},
	{71, "PixTone/071.pxt", SOUND_TYPE_PIXTONE},
	{72, "PixTone/072.pxt", SOUND_TYPE_PIXTONE},
	{100, "PixTone/100.pxt", SOUND_TYPE_PIXTONE},
	{101, "PixTone/101.pxt", SOUND_TYPE_PIXTONE},
	{102, "PixTone/102.pxt", SOUND_TYPE_PIXTONE},
	{103, "PixTone/103.pxt", SOUND_TYPE_PIXTONE},
	{104, "PixTone/104.pxt", SOUND_TYPE_PIXTONE},
	{105, "PixTone/105.pxt", SOUND_TYPE_PIXTONE},
	{106, "PixTone/106.pxt", SOUND_TYPE_PIXTONE},
	{107, "PixTone/107.pxt", SOUND_TYPE_PIXTONE},
	{108, "PixTone/108.pxt", SOUND_TYPE_PIXTONE},
	{109, "PixTone/109.pxt", SOUND_TYPE_PIXTONE},
	{110, "PixTone/110.pxt", SOUND_TYPE_PIXTONE},
	{111, "PixTone/111.pxt", SOUND_TYPE_PIXTONE},
	{112, "PixTone/112.pxt", SOUND_TYPE_PIXTONE},
	{113, "PixTone/113.pxt", SOUND_TYPE_PIXTONE},
	{114, "PixTone/114.pxt", SOUND_TYPE_PIXTONE},
	{115, "PixTone/115.pxt", SOUND_TYPE_PIXTONE},
	{116, "PixTone/116.pxt", SOUND_TYPE_PIXTONE},
	{117, "PixTone/117.pxt", SOUND_TYPE_PIXTONE},
	{150, "PixTone/150.pxt", SOUND_TYPE_PIXTONE},
	{151, "PixTone/151.pxt", SOUND_TYPE_PIXTONE},
	{152, "PixTone/152.pxt", SOUND_TYPE_PIXTONE},
	{153, "PixTone/153.pxt", SOUND_TYPE_PIXTONE},
	{154, "PixTone/154.pxt", SOUND_TYPE_PIXTONE},
	{155, "PixTone/155.pxt", SOUND_TYPE_PIXTONE}
};

// Original decompiled from `PTone103.exe` - has since been modified
static BOOL LoadPixToneFile(const char *filename, PIXTONEPARAMETER *pixtone_parameters)
{
	BOOL success = FALSE;

	FILE *fp = fopen(filename, "r");

	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		const size_t file_size = ftell(fp);
		rewind(fp);

		char *file_buffer = (char*)malloc(file_size);
		fread(file_buffer, 1, file_size, fp);

		fclose(fp);

		char *p = file_buffer;

		for (unsigned int i = 0; i < 4; ++i)
		{
			float freq;
			int increment;
			sscanf(p, "use  :%d\n%n", &pixtone_parameters[i].use, &increment);
			p += increment;
			sscanf(p, "size :%d\n%n", &pixtone_parameters[i].size, &increment);
			p += increment;
			sscanf(p, "main_model   :%d\n%n", &pixtone_parameters[i].oMain.model, &increment);
			p += increment;
			sscanf(p, "main_freq    :%f\n%n", &freq, &increment);
			p += increment;
			pixtone_parameters[i].oMain.num = freq;
			sscanf(p, "main_top     :%d\n%n", &pixtone_parameters[i].oMain.top, &increment);
			p += increment;
			sscanf(p, "main_offset  :%d\n%n", &pixtone_parameters[i].oMain.offset, &increment);
			p += increment;
			sscanf(p, "pitch_model  :%d\n%n", &pixtone_parameters[i].oPitch.model, &increment);
			p += increment;
			sscanf(p, "pitch_freq   :%f\n%n", &freq, &increment);
			p += increment;
			pixtone_parameters[i].oPitch.num = freq;
			sscanf(p, "pitch_top    :%d\n%n", &pixtone_parameters[i].oPitch.top, &increment);
			p += increment;
			sscanf(p, "pitch_offset :%d\n%n", &pixtone_parameters[i].oPitch.offset, &increment);
			p += increment;
			sscanf(p, "volume_model :%d\n%n", &pixtone_parameters[i].oVolume.model, &increment);
			p += increment;
			sscanf(p, "volume_freq  :%f\n%n", &freq, &increment);
			p += increment;
			pixtone_parameters[i].oVolume.num = freq;
			sscanf(p, "volume_top   :%d\n%n", &pixtone_parameters[i].oVolume.top, &increment);
			p += increment;
			sscanf(p, "volume_offset:%d\n%n", &pixtone_parameters[i].oVolume.offset, &increment);
			p += increment;
			sscanf(p, "initialY:%d\n%n", &pixtone_parameters[i].initial, &increment);
			p += increment;
			sscanf(p, "ax      :%d\n%n", &pixtone_parameters[i].pointAx, &increment);
			p += increment;
			sscanf(p, "ay      :%d\n%n", &pixtone_parameters[i].pointAy, &increment);
			p += increment;
			sscanf(p, "bx      :%d\n%n", &pixtone_parameters[i].pointBx, &increment);
			p += increment;
			sscanf(p, "by      :%d\n%n", &pixtone_parameters[i].pointBy, &increment);
			p += increment;
			sscanf(p, "cx      :%d\n%n", &pixtone_parameters[i].pointCx, &increment);
			p += increment;
			sscanf(p, "cy      :%d\n\n%n", &pixtone_parameters[i].pointCy, &increment);
			p += increment;
		}

		free(file_buffer);

		success = TRUE;
	}

	return success;
}

BOOL LoadGenericData(void)
{
	int pt_size;
	BOOL bError;

	bError = FALSE;
	if (!MakeSurface_File("Resource/BITMAP/pixel", SURFACE_ID_PIXEL))
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
	if (!MakeSurface_File("Resource/BITMAP/Credit01", SURFACE_ID_CREDITS_IMAGE))
		bError = TRUE;

	if (bError)
		return FALSE;

	MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_SCREEN_GRAB, TRUE);
	MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_BACKGROUND, FALSE);
	MakeSurface_Generic(WINDOW_WIDTH, WINDOW_HEIGHT, SURFACE_ID_MAP, TRUE);
	MakeSurface_Generic(320, 240, SURFACE_ID_CASTS, FALSE);
	MakeSurface_Generic(256, 256, SURFACE_ID_LEVEL_TILESET, FALSE);
	MakeSurface_Generic(160, 16, SURFACE_ID_ROOM_NAME, FALSE);
	MakeSurface_Generic(40, 240, SURFACE_ID_VALUE_VIEW, FALSE);
	MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_1, FALSE);
	MakeSurface_Generic(320, 240, SURFACE_ID_LEVEL_SPRITESET_2, FALSE);
	MakeSurface_Generic(320, 16 * (MAX_STRIP - 1), SURFACE_ID_CREDIT_CAST, FALSE);

	pt_size = 0;

	for (unsigned int i = 0; i < sizeof(ptp_table) / sizeof(ptp_table[0]); ++i)
	{
		std::string path = gDataPath + '/' + ptp_table[i].path;

		switch (ptp_table[i].type)
		{
			case SOUND_TYPE_PIXTONE:
				PIXTONEPARAMETER pixtone_parameters[4];

				if (LoadPixToneFile(path.c_str(), pixtone_parameters))
				{
					int ptp_num = 0;
					while (pixtone_parameters[ptp_num].use && ptp_num < 4)
						++ptp_num;

					pt_size += MakePixToneObject(pixtone_parameters, ptp_num, ptp_table[i].slot);
				}

				break;

#ifdef EXTRA_SOUND_FORMATS
			case SOUND_TYPE_OTHER:
				ExtraSound_LoadSFX(path.c_str(), ptp_table[i].slot);
				break;
#endif
		}
	}

	// Commented-out, since ints *technically* have an undefined length
	// char str[0x40];
	// sprintf(str, "PixTone = %d byte", pt_size);
	// There must have been some kind of console print function here or something
	return TRUE;
}
