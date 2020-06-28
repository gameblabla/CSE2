#include <assert.h>
#if !(_XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200809L)

#include "Strdup.h"
#include "../Attributes.h"
#include <string.h>
#include <stdlib.h>

ATTRIBUTE_MALLOC ATTRIBUTE_NONNULL((1)) char *strdup(const char *duplicated_string)
{
	size_t duplicatedStringLength = strlen(duplicated_string) + sizeof(char);	// Length of the whole string, plus the terminator
	char *returnedString = (char *)malloc(duplicatedStringLength);	// Memory for the new string is obtained with malloc. malloc also sets errno to ENOMEM on failure, which is exactly what we want for conformance

	if (returnedString)	// If the result of malloc was NULL, allocation failed copying into it would be UB
		memcpy(returnedString, duplicated_string, duplicatedStringLength);	// Copy the entire duplicated string (including the null terminator)

	return returnedString;	// Returns a pointer to the duplicated string on success, NULL if insufficient memory was available
}

#endif
