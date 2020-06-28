#ifndef _GNU_SOURCE

#include "Asprintf.h"
#include "Vasprintf.h"
#include "../Attributes.h"
#include <stdarg.h>

ATTRIBUTE_FORMAT(printf, 2, 3) ATTRIBUTE_WARN_UNUSED_RESULT int asprintf(char **result_string, const char *format_string, ...)
{
	// Handle variadic arguments and redirect to vasprintf
	va_list arguments_local;
	va_start(arguments_local, format_string);
	int retVal = vasprintf(result_string, format_string, arguments_local);
	va_end(arguments_local);	// Destroy arguments_local
	return retVal;
}

#endif
