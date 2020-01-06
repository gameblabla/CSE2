#pragma once

#include "WindowsWrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

void GetCompileDate(int *year, int *month, int *day);
BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4);
BOOL OpenVolumeConfiguration(HWND hWnd);
void DeleteDebugLog(void);
BOOL PrintDebugLog(const char *string, int value1, int value2, int value3);
int CheckTime(SYSTEMTIME *system_time_low, SYSTEMTIME *system_time_high);
BOOL CheckFileExists(const char *name);
long GetFileSizeLong(const char *path);
BOOL PrintBitmapError(const char *string, int value);
BOOL IsShiftJIS(unsigned char c);
BOOL CenterWindow(HWND hWnd);
BOOL LoadWindowRect(HWND hWnd, const char *filename, BOOL unknown);
BOOL SaveWindowRect(HWND hWnd, const char *filename);
BOOL IsEnableBitmap(const char *path);

#ifdef __cplusplus
}
#endif
