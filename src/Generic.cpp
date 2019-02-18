#include "Generic.h"

#include <stdint.h>
#include <stdio.h>

#include "CommonDefines.h"
#include "Tags.h"

bool GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	*v1 = 1;
	*v2 = 0;
	*v3 = 0;
	*v4 = 6;
	return true;
}

long GetFileSizeLong(const char *path)
{
	long len = -1;

	FILE *fp = fopen(path, "rb");
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fclose(fp);
	}

	return len;
}

bool CheckFileExists(const char *name)
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gModulePath, name);
	
	FILE *file = fopen(path, "rb");
	if (file)
	{
		fclose(file);
		return true;
	}
	
	return false;
}

bool IsShiftJIS(uint8_t c)
{
	if ( c > 0x80 && c < 0xA0 )
		return true;
	if ( c < 0xE0 || c >= 0xF0 )
		return false;
	return true;
}
