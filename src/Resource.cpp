#include "Resource.h"

#include <stddef.h>
#include <string.h>

#ifdef JAPANESE
#include "Resource/FONT/NotoSansMonoCJKjp.otf.h"
#else
#include "Resource/FONT/LiberationMono.ttf.h"
#endif
#ifndef _WIN32
#include "Resource/ICON/ICON_MINI.png.h"
#endif

static const struct
{
	const char *type;
	const char *name;
	const unsigned char *data;
	size_t size;
} resources[] = {
#ifdef JAPANESE
	{"FONT", "FONT", rNotoSansMonoCJKjp, sizeof(rNotoSansMonoCJKjp)},
#else
	{"FONT", "FONT", rLiberationMono, sizeof(rLiberationMono)},
#endif
#ifndef _WIN32
	{"ICON", "ICON_MINI", rICON_MINI, sizeof(rICON_MINI)},
#endif
};

const unsigned char* FindResource(const char *name, const char *type, size_t *size)
{
	for (unsigned int i = 0; i < sizeof(resources) / sizeof(resources[0]); ++i)
	{
		if (!strcmp(name, resources[i].name) && !strcmp(type, resources[i].type))
		{
			if (size)
				*size = resources[i].size;

			return resources[i].data;
		}
	}

	return NULL;
}
