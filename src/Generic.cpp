#include "Generic.h"

#include <stdio.h>

#include "WindowsWrapper.h"

#include "CommonDefines.h"
#include "Tags.h"

BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	*v1 = 1;
	*v2 = 0;
	*v3 = 0;
	*v4 = 6;
	return TRUE;
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

BOOL CheckFileExists(const char *name)
{
	char path[PATH_LENGTH];
	sprintf(path, "%s/%s", gModulePath, name);

	FILE *file = fopen(path, "rb");
	if (file)
	{
		fclose(file);
		return TRUE;
	}

	return FALSE;
}

BOOL IsShiftJIS(unsigned char c)
{
	if (c > 0x80 && c < 0xA0)
		return TRUE;
	if (c < 0xE0 || c >= 0xF0)
		return FALSE;
	return TRUE;
}
