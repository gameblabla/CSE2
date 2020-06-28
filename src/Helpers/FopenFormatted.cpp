#include "FopenFormatted.h"
#include "../Attributes.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

ATTRIBUTE_FORMAT(printf, 2, 3) ATTRIBUTE_WARN_UNUSED_RESULT FILE *fopenFormatted(const char *mode, const char *format_string, ...)
{
	// Handle variadic arguments and redirect to vasprintf
	va_list arguments_local;
	va_start(arguments_local, format_string);
	char *path;
	int vasprintf_retval = vasprintf(&path, format_string, arguments_local);
	va_end(arguments_local);	// Destroy arguments_local

	if (vasprintf_retval < 0)
		return NULL;

	FILE *file = fopen(path, mode);
	free(path);

	return file;
}
