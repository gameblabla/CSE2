#include <stdint.h>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"
#include "Profile.h"
#include "Fade.h"
#include "Flags.h"
#include "MyChar.h"
#include "Frame.h"
#include "Stage.h"
#include "Game.h"

const char *gDefaultName = "Profile.dat";
const char *gProfileCode = "Do041220";

bool IsProfile()
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gModulePath, gDefaultName);
	
	SDL_RWops *fp = SDL_RWFromFile(path, "rb");
	if (!fp)
		return false;
	
	SDL_RWclose(fp);
	return true;
}

bool SaveProfile(char *name)
{
	return false;
}

bool LoadProfile(char *name)
{
	return false;
}

bool InitializeGame()
{
	InitMyChar();
	//gSelectedArms = 0;
	//gSelectedItem = 0;
	gCounter = 0;
	//ClearArmsData();
	//ClearItemData();
	//ClearPermitStage();
	//StartMapping();
	InitFlags();
	if (!TransferStage(13, 200, 10, 8))
		return false;
	ClearFade();
	SetFrameMyChar();
	SetFrameTargetMyChar(16);
	//InitBossLife();
	//CutNoise();
	//ClearValueView();
	//gCurlyShoot_wait = 0;
	SetFadeMask();
	SetFrameTargetMyChar(16);
	return true;
}
