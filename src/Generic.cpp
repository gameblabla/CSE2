#include "Generic.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "WindowsWrapper.h"

#include "Tags.h"

void GetCompileDate(int *year, int *month, int *day)
{
	int i;
	const char *months[13];
	char month_string[0x10];

	months[0] = "XXX";
	months[1] = "Jan";
	months[2] = "Feb";
	months[3] = "Mar";
	months[4] = "Apr";
	months[5] = "May";
	months[6] = "Jun";
	months[7] = "Jul";
	months[8] = "Aug";
	months[9] = "Sep";
	months[10] = "Oct";
	months[11] = "Nov";
	months[12] = "Dec";
	sscanf(__DATE__, "%s %d %d", month_string, day, year);

	for (i = 0; i < 12; ++i)	// This being 12 instead of 13 might be a bug, but it works anyway by accident
		if (!memcmp(&month_string, months[i], 3))
			break;

	*month = i;
}

BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4)
{
	*v1 = 1;
	*v2 = 0;
	*v3 = 0;
	*v4 = 6;
	return TRUE;
}

void DeleteDebugLog(void)
{
	char path[MAX_PATH];

	sprintf(path, "%s/debug.txt", gModulePath);
	remove(path);
}

BOOL PrintDebugLog(const char *string, int value1, int value2, int value3)
{
	char path[MAX_PATH];
	FILE *fp;

	sprintf(path, "%s/debug.txt", gModulePath);
	fp = fopen(path, "a+");

	if (fp == NULL)
		return FALSE;

	fprintf(fp, "%s,%d,%d,%d\n", string, value1, value2, value3);
	fclose(fp);
	return TRUE;
}

BOOL CheckFileExists(const char *name)
{
	char path[MAX_PATH];

	sprintf(path, "%s/%s", gModulePath, name);

	FILE *file = fopen(path, "rb");

	if (file == NULL)
		return FALSE;

	fclose(file);
	return TRUE;
}

long GetFileSizeLong(const char *path)
{
	long len;
	FILE *fp;

	len = 0;

	fp = fopen(path, "rb");
	if (fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fclose(fp);
	return len;
}

BOOL PrintBitmapError(const char *string, int value)
{
	char path[MAX_PATH];
	FILE *fp;

	sprintf(path, "%s/%s", gModulePath, "error.log");

	if (GetFileSizeLong(path) > 0x19000)	// Purge the error log if it gets too big, I guess
		remove(path);

	fp = fopen(path, "a+");
	if (fp == NULL)
		return FALSE;

	fprintf(fp, "%s,%d\n", string, value);
	fclose(fp);
	return TRUE;
}

BOOL IsShiftJIS(unsigned char c)
{
	if (c >= 0x81 && c <= 0x9F)
		return TRUE;

	if (c >= 0xE0 && c <= 0xEF)
		return TRUE;

	return FALSE;
}

BOOL IsEnableBitmap(const char *path)
{
	char str[16];
	static const char *extra_text = "(C)Pixel";

	const long len = (long)strlen(extra_text);

	FILE *fp = fopen(path, "rb");

	if (fp == NULL)
		return FALSE;

	fseek(fp, len * -1, SEEK_END);
	fread(str, 1, len, fp);
	fclose(fp);

	if (memcmp(str, extra_text, len) != 0)
		return FALSE;
	else
		return TRUE;
}
