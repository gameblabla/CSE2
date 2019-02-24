#include "Game.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "ArmsItem.h"
#include "Back.h"
#include "Boss.h"
#include "BossLife.h"
#include "BulHit.h"
#include "Bullet.h"
#include "Caret.h"
#include "Draw.h"
#include "Ending.h"
#include "Escape.h"
#include "Fade.h"
#include "Flags.h"
#include "Flash.h"
#include "Frame.h"
#include "Generic.h"
#include "GenericLoad.h"
#include "KeyControl.h"
#include "Main.h"
#include "Map.h"
#include "MapName.h"
#include "MiniMap.h"
#include "MyChar.h"
#include "MycHit.h"
#include "MycParam.h"
#include "NpChar.h"
#include "NpcHit.h"
#include "NpcTbl.h"
#include "Organya.h"
#include "Profile.h"
#include "SelStage.h"
#include "Shoot.h"
#include "Sound.h"
#include "Stage.h"
#include "Star.h"
#include "Tags.h"
#include "TextScr.h"
#include "ValueView.h"

int g_GameFlags;
int gCounter;

bool bContinue;

int Random(int min, int max)
{
	return min + rep_rand() % (max - min + 1);
}

void PutNumber4(int x, int y, int value, bool bZero)
{
	//Define rects
	RECT rcClient = grcFull;
	
	RECT rect[10] = {
		{0, 56, 8, 64},
		{8, 56, 16, 64},
		{16, 56, 24, 64},
		{24, 56, 32, 64},
		{32, 56, 40, 64},
		{40, 56, 48, 64},
		{48, 56, 56, 64},
		{56, 56, 64, 64},
		{64, 56, 72, 64},
		{72, 56, 80, 64},
	};
	
	//Digits
	int tbl[4] = {1000, 100, 10, 1};
	
	//Limit value
	if ( value > 9999 )
		value = 9999;
	
	//Go through number and draw digits
	int offset = 0;
	int sw = 0;
	while (offset < 4)
	{
		//Get the digit that this is
		int a = 0;
		
		while (tbl[offset] <= value)
		{
			value -= tbl[offset];
			++a;
			++sw;
		}
		
		//Draw digit
		if ((bZero && offset == 2) || sw != 0 || offset == 3 )
			PutBitmap3(&rcClient, x + 8 * offset, y, &rect[a], SURFACE_ID_TEXT_BOX);
		
		//Go to next digit
		++offset;
	}
}

int ModeOpening()
{
	InitNpChar();
	InitCaret();
	InitStar();
	InitFade();
	InitFlash();
	InitBossLife();
	ChangeMusic(0);
	if (!TransferStage(72, 100, 3, 3))
		return 0;
	SetFrameTargetMyChar(16);
	SetFadeMask();
	
	//Reset cliprect and flags
	grcGame.left = 0;
	//Non-vanilla: these three lines are widescreen-related(?)
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;

	g_GameFlags = 3;
	
	CutNoise();
	
	unsigned int wait = 0;
	while (wait < 500)
	{
		//Increase timer
		++wait;
		
		//Get pressed keys
		GetTrg();
		
		//Escape menu
		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 1;
		}
		
		//Skip intro if OK is pressed
		if (gKey & gKeyOk)
			break;
		
		//Update everything
		ActNpChar();
		ActBossChar();
		ActBack();
		ResetMyCharFlag();
		HitMyCharMap();
		HitMyCharNpChar();
		HitMyCharBoss();
		HitNpCharMap();
		HitBossMap();
		HitBossBullet();
		ActCaret();
		MoveFrame3();
		ProcFade();
		
		//Draw everything
		CortBox(&grcFull, 0x000000);
		
		int frame_x, frame_y;
		GetFramePosition(&frame_x, &frame_y);
		PutBack(frame_x, frame_y);
		PutStage_Back(frame_x, frame_y);
		PutBossChar(frame_x, frame_y);
		PutNpChar(frame_x, frame_y);
		PutMapDataVector(frame_x, frame_y);
		PutStage_Front(frame_x, frame_y);
		PutFront(frame_x, frame_y);
		PutCaret(frame_x, frame_y);
		PutFade();
		
		//Update Text Script
		int tscRet = TextScriptProc();
		if (tscRet == 0)
			return 0;
		if (tscRet == 2)
			return 1;
	
		PutMapName(false);
		PutTextScript();
		PutFramePerSecound();
		
		if (!Flip_SystemTask())
			return 0;
		
		CortBox(&grcFull, gCounter * 0x20);
		++gCounter;
	}
	
	for (wait = 0; wait < 500; wait += 20)
	{
		CortBox(&grcGame, 0x000000);
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}
	return 2;
}

int ModeTitle()
{
	//Set rects
	RECT rcTitle = {0, 0, 144, 32};
	RECT rcPixel = {0, 0, 160, 16};
	
	RECT rcNew = {144, 0, 192, 16};
	RECT rcContinue = {144, 16, 192, 32};
	
	RECT rcVersion = {152, 80, 208, 88};
	RECT rcPeriod = {152, 88, 208, 96};
	
	//Character rects
	RECT rcMyChar[4] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{0, 16, 16, 32},
		{32, 16, 48, 32},
	};

	RECT rcCurly[4] = {
		{0, 112, 16, 128},
		{16, 112, 32, 128},
		{0, 112, 16, 128},
		{32, 112, 48, 128},
	};
	
	RECT rcToroko[4] = {
		{64, 80, 80, 96},
		{80, 80, 96, 96},
		{64, 80, 80, 96},
		{96, 80, 112, 96},
	};
	
	RECT rcKing[4] = {
		{224, 48, 240, 64},
		{288, 48, 304, 64},
		{224, 48, 240, 64},
		{304, 48, 320, 64},
	};
	
	RECT rcSu[4] = {
		{0, 16, 16, 32},
		{32, 16, 48, 32},
		{0, 16, 16, 32},
		{48, 16, 64, 32},
	};
	
	//Reset everything
	InitCaret();
	InitStar();
	CutNoise();
	
	//Create variables
	int anime = 0;
	int char_type = 0;
	int time_counter = 0;
	unsigned long back_color = GetCortBoxColor(RGB(0x20, 0x20, 0x20));
	
	//Set state
	bContinue = IsProfile();
	
	//Set character
	time_counter = LoadTimeCounter();
	
	if (time_counter && time_counter < 18000)
		char_type = 1;
	if (time_counter && time_counter < 15000)
		char_type = 2;
	if (time_counter && time_counter < 12000)
		char_type = 3;
	if (time_counter && time_counter < 9000)
		char_type = 4;
	
	//Set music to character's specific music
	switch (char_type)
	{
		case 1:
			ChangeMusic(mus_RunningHell);
			break;
		case 2:
			ChangeMusic(mus_TorokosTheme);
			break;
		case 3:
			ChangeMusic(mus_White);
			break;
		case 4:
			ChangeMusic(mus_Safety);
			break;
		default:
			ChangeMusic(mus_CaveStory);
			break;
	}
	
	//Reset cliprect, flags, and give the player the nikumaru counter
	grcGame.left = 0;
	//Non-vanilla: these three lines are widescreen-related(?)
	grcGame.top = 0;
	grcGame.right = WINDOW_WIDTH;
	grcGame.bottom = WINDOW_HEIGHT;

	g_GameFlags = 0;
	gMC.equip |= 0x100;
	
	//Start loop
	unsigned int wait = 0;
	
	while (true)
	{
		//Don't accept selection for 10 frames
		if (wait < 10)
			++wait;
		
		//Get pressed keys
		GetTrg();
		
		//Quit when OK is pressed
		if (wait >= 10)
		{
			if (gKeyTrg & gKeyOk)
				break;
		}
		
		if (gKey & KEY_ESCAPE)
		{
			int escRet = Call_Escape();
			if (escRet == 0)
				return 0;
			if (escRet == 2)
				return 1;
		}
		
		//Move cursor
		if ((gKeyDown | gKeyUp) & gKeyTrg)
		{
			PlaySoundObject(1, 1);
			bContinue = !bContinue;
		}
		
		//Update carets
		ActCaret();
		
		//Animate character cursor
		if ( ++anime >= 40 )
			anime = 0;
		
		//Draw title
		CortBox(&grcGame, back_color);
		
		//Draw version
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 120) / 2, WINDOW_HEIGHT - 24, &rcVersion, SURFACE_ID_TEXT_BOX);
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 8) / 2, WINDOW_HEIGHT - 24, &rcPeriod, SURFACE_ID_TEXT_BOX);
		
		int v1, v2, v3, v4;
		GetCompileVersion(&v1, &v2, &v3, &v4);
		PutNumber4((WINDOW_WIDTH - 40) / 2, WINDOW_HEIGHT - 24, v1, 0);
		PutNumber4((WINDOW_WIDTH - 8) / 2, WINDOW_HEIGHT - 24, v2, 0);
		PutNumber4((WINDOW_WIDTH + 24) / 2, WINDOW_HEIGHT - 24, v3, 0);
		PutNumber4((WINDOW_WIDTH + 56) / 2, WINDOW_HEIGHT - 24, v4, 0);
		
		//Draw main title
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 144) / 2, 40, &rcTitle, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 48) / 2, (WINDOW_HEIGHT + 16) / 2, &rcNew, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 48) / 2, (WINDOW_HEIGHT + 56) / 2, &rcContinue, SURFACE_ID_TITLE);
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 160) / 2, WINDOW_HEIGHT - 48, &rcPixel, SURFACE_ID_PIXEL);
		
		//Draw character cursor
		RECT char_rc;
		Surface_Ids char_surf;
		
		switch ( char_type )
		{
			case 0:
				char_rc = rcMyChar[anime / 10 % 4];
				char_surf = SURFACE_ID_MY_CHAR;
				break;
			case 1:
				char_rc = rcCurly[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 2:
				char_rc = rcToroko[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 3:
				char_rc = rcKing[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
			case 4:
				char_rc = rcSu[anime / 10 % 4];
				char_surf = SURFACE_ID_NPC_REGU;
				break;
		}
		
		int char_y;
		if (bContinue == 1)
			char_y = (WINDOW_HEIGHT + 54) / 2;
		else
			char_y = (WINDOW_HEIGHT + 14) / 2;
		
		PutBitmap3(&grcGame, (WINDOW_WIDTH - 88) / 2, char_y, &char_rc, char_surf);
		
		//Draw carets
		PutCaret(0, 0);
		
		if (time_counter)
			PutTimeCounter(16, 8);
	
		PutFramePerSecound();
		
		if (!Flip_SystemTask())
			return 0;
	}
	
	PlaySoundObject(18, 1);
	ChangeMusic(0);
	
	//Black screen when option is selected
	for (wait = 0; wait < 1000; wait += 20)
	{
		CortBox(&grcGame, 0);
		PutFramePerSecound();
		if (!Flip_SystemTask())
			return 0;
	}
	
	return 3;
}

int ModeAction()
{
	int frame_x = 0;
	int frame_y = 0;

	unsigned long color = GetCortBoxColor(RGB(0, 0, 0x20));

	bool swPlay = true;
	
	//Reset stuff
	gCounter = 0;
	grcGame.left = 0;
	g_GameFlags = 3;
	
	//Initialize everything
	InitMyChar();
	InitNpChar();
	InitBullet();
	InitCaret();
	InitStar();
	InitFade();
	InitFlash();
	ClearArmsData();
	ClearItemData();
	ClearPermitStage();
	StartMapping();
	InitFlags();
	InitBossLife();
	
	if ((bContinue && LoadProfile(NULL)) || InitializeGame())
	{
		while (true)
		{
			//Get pressed keys
			GetTrg();
			
			//Escape menu
			if (gKey & KEY_ESCAPE)
			{
				int escRet = Call_Escape();
				if (escRet == 0)
					return 0;
				if (escRet == 2)
					return 1;
			}
			
			if (swPlay & 1 && g_GameFlags & 1)
			{
				if (g_GameFlags & 2)
					ActMyChar(true);
				else
					ActMyChar(false);
				
				ActStar();
				ActNpChar();
				ActBossChar();
				ActValueView();
				ActBack();
				ResetMyCharFlag();
				HitMyCharMap();
				HitMyCharNpChar();
				HitMyCharBoss();
				HitNpCharMap();
				HitBossMap();
				HitBulletMap();
				HitNpCharBullet();
				HitBossBullet();
				if (g_GameFlags & 2)
					ShootBullet();
				ActBullet();
				ActCaret();
				MoveFrame3();
				ActFlash(frame_x, frame_y);
				
				if (g_GameFlags & 2)
					AnimationMyChar(true);
				else
					AnimationMyChar(false);
			}
			
			if (g_GameFlags & 8)
			{
				ActionCredit();
				ActionIllust();
				ActionStripper();
			}
			
			ProcFade();
			CortBox(&grcFull, color);
			GetFramePosition(&frame_x, &frame_y);
			PutBack(frame_x, frame_y);
			PutStage_Back(frame_x, frame_y);
			PutBossChar(frame_x, frame_y);
			PutNpChar(frame_x, frame_y);
			PutBullet(frame_x, frame_y);
			PutMyChar(frame_x, frame_y);
			PutStar(frame_x, frame_y);
			PutMapDataVector(frame_x, frame_y);
			PutStage_Front(frame_x, frame_y);
			PutFront(frame_x, frame_y);
			PutFlash();
			PutCaret(frame_x, frame_y);
			PutValueView(frame_x, frame_y);
			PutBossLife();
			PutFade();
			
			if (!(g_GameFlags & 4))
			{
				//Open inventory
				if (gKeyTrg & gKeyItem)
				{
					BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);
					
					switch (CampLoop())
					{
						case 0:
							return 0;
						case 2:
							return 1;
					}
					
					gMC.cond &= ~1;
				}
				else if (gMC.equip & 2 && gKeyTrg & gKeyMap)
				{
					BackupSurface(SURFACE_ID_SCREEN_GRAB, &grcGame);
					
					switch (MiniMapLoop())
					{
						case 0:
							return 0;
						case 2:
							return 1;
					}
				}
			}
			
			if (g_GameFlags & 2)
			{
				if (gKeyTrg & gKeyArms)
					RotationArms();
				else if (gKeyTrg & gKeyArmsRev)
					RotationArmsRev();
			}

			if (swPlay & 1)
			{
				int tscRet = TextScriptProc();
				if (tscRet == 0)
					return 0;
				if (tscRet == 2)
					return 1;
			}
			
			PutMapName(false);
			PutTimeCounter(16, 8);
			
			if (g_GameFlags & 2)
			{
				PutMyLife(true);
				PutArmsEnergy(true);
				PutMyAir((WINDOW_WIDTH - 80) / 2, (WINDOW_HEIGHT - 32) / 2);
				PutActiveArmsList();
			}
			
			if (g_GameFlags & 8)
			{
			  PutIllust();
			  PutStripper();
			}
			
			PutTextScript();
			
			PutFramePerSecound();
			if (!Flip_SystemTask())
			  break;
			++gCounter;
		}
	}
	
	return 0;
}

bool Game()
{
	if (LoadGenericData())
	{
		char path[PATH_LENGTH];
		sprintf(path, "%s/npc.tbl", gDataPath);
		
		if (LoadNpcTable(path))
		{
			InitTextScript2();
			InitSkipFlags();
			InitMapData2();
			InitCreditScript();
			
			
			int mode = 1;
			while (mode)
			{
				if (mode == 1)
					mode = ModeOpening();
				if (mode == 2)
					mode = ModeTitle();
				if (mode == 3)
					mode = ModeAction();
			}
			
			EndMapData();
			EndTextScript();
			ReleaseNpcTable();
			ReleaseCreditScript();
		}
		else
		{
			return false;
		}
	}
	
	return true;
}