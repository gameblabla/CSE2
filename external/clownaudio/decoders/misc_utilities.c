#include "misc_utilities.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* DecoderUtil_vsprintfMalloc(const char *format, va_list args)
{
	va_list args_copy;

	va_copy(args_copy, args);
	const int string_length = vsnprintf(NULL, 0, format, args_copy) + 1;
	va_end(args_copy);

	char *string = (char*)malloc(string_length);

	vsnprintf(string, string_length, format, args);

	return string;
}

char* DecoderUtil_sprintfMalloc(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	char *string = DecoderUtil_vsprintfMalloc(format, args);

	va_end(args);

	return string;
}

void DecoderUtil_SplitFileExtension(const char *path, char **path_no_extension, char **extension)
{
	// Get filename
	const char *slash1 = strrchr(path, '/');
	const char *slash2 = strrchr(path, '\\');

	if (!slash1)
		slash1 = path - 1;
	if (!slash2)
		slash2 = path - 1;

	const char* const filename = (slash1 > slash2 ? slash1 : slash2) + 1;

	// Get address of extension
	const char *dot = strrchr(filename, '.');

	if (!dot || dot == filename)
		dot = strchr(filename, '\0');

	// Output them
	if (path_no_extension)
	{
		const size_t size = dot - path;
		*path_no_extension = malloc(size + 1);
		memcpy(*path_no_extension, path, size);
		(*path_no_extension)[size] = '\0';
	}

	if (extension)
	{
		*extension = malloc(strlen(dot) + 1);
		strcpy(*extension, dot);
	}
}
