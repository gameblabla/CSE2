#include <stdint.h>

#include <SDL_rwops.h>
#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"
#include "Profile.h"

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
