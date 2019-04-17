// sprintfMalloc (because I'm lazy)
// Public domain

#pragma once

#include <stdarg.h>

char* DecoderUtil_vsprintfMalloc(const char *format, va_list args);
char* DecoderUtil_sprintfMalloc(const char *format, ...);
void DecoderUtil_SplitFileExtension(const char *path, char **path_no_extension, char **extension);
