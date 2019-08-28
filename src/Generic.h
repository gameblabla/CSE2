#pragma once

#include "WindowsWrapper.h"

void GetCompileDate(int *year, int *month, int *day);
BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4);
#ifdef WINDOWS
BOOL OpenVolumeConfiguration(HWND hWnd);
void DeleteDebugLog(void);
BOOL PrintDebugLog(const char *string, int value1, int value2, int value3);
int CheckTime(SYSTEMTIME *system_time_low, SYSTEMTIME *system_time_high);
#endif
BOOL CheckFileExists(const char *name);
long GetFileSizeLong(const char *path);
#ifdef WINDOWS
BOOL PrintBitmapError(char *string, int value);
#endif
BOOL IsShiftJIS(unsigned char c);
BOOL CenterWindow(HWND hWnd);
BOOL LoadWindowRect(HWND hWnd, char *filename, BOOL unknown);
BOOL SaveWindowRect(HWND hWnd, const char *filename);
BOOL IsEnableBitmap(const char *path);
