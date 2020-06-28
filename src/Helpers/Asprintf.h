#pragma once

// If <stdio.h> defines asprintf for us, use its definition
#ifdef _GNU_SOURCE

#include <stdio.h>

#else

#include "../Attributes.h"
#define asprintf Portable_asprintf

ATTRIBUTE_FORMAT(printf, 2, 3) ATTRIBUTE_WARN_UNUSED_RESULT int asprintf(char **resultString, const char *formatString, ...);

#endif
