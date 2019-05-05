#include "Resource.h"

#include <stddef.h>
#include <string.h>

#ifdef JAPANESE
#include "Resource/FONT/msgothic.ttc.h"
#else
#ifndef WINDOWS
#include "Resource/FONT/cour.ttf.h"
#endif
#endif

static const struct
{
	const char *type;
	const char *name;
	const unsigned char *data;
	size_t size;
} resources[] = {
#ifdef JAPANESE
	{"FONT", "DEFAULT_FONT", rmsgothic, sizeof(rmsgothic)},
#else
#ifndef WINDOWS
	{"FONT", "DEFAULT_FONT", rcour, sizeof(rcour)},
#endif
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
