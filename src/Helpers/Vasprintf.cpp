#ifndef _GNU_SOURCE

#include "Vasprintf.h"
#include "../Attributes.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

ATTRIBUTE_FORMAT(printf, 2, 0) ATTRIBUTE_WARN_UNUSED_RESULT int vasprintf(char **result_string, const char *format_string, va_list arguments)
{
	const int error_return_value = -1;	// According to the man page, this is returned on error
	va_list arguments_local;
	va_copy(arguments_local, arguments);	// Copy the arguments so we can actually use them

	int size = vsnprintf(NULL, 0, format_string, arguments_local);	// Get the amount of bytes we need for the output buffer

	va_end(arguments_local);	// Destroy arguments_local

	if (size < 0)	// If an output error is encountered, vsnprintf returns a negative value
		return error_return_value;

	*result_string = (char *)malloc(size + 1);	// Allocate enough space for the string (need + 1 for the null terminator)

	if (*result_string == NULL)	// On error, malloc returns NULL
		return error_return_value;

	// We know we have enough space, so we can use vsprintf safely
	return vsprintf(*result_string, format_string, arguments);	// vsprintf returns the amount of characters that got printed. This is what we're supposed to return
}

#endif
