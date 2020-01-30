#pragma once

#ifdef __GNUC__

#define ATTRIBUTE_HOT __attribute__((hot))

#else

#define ATTRIBUTE_HOT

#endif
