#pragma once
#include "../Attributes.h"
#include <stdio.h>

ATTRIBUTE_FORMAT(printf, 2, 3) ATTRIBUTE_WARN_UNUSED_RESULT FILE *fopenFormatted(const char *mode, const char *formatString, ...);
