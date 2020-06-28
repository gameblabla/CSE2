#pragma once
#include <assert.h>	// Probably the smallest header that will include <features.h> on systems with it

// If <string.h> defines strdup for us, use its definition
#if (_XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200809L)

#include <string.h>

#else

#include "../Attributes.h"
#define strdup Portable_strdup

ATTRIBUTE_MALLOC ATTRIBUTE_NONNULL(1) char *strdup(const char *duplicatedString);

#endif
