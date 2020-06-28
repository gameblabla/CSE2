#pragma once

// If <stdio.h> defines asprintf for us, use its definition
#ifdef _GNU_SOURCE

#include <stdio.h>

#else

#include "../Attributes.h"
#include <stdarg.h>
#define vasprintf Portable_vasprintf

ATTRIBUTE_FORMAT(printf, 2, 0) ATTRIBUTE_WARN_UNUSED_RESULT int vasprintf(char **resultString, const char *formatString, va_list arguments);

#endif
