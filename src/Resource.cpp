#include "Resource.h"

#include <stdint.h>
#include <string>

#include <SDL_rwops.h>

#include "WindowsWrapper.h"

const unsigned char* GetResource(const char *name, size_t *size)
{
	return NULL;
}

SDL_RWops* FindResource(const char *name)
{
	size_t resSize;
	const unsigned char* resource = GetResource(name, &resSize);
	
	if (!resource)
		return NULL;
	
	SDL_RWops *fp = SDL_RWFromConstMem(resource, resSize);
	
	if (!fp)
	{
		printf("Couldn't open resource %s\nSDL Error: %s\n", name, SDL_GetError());
		return NULL;
	}
	
	return fp;
}
