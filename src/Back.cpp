#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "Tags.h"
#include "Back.h"
#include "Draw.h"

BACK gBack;
int gWaterY;

bool InitBack(char *fName, int type)
{
	//Get width and height
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s.pbm", gDataPath, fName);
	
	SDL_Surface *temp = SDL_LoadBMP(path);
	if (!temp)
		return false;
	
	gBack.partsW = temp->w;
	gBack.partsH = temp->h;
	
	SDL_FreeSurface(temp);
	
	//Set background stuff and load texture
	gBack.flag = 1;
	if (!ReloadBitmap_File(fName, 28))
		return false;
	gBack.type = type;
	gWaterY = 0x1E0000;
	return true;
}
