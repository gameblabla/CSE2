#pragma once

#include "WindowsWrapper.h"

BOOL GetCompileVersion(int *v1, int *v2, int *v3, int *v4);
long GetFileSizeLong(const char *path);
BOOL CheckFileExists(const char *name);
BOOL IsShiftJIS(unsigned char c);
